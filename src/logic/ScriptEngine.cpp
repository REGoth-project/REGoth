#include "ScriptEngine.h"
#include "PlayerController.h"
#include <ZenLib/daedalus/DATFile.h>
#include <components/VobClasses.h>
#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusVM.h>
#include <engine/GameEngine.h>
#include <engine/World.h>
#include <handle/HandleDef.h>
#include <logic/scriptExternals/Externals.h>
#include <logic/scriptExternals/Stubs.h>
#include <ui/PrintScreenMessages.h>
#include <utils/logger.h>

using namespace Logic;

// Set to 1 to generate valid timing data for script-calls
#define PROFILE_SCRIPT_CALLS 0

ScriptEngine::ScriptEngine(World::WorldInstance& world)
    : m_World(world)
{
    m_pVM = nullptr;
    m_ProfilingDataFrame = 0;
}

ScriptEngine::~ScriptEngine()
{
    delete m_pVM;
}

bool ScriptEngine::loadDAT(const std::string& file)
{
    delete m_pVM;  // FIXME: Should support merging DATS?

    LogInfo() << "Loading Daedalus compiled script file: " << file;

    m_pVM = new Daedalus::DaedalusVM(file);

    // Register externals
    const bool verbose = false;
    Logic::ScriptExternals::registerStubs(*m_pVM, verbose);
    Daedalus::registerGothicEngineClasses(*m_pVM);
    Logic::ScriptExternals::registerEngineExternals(m_World, m_pVM, verbose);

    // Register our externals
    Daedalus::GameState::DaedalusGameState::GameExternals ext;
    ext.wld_insertnpc = [this](Daedalus::GameState::NpcHandle npc, std::string spawnpoint) { onNPCInserted(npc, spawnpoint); };
    ext.post_wld_insertnpc = [this](Daedalus::GameState::NpcHandle npc) { onNPCInitialized(npc); };
    ext.wld_removenpc = [this](Daedalus::GameState::NpcHandle npc) { onNPCRemoved(npc); };
    ext.createinvitem = [this](Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc) { onInventoryItemInserted(item, npc); };

    m_pVM->getGameState().setGameExternals(ext);

    return true;
}

void ScriptEngine::prepareRunFunction()
{
    // Clean the VM for this run
    m_pVM->pushState();
}

int32_t ScriptEngine::runFunction(const std::string& fname, bool clearDataStack)
{
    assert(getVM().getDATFile().hasSymbolName(fname));
    return runFunctionBySymIndex(getVM().getDATFile().getSymbolIndexByName(fname), clearDataStack);
}

int32_t ScriptEngine::runFunctionBySymIndex(size_t symIdx, bool clearDataStack)
{
#if PROFILE_SCRIPT_CALLS
    startProfiling(symIdx);
#endif
    int32_t ret = m_pVM->runFunctionBySymIndex(symIdx, clearDataStack);
#if PROFILE_SCRIPT_CALLS
    stopProfiling(symIdx);
#endif
    return ret;
}

void ScriptEngine::pushInt(int32_t v)
{
    m_pVM->pushInt(v);
}

void ScriptEngine::pushString(const std::string& str)
{
    m_pVM->pushString(str);
}

void ScriptEngine::pushSymbol(size_t sym, uint32_t arrayIndex)
{
    m_pVM->pushVar(sym, arrayIndex);
}

void ScriptEngine::pushSymbol(const std::string& sname)
{
    m_pVM->pushVar(sname);
}

void ScriptEngine::setInstance(const std::string& target, const std::string& source)
{
    // Target is checked later
    assert(m_pVM->getDATFile().hasSymbolName(source));

    setInstance(target,
                m_pVM->getDATFile().getSymbolIndexByName(source));
}

void ScriptEngine::setInstance(const std::string& target, size_t source)
{
    assert(m_pVM->getDATFile().hasSymbolName(target));

    auto& sym = m_pVM->getDATFile().getSymbolByIndex(source);

    m_pVM->setInstance(target, sym.instanceDataHandle, sym.instanceDataClass);
}

void ScriptEngine::setInstanceNPC(const std::string& target, Daedalus::GameState::NpcHandle npc)
{
    assert(m_pVM->getDATFile().hasSymbolName(target));

    m_pVM->setInstance(target, ZMemory::toBigHandle(npc), Daedalus::EInstanceClass::IC_Npc);
}

void ScriptEngine::setInstanceItem(const std::string& target, Daedalus::GameState::ItemHandle item)
{
    assert(m_pVM->getDATFile().hasSymbolName(target));

    m_pVM->setInstance(target, ZMemory::toBigHandle(item), Daedalus::EInstanceClass::IC_Item);
}

void ScriptEngine::initForWorld(const std::string& world, bool firstStart)
{
    if (!m_World.getEngine()->getEngineArgs().cmdline.hasArg('c'))
    {
        if (firstStart && m_pVM->getDATFile().hasSymbolName("startup_" + world))
        {
            LogInfo() << "Running: Startup_" << world;
            prepareRunFunction();
            runFunction("startup_" + world);
            LogInfo() << "Done!";
        }

        if (m_pVM->getDATFile().hasSymbolName("init_" + world))
        {
            LogInfo() << "Running init_" << world;
            prepareRunFunction();
            runFunction("init_" + world);
            LogInfo() << "Done!";
        }
    }
    else
    {
        VobTypes::Wld_InsertNpc(m_World, "PC_THIEF", "WP_INTRO_FALL3");
    }
}

void ScriptEngine::onNPCInserted(Daedalus::GameState::NpcHandle npc, const std::string& spawnpoint)
{
    // LogInfo() << "Created npc " << npc.index <<" on: " << spawnpoint;

    // Create the NPC-vob
    Handle::EntityHandle e = VobTypes::initNPCFromScript(m_World, npc);
    registerNpc(e);

    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);

    if (vob.isValid() && !spawnpoint.empty())
    {
        if (World::Waynet::waypointExists(m_World.getWaynet(), spawnpoint))
            vob.playerController->teleportToWaypoint(World::Waynet::getWaypointIndex(m_World.getWaynet(), spawnpoint));
        else if(m_World.doesFreepointExist(spawnpoint))
        {
            vob.playerController->teleportToPosition(m_World.getFreepointPosition(spawnpoint));
        } else
        {
            LogError() << "spawnpoint does not exist: " << spawnpoint;
            assert(false);
        }
    }
}

Daedalus::GameState::DaedalusGameState& ScriptEngine::getGameState()
{
    return m_pVM->getGameState();
}

size_t ScriptEngine::getSymbolIndexByName(const std::string& name)
{
    return m_pVM->getDATFile().getSymbolIndexByName(name);
}

void ScriptEngine::onInventoryItemInserted(Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc)
{
    Daedalus::GEngineClasses::C_Item& itemData = getGameState().getItem(item);
    //LogInfo() << "Inserted item '" << itemData.name
    //          << "' into the inventory of '" << getGameState().getNpc(npc).name[0] << "'";

    // Equip
    // TODO: Implement this properly

    Handle::EntityHandle e = VobTypes::getEntityFromScriptInstance(m_World, npc);
    if (!e.isValid())
        return;  // FIXME: Happens on windows, wtf?

    if ((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR) != 0)
    {
        //LogInfo() << "Equiping armor... " << itemData.visual_change;
        VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);

        std::string visual = itemData.visual_change.substr(0, itemData.visual_change.size() - 4) + ".MDM";

        // Only switch the body-armor
        VobTypes::NPC_SetBodyMesh(vob, visual);
    }

    if ((itemData.mainflag & (Daedalus::GEngineClasses::C_Item::ITM_CAT_NF | Daedalus::GEngineClasses::C_Item::ITM_CAT_FF)) != 0)
    {
        VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);
        VobTypes::NPC_EquipWeapon(vob, item);
    }
}

void ScriptEngine::onNPCInitialized(Daedalus::GameState::NpcHandle npc)
{
    // Initialize daily routine
    Daedalus::GEngineClasses::C_Npc& npcData = getGameState().getNpc(npc);

    //LogInfo() << "Initializing daily routine for: " << npcData.name[0] << ", hdl: " << npc.index;
    //LogInfo() << "Self: " << getSymbolIndexByName("self");

    if (npcData.daily_routine != 0)
    {
        prepareRunFunction();

        m_pVM->setInstance("self", ZMemory::toBigHandle(npc), Daedalus::IC_Npc);
        m_pVM->setCurrentInstance(getSymbolIndexByName("self"));

        runFunctionBySymIndex(npcData.daily_routine);
    }
}

std::set<Handle::EntityHandle> ScriptEngine::getNPCsInRadius(const Math::float3& center, float radius)
{
    std::set<Handle::EntityHandle> outSet;
    float radSq = radius * radius;

    for (const Handle::EntityHandle& e : m_WorldNPCs)
    {
        Math::float3 translation = m_World.getEntity<Components::PositionComponent>(e).m_WorldMatrix.Translation();

        if ((center - translation).lengthSquared() < radSq)
            outSet.insert(e);
    }

    return outSet;
}

Handle::EntityHandle ScriptEngine::findWorldNPC(const std::string& name)
{
    auto& datFile = getVM().getDATFile();

    for (const Handle::EntityHandle& npc : getWorldNPCs())
    {
        VobTypes::NpcVobInformation npcVobInfo = VobTypes::asNpcVob(m_World, npc);
        if (!npcVobInfo.isValid())
            continue;

        Daedalus::GEngineClasses::C_Npc& npcScriptObject = npcVobInfo.playerController->getScriptInstance();
        std::string npcDisplayName = npcScriptObject.name[0];
        std::string npcDatFileName = datFile.getSymbolByIndex(npcScriptObject.instanceSymbol).name;

        for (auto npcName : {npcDisplayName, npcDatFileName})
        {
            std::replace(npcName.begin(), npcName.end(), ' ', '_');
            if (Utils::stringEqualIngoreCase(name, npcName))
            {
                return npc;
            }
        }
    }
    return Handle::EntityHandle();
}

void ScriptEngine::onLogEntryAdded(const std::string& topic, const std::string& entry)
{
    m_World.getPrintScreenManager().printMessage("Topic: " + topic);
    m_World.getPrintScreenManager().printMessage(entry);
}

bool ScriptEngine::hasSymbol(const std::string& name)
{
    return m_pVM->getDATFile().hasSymbolName(name);
}

Daedalus::GameState::NpcHandle ScriptEngine::getNPCFromSymbol(const std::string& symName)
{
    Daedalus::PARSymbol& sym = m_pVM->getDATFile().getSymbolByName(symName);

    if (sym.instanceDataClass != Daedalus::IC_Npc)
        return Daedalus::GameState::NpcHandle();

    return ZMemory::handleCast<Daedalus::GameState::NpcHandle>(sym.instanceDataHandle);
}

Daedalus::GameState::ItemHandle ScriptEngine::getItemFromSymbol(const std::string& symName)
{
    Daedalus::PARSymbol& sym = m_pVM->getDATFile().getSymbolByName(symName);

    if (sym.instanceDataClass != Daedalus::IC_Item)
        return Daedalus::GameState::ItemHandle();

    return ZMemory::handleCast<Daedalus::GameState::ItemHandle>(sym.instanceDataHandle);
}

void ScriptEngine::registerItem(Handle::EntityHandle e)
{
    m_WorldItems.insert(e);
}

void ScriptEngine::unregisterItem(Handle::EntityHandle e)
{
    m_WorldItems.erase(e);
}

void ScriptEngine::registerMob(Handle::EntityHandle e)
{
    m_WorldMobs.insert(e);
}

void ScriptEngine::unregisterMob(Handle::EntityHandle e)
{
    m_WorldMobs.erase(e);
}

bool ScriptEngine::useItemOn(Daedalus::GameState::ItemHandle hitem, Handle::EntityHandle hnpc)
{
    // Get item data
    Daedalus::GEngineClasses::C_Item& data = getGameState().getItem(hitem);

    // Check if we can even use this item
    if (!data.on_state[0] && !data.on_equip)
    {
        // Nothing to use here
        return false;
    }

    // Push the message to the npc
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, hnpc);

    EventMessages::ManipulateMessage msg;
    msg.targetItem = hitem;

    if (data.on_state[0])
        msg.subType = EventMessages::ManipulateMessage::ST_UseItem;
    else
        msg.subType = EventMessages::ManipulateMessage::ST_EquipItem;

    npc.playerController->getEM().onMessage(msg);
    return true;
}

void ScriptEngine::startProfiling(size_t fnSym)
{
    const int64_t now = bx::getHPCounter();

    // Store starting time
    m_TimeStartStack.push(now);
}

void ScriptEngine::stopProfiling(size_t fnSym)
{
    const double freq = double(bx::getHPFrequency());

    if (m_TimeByFunctionSymbol[m_ProfilingDataFrame].find(fnSym) == m_TimeByFunctionSymbol[m_ProfilingDataFrame].end())
        m_TimeByFunctionSymbol[m_ProfilingDataFrame][fnSym] = 0.0;

    // Make delta-time
    m_TimeByFunctionSymbol[m_ProfilingDataFrame][fnSym] += (bx::getHPCounter() - m_TimeStartStack.top()) / freq;

    m_TimeStartStack.pop();
}

void ScriptEngine::resetProfilingData()
{
    while (!m_TimeStartStack.empty())
        m_TimeStartStack.pop();

    m_TimeByFunctionSymbol[m_ProfilingDataFrame].clear();
}

void ScriptEngine::onFrameStart()
{
#if PROFILE_SCRIPT_CALLS
    m_ProfilingDataFrame = (m_ProfilingDataFrame + 1) % 10;

    resetProfilingData();
#endif
}

void ScriptEngine::onFrameEnd()
{
#if PROFILE_SCRIPT_CALLS
    // Get the 5 most costly calls
    std::vector<std::pair<size_t, double>> calls;
    std::map<size_t, double> combined;

    volatile double sum = 0.0;
    for (int i = 0; i < 10; i++)
    {
        for (auto& p : m_TimeByFunctionSymbol[i])
        {
            if (combined.find(p.first) == combined.end())
                combined[p.first] = 0;

            combined[p.first] += p.second / 10;
        }
    }

    for (auto& p : combined)
    {
        sum += p.second;
        calls.push_back(p);
    }

    // Sort descending
    std::sort(calls.begin(), calls.end(), [](const std::pair<size_t, double>& a, const std::pair<size_t, double>& b) {
        return a.second > b.second;
    });

    bgfx::dbgTextPrintf(60, 0, 0x0f, "Script profiling [ms] (Total: %.3f):", sum * 1000.0);
    for (int i = 0; i < std::min(5, (int)calls.size()); i++)
    {
        std::string name = getVM().getDATFile().getSymbolByIndex(calls[i].first).name;
        bgfx::dbgTextPrintf(60, 1 + i, 0x0f, "  %s: %.3f", name.c_str(), calls[i].second * 1000.0);
    }
#endif
}

void ScriptEngine::exportScriptEngine(json& j)
{
    auto& dat = m_pVM->getDATFile();

    // Walk the symtable and find any non-const integer without any flags
    // Just like the original!

    json& symbols = j["globals"];

    for (auto& sym : dat.getSymTable().symbols)
    {
        // Only flat integers
        if (sym.properties.elemProps.flags == 0 && sym.properties.elemProps.type == Daedalus::EParType_Int)
        {
            // Write arrays in order
            for (int32_t i : sym.intData)
                symbols.push_back({sym.name, i});
        }
    }
}

void ScriptEngine::importScriptEngine(const json& j)
{
    auto& dat = m_pVM->getDATFile();

    // j["globals"]: Array of 2 elements. [0]=SymbolName, [1]=Value

    // Clear any value already inside
    for (const json& p : j["globals"])
    {
        dat.getSymbolByName(p[0]).intData.clear();
    }

    // Assign imported values
    for (const json& p : j["globals"])
    {
        dat.getSymbolByName(p[0]).intData.push_back(p[1]);
    }
}

Handle::EntityHandle ScriptEngine::createDefaultPlayer(const std::string& symbolname)
{
    LogInfo() << "Creating default player";
    if (!hasSymbol(symbolname))
    {
        LogError() << "Error: could not start as NPC with name: " + symbolname;
        return Handle::EntityHandle::makeInvalidHandle();
    }

    std::vector<size_t> startpoints = m_World.findStartPoints();

    if (!startpoints.empty())
    {
        std::string startpoint = m_World.getWaynet().waypoints[startpoints[0]].name;

        LogInfo() << "Inserting player of class '" + symbolname + "' at startpoint '" << startpoint << "'";

        auto entityHandle = VobTypes::Wld_InsertNpc(m_World, symbolname, startpoint);
        // FIXME startpoints are inverted?
        auto playerController = VobTypes::asNpcVob(m_World, entityHandle).playerController;
        playerController->setDirection(-1 * playerController->getDirection());
        return entityHandle;
    }
    return Handle::EntityHandle::makeInvalidHandle();
}

void ScriptEngine::onNPCRemoved(Daedalus::GameState::NpcHandle npc)
{
}

void ScriptEngine::registerNpc(Handle::EntityHandle e)
{
    m_WorldNPCs.insert(e);
}

void ScriptEngine::unregisterNpc(Handle::EntityHandle e)
{
    m_WorldNPCs.erase(e);
}
