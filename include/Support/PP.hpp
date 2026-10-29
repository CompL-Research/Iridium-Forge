#pragma once

#include "Storage/IRIContext.h"
#include "Support/PP/PPContext.hpp"
#include <ostream>
#include <span>
#include <string>
#include <string_view>


//
// IRI Pretty Printer
//
namespace IRI_STRUCTURAL {

std::string ppName(std::string_view raw);
std::string ppStringLit(std::string_view s);
std::string ppNumber(double d);
std::string ppBinding(const PPContext &pp, IRID binding);
std::string ppMember(const PPContext &pp, IRID obj, IRID key);
const char *ppWriteOp(bool init, bool safe);
std::string ppWriteLHS(const PPContext &pp, IRID write);
std::string ppBB(double idx);
std::string ppOperand(const PPContext &pp, IRID node);
std::string ppList(const PPContext &pp, std::span<const IRID> args,
                   size_t from = 0);

void prettyPrint(std::ostream &os, IRI_STORAGE::IRIContext &ctx, IRID root,
                 bool showIds = false);


std::string ppGeneric(const PPContext &pp, IRID node);
} // namespace IRI_STRUCTURAL
