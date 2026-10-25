#include "Storage/IRIContext.h"
#include "Storage/IridiumPool.h"
#include "Storage/Config.h"
#include "Storage/GCStats.h"
#include "Support/IRIS.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#if DUMP_CORE_PASSES == 1
#include <fstream>
#endif

namespace IRI_STORAGE {

std::vector<IRID> IridiumPool::collect_roots(IRIContext & ctx, IRID entry) const {
  std::vector<IRID> roots = {
    entry, NULL_SEXP, NOP_SEXP, NUBD_SEXP
  };

  // if IRIS exists, put referenced IRIDS in root set
  if (ctx.iris) {
    auto iris_roots =  ctx.iris->collect_referenced_irids();
    roots.insert(roots.end(), iris_roots.begin(), iris_roots.end());
  }

  return roots;
}

ReachabilityResult IridiumPool::mark_reachable(std::span<const IRID> roots) const {
  ReachabilityResult result;
  result.visited.assign(nodes.size(), false);

  if (nodes.empty()) return result;

  // worklist works like a stack
  std::vector<IRID> worklist;

  for (IRID node : roots) {
    if (!result.visited[node]) {
      result.visited[node] = true;
      worklist.push_back(node);
      result.reachable_count++;
    }
  }

  // visit a node, mark it reachable, go to children (depth-first) and mark accordingly
  while (!worklist.empty()) {
    IRID id = worklist.back();
    worklist.pop_back();

    const IridiumSEXP &node = nodes[id];
    for (IRID child : get_args_view(&node)) {
      if (!result.visited[child]) {
        result.visited[child] = true;
        worklist.push_back(child);
        result.reachable_count++;
      }
    }
  }

  return result;
}

void IridiumPool::print_reachability(const ReachabilityResult &reach) const {
  size_t total = nodes.size();
  size_t reachable = reach.reachable_count;
  size_t unreachable = total - reachable;
  double garbage_pct = total > 0
      ? (static_cast<double>(unreachable) / total) * 100.0
      : 0.0;

  std::cout << "\n------- IridiumPool Reachability --------\n";
  std::cout << "Total nodes:      " << total << std::endl;
  std::cout << "Reachable:        " << reachable << std::endl;
  std::cout << "Unreachable:      " << unreachable << std::endl;
  std::cout << "Garbage %:        " << garbage_pct << "%" << std::endl;

  std::cout << "\n-------- Reachable nodes -------------\n";
  for (size_t i = 0; i < reach.visited.size(); i++) {
    if (reach.visited[i] == true) {
      std::cout << "[" << i << "], ";
    }
  }
  std::cout << std::endl;
}

CompactionResult IridiumPool::compact_pool(ReachabilityResult &reach) {
  // For measurement: set IRI_GC_NOEARLYFREE to keep old storage alive until end,
  // to measure old vs new peak RSS
  static const bool earlyFree = std::getenv("IRI_GC_NOEARLYFREE") == nullptr;

  CompactionResult result;
  // initialize all IRID mappings to -1
  // at last -1 means that is unreachable
  result.old_to_new_irid_map.assign(nodes.size(), static_cast<IRID>(-1));

  // create a copy of nodes containing only the reachable IridiumSEXPs
  std::vector<IridiumSEXP> new_nodes;
  new_nodes.reserve(reach.reachable_count);

  // count the total number of args from all the reachable nodes
  size_t reachable_args_count = 0;

  // count an upper bound of the number of flag slots needed for the reachable nodes
  size_t reachable_flags_upper_bound = 0;

  // Pass 1
  // map old IRIDs to new IRIDs in the new_nodes
  for (size_t old_id = 0; old_id < nodes.size(); old_id++) {
    if (reach.visited[old_id]) {
      result.old_to_new_irid_map[old_id] = static_cast<IRID>(new_nodes.size());
      new_nodes.push_back(nodes[old_id]);
      reachable_args_count += nodes[old_id].num_args;
      reachable_flags_upper_bound += nodes[old_id].num_flag_slots;
    }
  }

  // release old nodes vector's allocated storage
  if (earlyFree) {
    std::vector<IridiumSEXP>().swap(nodes);
  }

  std::vector<IRID> new_args_pool;
  new_args_pool.reserve(reachable_args_count);
  
  // Pass 2
  // Walk new_nodes list and rewrite each node's args
  for (size_t new_id = 0; new_id < new_nodes.size(); new_id++) {
    IridiumSEXP &node = new_nodes[new_id];
    
    // Copying to new_args_pool
    uint32_t start_index = node.args_start_index;
    node.args_start_index = new_args_pool.size();
    
    for (uint32_t offset = 0; offset < node.num_args; offset++) {
      IRID old_child_irid = args_pool[start_index + offset];
      IRID new_child_irid = result.old_to_new_irid_map[old_child_irid];
      new_args_pool.push_back(new_child_irid);
    }
  }

  // release old args_pool vector's allocated storage
  if (earlyFree) {
    std::vector<IRID>().swap(args_pool);
  }
  
  std::vector<FlagValue> new_flags_pool;
  new_flags_pool.reserve(reachable_flags_upper_bound);

  std::unordered_map<std::vector<FlagValue>, uint32_t, FlagVectorHash, FlagVectorEq> new_flag_intern_map;

  // Pass 3
  // Rebuild flags_pool and re-intern flags into the new pool
  for (size_t new_id = 0; new_id < new_nodes.size(); new_id++) {
    IridiumSEXP &node = new_nodes[new_id];
    
    if (node.num_flag_slots == 0) {
      node.flags_start_index = 0;
      continue;
    }

    uint32_t old_flags_start = node.flags_start_index;
    std::vector<FlagValue> flags_content(
        flags_pool.begin() + old_flags_start,
        flags_pool.begin() + old_flags_start + node.num_flag_slots);

    // check whether the flags flags_content is already present in the new_flags_pool
    auto it = new_flag_intern_map.find(flags_content);

    if (it != new_flag_intern_map.end()) { 
      // flags_content found, reuse it
      node.flags_start_index = it->second;

    } else { 
      // flags_content not found, put into new_flags_pool and in the intern map
      uint32_t new_flags_start = static_cast<uint32_t>(new_flags_pool.size());
      new_flags_pool.insert(new_flags_pool.end(), flags_content.begin(), flags_content.end());

      // Record it in the map for future nodes
      new_flag_intern_map[flags_content] = new_flags_start;
      node.flags_start_index = new_flags_start;
    }
  }

  // commit storage copy to actual pool
  nodes = std::move(new_nodes);
  args_pool = std::move(new_args_pool);
  flags_pool = std::move(new_flags_pool);
  flags_pool.shrink_to_fit();

  flag_intern_map = std::move(new_flag_intern_map);

  // mutatedFlags cleared to avoid assuming private flag block after re-interning
  mutatedFlags.clear();

  return result;
}

// b -> before, a -> after
void IridiumPool::print_compaction_result(const std::string &label, const PoolMemory &b, const PoolMemory &a, double ms) const {
  auto pct = [](size_t before, size_t after) {
    return before ? 100.0 * (double(before) - double(after)) / double(before) : 0.0;
  };

  auto kb = [](size_t bytes) { return bytes / 1024.0; };

  std::cout << "\n---- GC: " << label << " (" << std::fixed << std::setprecision(2)
            << ms << " ms) ----\n"
            << std::left << std::setw(16) << "structure" << std::right
            << std::setw(10) << "count" << "    " << std::setw(10) << "count"
            << std::setw(14) << "KB" << "    " << std::setw(10) << "KB"
            << std::setw(9) << "saved" << "\n"
            << std::left << std::setw(16) << "" << std::right
            << std::setw(10) << "before" << "    " << std::setw(10) << "after"
            << std::setw(14) << "before" << "    " << std::setw(10) << "after" << "\n";

  std::cout << std::setprecision(1);
  auto row = [&](const char *name, size_t cb, size_t ca, size_t bb, size_t ba) {
    std::cout << std::left << std::setw(16) << name << std::right
              << std::setw(10) << cb << " -> " << std::setw(10) << ca
              << std::setw(14) << kb(bb) << " -> " << std::setw(10) << kb(ba)
              << std::setw(8) << pct(bb, ba) << "%\n";
  };

  row("nodes",           b.nodeCount,     a.nodeCount,     b.nodesB,  a.nodesB);
  row("args_pool",       b.argCount,      a.argCount,      b.argsB,   a.argsB);
  row("flags_pool",      b.flagCount,     a.flagCount,     b.flagsB,  a.flagsB);
  row("flag_intern_map", b.flagInternEntries, a.flagInternEntries, b.flagInternB, a.flagInternB);

  std::cout << std::left << std::setw(16) << "total (used)" << std::right
            << std::setw(24) << "" << std::setw(14) << kb(b.usedBytes) << " -> "
            << std::setw(10) << kb(a.usedBytes) << std::setw(8)
            << pct(b.usedBytes, a.usedBytes) << "%\n"
            << std::left << std::setw(16) << "total (capacity)" << std::right
            << std::setw(24) << "" << std::setw(14) << kb(b.capBytes) << " -> "
            << std::setw(10) << kb(a.capBytes) << std::setw(8)
            << pct(b.capBytes, a.capBytes) << "%\n";

  std::cout.unsetf(std::ios::floatfield);
  std::cout.precision(6);
}

CompactionResult IridiumPool::garbage_collect(IRIContext &ctx, IRID &root, const std::string &label) {
  const bool stats = statsOn();
  const bool verbose = std::getenv("IRI_GC_VERBOSE") != nullptr;
  const bool measure = stats || verbose;

  PoolMemory b;   // before GC
  std::chrono::steady_clock::time_point t0;
  if (measure) {
    b = memory_usage();
    t0 = std::chrono::steady_clock::now();
  }
  
  std::vector<IRID> roots = collect_roots(ctx, root);
  auto reach = mark_reachable(roots);

  if (verbose) {
    std::cout << "\n" << label << " :\n";
    print_reachability(reach);
    std::ofstream out(label + "_with_ids.iridump");
    get_node(root).dumpFlat(out, &ctx, 0, true, root);
  }

  auto compaction_result = compact_pool(reach);
  root = compaction_result.old_to_new_irid_map[root];
  if (ctx.iris) {
    ctx.iris->remap_irids(compaction_result.old_to_new_irid_map);
  }

  if (measure) {
    double ms = std::chrono::duration<double, std::milli>(
      std::chrono::steady_clock::now() - t0
    ).count();

    PoolMemory a = memory_usage();

    if (verbose) {
      print_compaction_result(label, b, a, ms);
    }

    if (stats) {
      static bool headerDone = false;   // header for CSV once per process

      if (!headerDone) {
        // _b -> before GC, _a -> after GC
        std::cerr << "IRI_STAT_HEADER,label,nodes_b,nodes_a,args_b,args_a,flags_b,flags_a,"
                     "flagIntern_b,flagIntern_a,nodesB_b,nodesB_a,argsB_b,argsB_a,flagsB_b,flagsB_a,"
                     "flagInternB_b,flagInternB_a,used_b,used_a,cap_b,cap_a,ms\n";
        headerDone = true;
      }

      std::cerr << "IRI_STAT," << label
        << "," << b.nodeCount << "," << a.nodeCount
        << "," << b.argCount  << "," << a.argCount
        << "," << b.flagCount << "," << a.flagCount
        << "," << b.flagInternEntries << "," << a.flagInternEntries
        << "," << b.nodesB << "," << a.nodesB
        << "," << b.argsB  << "," << a.argsB
        << "," << b.flagsB << "," << a.flagsB
        << "," << b.flagInternB << "," << a.flagInternB
        << "," << b.usedBytes << "," << a.usedBytes
        << "," << b.capBytes  << "," << a.capBytes
        << "," << ms << "\n";
    }
  }

  return compaction_result;
}

PoolMemory IridiumPool::memory_usage() const {
  PoolMemory m;

  // count of elements in each storage structure
  m.nodeCount = nodes.size();
  m.argCount = args_pool.size();
  m.flagCount = flags_pool.size();
  m.flagInternEntries = flag_intern_map.size();

  // size in bytes of each storage structure
  m.nodesB = m.nodeCount * sizeof(IridiumSEXP);
  m.argsB = m.argCount * sizeof(IRID);
  m.flagsB = m.flagCount * sizeof(FlagValue);

  // hash map keeps an array of buckets, one pointer each (even empty buckets)
  // this is the cost of bucket array itself
  size_t flagInternB = flag_intern_map.bucket_count() * sizeof(void *);

  // walk every entry in the map and add up bytes in flagInternB
  for (const auto &[k, v] : flag_intern_map) {
    flagInternB += k.capacity() * sizeof(FlagValue) // key's heap buffer
                   + sizeof(k)  // vector object itself
                   + sizeof(v)  // the value
                   + 2 * sizeof(void *);   // approx per-entry overhead (next pointer, hash code)
  }
  m.flagInternB = flagInternB;

  // capacity in bytes allocated for each storage structure
  m.nodesCapB = nodes.capacity() * sizeof(IridiumSEXP);
  m.argsCapB = args_pool.capacity() * sizeof(IRID);
  m.flagsCapB = flags_pool.capacity() * sizeof(FlagValue);

  // total bytes
  m.usedBytes = m.nodesB + m.argsB + m.flagsB + m.flagInternB;
  m.capBytes = m.nodesCapB + m.argsCapB + m.flagsCapB + m.flagInternB;
  
  return m;
}

}
