#pragma once
#include "Storage/Config.h"
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IridiumPool;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_CORE_PASSES {

void _1_NBBF(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID sexp,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);
void _2_HFD(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID sexp,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);
void _3_FNOPS(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);
void _4_1_GICG(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _4_2_PMB(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _4_3_PIB(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _4_4_RFD(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _4_5_PEB(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _4_6_CBA(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _5_INITSFRAME(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _6_PHCSC(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _7_RRPEBS(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _8_RREBS(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _9_RLT(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _10_RBACT(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _11_12_DAPRT(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _13_MNSI(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _14_MSW(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _15_LWTA(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _16_MDE(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

void _17_RTDZ(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);


}; // namespace IRI_CORE_PASSES
