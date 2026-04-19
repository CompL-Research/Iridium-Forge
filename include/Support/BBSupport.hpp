#pragma once
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"

namespace IRI_STRUCTURAL {
struct BBSupport : IRI_GEN::BBSEXP {
public:
  BBSupport(IRI_GEN::IRID n, IRI_GEN::IridiumPool &p)
      : IRI_GEN::BBSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> stmts() const;
  std::vector<IRI_GEN::IRID> stmtsVec() const;

  template <typename T>
  static std::vector<T> insert_chunks_after_given_offsets(
      const std::vector<T>& A,
      const std::vector<size_t>& offsets,
      const std::vector<std::vector<T>>& chunks
  ) {
      std::vector<T> result;

      // compute total size once
      size_t extra = 0;
      for (const auto& c : chunks) extra += c.size();
      result.reserve(A.size() + extra);

      size_t p = 0;

      for (size_t i = 0; i < A.size(); ++i) {
          result.push_back(A[i]);

          while (p < offsets.size() && offsets[p] == i) {
              result.insert(result.end(),
                            chunks[p].begin(),
                            chunks[p].end());
              ++p;
          }
      }

      return result;
  }

};
} // namespace IRI_STRUCTURAL
