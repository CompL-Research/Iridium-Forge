#pragma once
#include "Storage/Config.h"
#include "Support/IRICFG.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace IRI_STORAGE {
class IRIContext;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {
class TryStructure;
}

namespace IRI_CORE_PASSES {

using IRI_STORAGE::BBIDX;
using IRI_STORAGE::IRIContext;
using IRI_STORAGE::IRID;
using IRI_STORAGE::NO_BB;
using IRI_STORAGE::NO_SCOPE;
using IRI_STORAGE::SCPIDX;
using IRI_CONTEXTMAP =
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>;

using CTARGETS = std::unordered_map<IRI_STORAGE::IRID, IRI_STORAGE::BBIDX>;

// Remember the declaration statements explicitly, used by our assertion pass
// later... Might actually be redundant but that assertion pass has helped hunt
// many bugs, so we'll keep this
using DECLSTORES = std::unordered_set<IRI_STORAGE::IRID>;

void _1_NBBF(IRIContext &ctx, IRID sexp, IRI_CONTEXTMAP &iridiumBuildContext);
void _2_HFD(IRIContext &ctx, IRID sexp, IRI_CONTEXTMAP &iridiumBuildContext);
void _FNOPS(IRIContext &ctx, IRID fileSEXP,
            IRI_CONTEXTMAP &iridiumBuildContext);
void _4_1_1_GICG(IRIContext &ctx, IRID fileSEXP,
                 IRI_CONTEXTMAP &iridiumBuildContext);

void _4_1_2_RUR(IRIContext &ctx, IRID fileSEXP,
                IRI_CONTEXTMAP &iridiumBuildContext);

void _4_2_PMB(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _4_3_PIB(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext, DECLSTORES &declStores);

void _4_5_PEB(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext, DECLSTORES &declStores);

void _4_4_RFD(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext, DECLSTORES &declStores);

void _4_6_CBA(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _5_INITSFRAME(IRIContext &ctx, IRID fileSEXP,
                   IRI_CONTEXTMAP &iridiumBuildContext);

void _8_RREBS(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext, DECLSTORES &declStores);

CTARGETS
_10_RBACT(IRIContext &ctx, IRID fileSEXP, IRI_CONTEXTMAP &iridiumBuildContext);

void _11_12_DAPRT(IRIContext &ctx, IRID fileSEXP,
                  IRI_CONTEXTMAP &iridiumBuildContext);

void _13_MNSI(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _14_MSW(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext);

void _15_LWTA(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _16_MDE(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext);

void _17_RTDZ(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _18_DELOP(IRIContext &ctx, IRID fileSEXP,
               IRI_CONTEXTMAP &iridiumBuildContext);

void _19_CBBAMTLA(IRIContext &ctx, IRID fileSEXP,
                  IRI_CONTEXTMAP &iridiumBuildContext);

void _20_REW(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext);

void _20_IFB(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext, CTARGETS &continueTargets,
             IRI_STRUCTURAL::TryStructure &tryInfo);

void _21_TER(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext);

void _22_ESTKTHM(IRIContext &ctx, IRID fileSEXP,
                 IRI_CONTEXTMAP &iridiumBuildContext, CTARGETS);

void _23_APVK(IRIContext &ctx, IRID fileSEXP,
              IRI_CONTEXTMAP &iridiumBuildContext);

void _23_RIB(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext);

void _24_RCW(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext, DECLSTORES &declStores);

}; // namespace IRI_CORE_PASSES
