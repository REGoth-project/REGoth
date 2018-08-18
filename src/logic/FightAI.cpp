#include "FightAI.h"
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <utils/Utils.h>
#include <utils/logger.h>

using namespace Logic;

FightAI::FightAI(World::WorldInstance& world)
    : m_World(world)
{
}

FightAI::~FightAI()
{
}

std::unique_ptr<Daedalus::DaedalusVM> Logic::FightAI::loadFightDat()
{
    std::string datPath = findFightDat();

    if (!Utils::fileExists(datPath))
    {
        LogError() << "Failed to find FIGHT.DAT at: " << datPath;
        return nullptr;
    }

    return std::make_unique<Daedalus::DaedalusVM>(datPath);
}

std::string Logic::FightAI::findFightDat()
{
    std::string datPath = "/_work/data/Scripts/_compiled/FIGHT.DAT";
    std::string base = m_World.getEngine()->getEngineArgs().gameBaseDirectory;
    std::string datFile = Utils::getCaseSensitivePath(datPath, base);

    return datFile;
}

std::vector<std::string> FightAI::dumpAllMoveInstancesToLog()
{
    using namespace Daedalus;
    auto vm = loadFightDat();

    if (!vm)
      return {};

    std::vector<std::string> symnames;
    vm->getDATFile().iterateSymbolsOfClass("C_FightAI",
                                           [&](size_t idx, PARSymbol& sym) {
                                               LogInfo() << " - " << sym.name;
                                               symnames.push_back(sym.name);
                                           });

    return symnames;
}
