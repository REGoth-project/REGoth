#include "ScriptEngine.h"
#include "PlayerController.h"
#include <daedalus/DaedalusVM.h>
#include <utils/logger.h>
#include <logic/scriptExternals/Externals.h>
#include <logic/scriptExternals/Stubs.h>
#include <daedalus/DaedalusGameState.h>
#include <handle/HandleDef.h>
#include <components/VobClasses.h>
#include <engine/World.h>
#include <engine/GameEngine.h>
#include <ui/PrintScreenMessages.h>

using namespace Logic;

ScriptEngine::ScriptEngine(World::WorldInstance& world)
    : m_World(world)
{
    m_pVM = nullptr;
}

ScriptEngine::~ScriptEngine()
{
    delete m_pVM;
}

bool ScriptEngine::loadDAT(const std::string& file)
{
    delete m_pVM; // FIXME: Should support merging DATS?

    LogInfo() << "Loading Daedalus compiled script file: " << file;

    Daedalus::DATFile dat = Daedalus::DATFile(file);
    m_pVM = new Daedalus::DaedalusVM(dat);


    // Register externals
    const bool verbose = false;
    Logic::ScriptExternals::registerStubs(*m_pVM, verbose);
    Logic::ScriptExternals::registerStdLib(*m_pVM, verbose);
    m_pVM->getGameState().registerExternals();
    Logic::ScriptExternals::registerEngineExternals(m_World, m_pVM, verbose);
    return false;
}

void ScriptEngine::prepareRunFunction()
{
    // Clean the VM for this run
    m_pVM->pushState();

    // Init stack with a value of 0. Some functions do not return, and thus, a default must be given.
    pushInt(0);
}


int32_t ScriptEngine::runFunction(const std::string& fname)
{
    assert(m_pVM->getDATFile().hasSymbolName(fname));

    return runFunction(m_pVM->getDATFile().getSymbolByName(fname).address);
}

int32_t ScriptEngine::runFunction(size_t addr)
{
	if(addr == 0)
		return -1;

    // Place the call-operation
    m_pVM->doCallOperation(addr);

    // Execute the instructions
    while(m_pVM->doStack());

    int32_t ret = 0;

    // Only pop if the VM didn't mess up
    if(!m_pVM->isStackEmpty())
        ret = m_pVM->popDataValue();
    else
        LogWarn() << "DaedalusVM: Safety int was popped by scriptcode!";

    // Restore to previous VM-State
    m_pVM->popState();
    return ret;
}

int32_t ScriptEngine::runFunctionBySymIndex(size_t symIdx)
{
    return runFunction(getVM().getDATFile().getSymbolByIndex(symIdx).address);
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


void ScriptEngine::initForWorld(const std::string& world)
{
    // Register our externals
    Daedalus::GameState::DaedalusGameState::GameExternals ext;
    ext.wld_insertnpc = [this](Daedalus::GameState::NpcHandle npc, std::string spawnpoint){ onNPCInserted(npc, spawnpoint); };
    ext.post_wld_insertnpc = [this](Daedalus::GameState::NpcHandle npc){ onNPCInitialized(npc); };
    ext.createinvitem = [this](Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc){ onItemInserted(item, npc); };
    ext.log_addentry = [this](std::string topic, std::string entry){ onLogEntryAdded(topic, entry); };

    m_pVM->getGameState().setGameExternals(ext);

    // FIXME: Call the startup-one only on a fresh load of the game
    if(m_pVM->getDATFile().hasSymbolName("startup_" + world))
    {
        prepareRunFunction();
        runFunction("startup_" + world);
    }

    if(m_pVM->getDATFile().hasSymbolName("init_" + world))
    {
        prepareRunFunction();
        runFunction("init_" + world);
    }

    // Create player
    std::vector<size_t> startpoints = m_World.findStartPoints();

    if(!startpoints.empty())
    {
        std::string startpoint = m_World.getWaynet().waypoints[startpoints[0]].name;

        LogInfo() << "Inserting player of class 'PC_HERO' at startpoint '" << startpoint << "'";

        m_PlayerEntity = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", startpoint); // FIXME: Read startpoint at levelchange

		if(!m_PlayerEntity.isValid()){
			LogWarn() << "Failed to insert player!";
		}
		else
		{
			VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_PlayerEntity);

			Daedalus::GameState::NpcHandle hsnpc =  VobTypes::getScriptHandle(npc);

            // Register as hero
            getVM().setInstance("hero", ZMemory::toBigHandle(hsnpc), Daedalus::IC_Npc);

            // Equip with a sword
			Daedalus::GameState::ItemHandle sword = getGameState().addInventoryItem(m_pVM->getDATFile().getSymbolIndexByName("ItMw_1H_Sword_Short_04"), hsnpc);

			if(sword.isValid())
				VobTypes::NPC_EquipWeapon(npc, sword);
		}

        Engine::GameEngine* e = reinterpret_cast<Engine::GameEngine*>(m_World.getEngine());
        e->getMainCameraController()->setTransforms(m_World.getWaynet().waypoints[startpoints[0]].position);
        e->getMainCameraController()->setCameraMode(Logic::CameraController::ECameraMode::ThirdPerson);
    }
}

void ScriptEngine::onNPCInserted(Daedalus::GameState::NpcHandle npc, const std::string& spawnpoint)
{
    // LogInfo() << "Created npc " << npc.index <<" on: " << spawnpoint;

    // Create the NPC-vob
    Handle::EntityHandle e = VobTypes::initNPCFromScript(m_World, npc);
    Vob::VobInformation v = Vob::asVob(m_World, e);
    m_WorldNPCs.insert(e);


    // Place NPC to it's location
    // TODO: Find some better solution to the casting
    Logic::PlayerController* pc = reinterpret_cast<Logic::PlayerController*>(v.logic);

    //LogInfo() << "Spawnpoint: " << spawnpoint;

    // FIXME: Some waypoints don't seem to exist?
    if(World::Waynet::waypointExists(m_World.getWaynet(), spawnpoint))
        pc->teleportToWaypoint(World::Waynet::getWaypointIndex(m_World.getWaynet(), spawnpoint));
}

Daedalus::GameState::DaedalusGameState& ScriptEngine::getGameState()
{
    return m_pVM->getGameState();
}

size_t ScriptEngine::getSymbolIndexByName(const std::string& name)
{
    return m_pVM->getDATFile().getSymbolIndexByName(name);
}

void ScriptEngine::onItemInserted(Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc)
{
    Daedalus::GEngineClasses::C_Item& itemData = getGameState().getItem(item);
    //LogInfo() << "Inserted item '" << itemData.name
    //          << "' into the inventory of '" << getGameState().getNpc(npc).name[0] << "'";

    // Equip
    // TODO: Implement this properly
    if((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_ARMOR) != 0)
    {
        //LogInfo() << "Equiping armor... " << itemData.visual_change;
		Handle::EntityHandle e = VobTypes::getEntityFromScriptInstance(m_World, npc);

		if(!e.isValid())
			return; // FIXME: Happens on windows, wtf?

        VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);

        std::string visual = itemData.visual_change.substr(0, itemData.visual_change.size()-4) + ".MDM";

        // Only switch the body-armor
        VobTypes::NPC_SetBodyMesh(vob, visual);
    }

    /*if((itemData.mainflag & Daedalus::GEngineClasses::C_Item::ITM_CAT_NF) != 0)
    {
        Handle::EntityHandle e = VobTypes::getEntityFromScriptInstance(m_World, npc);

        if(!e.isValid())
            return; // FIXME: Happens on windows, wtf?

        VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, e);
        VobTypes::NPC_EquipWeapon(vob, item);

    }*/
}

void ScriptEngine::onNPCInitialized(Daedalus::GameState::NpcHandle npc)
{
    // Initialize daily routine
    Daedalus::GEngineClasses::C_Npc& npcData = getGameState().getNpc(npc);

    //LogInfo() << "Initializing daily routine for: " << npcData.name[0] << ", hdl: " << npc.index;
    //LogInfo() << "Self: " << getSymbolIndexByName("self");


	if(npcData.daily_routine != 0)
	{
		prepareRunFunction();

		m_pVM->setInstance("self", ZMemory::toBigHandle(npc), Daedalus::IC_Npc);
		m_pVM->setCurrentInstance(getSymbolIndexByName("self"));

		runFunction(npcData.daily_routine);
	}
}

std::set<Handle::EntityHandle> ScriptEngine::getNPCsInRadius(const Math::float3 &center, float radius)
{
    std::set<Handle::EntityHandle> outSet;
    float radSq = radius * radius;

    for(const Handle::EntityHandle& e : m_WorldNPCs)
    {
        Math::float3 translation = m_World.getEntity<Components::PositionComponent>(e).m_WorldMatrix.Translation();

        if((center - translation).lengthSquared() < radSq)
            outSet.insert(e);
    }

    return outSet;
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

    if(sym.instanceDataClass != Daedalus::IC_Npc)
        return Daedalus::GameState::NpcHandle();

    return ZMemory::handleCast<Daedalus::GameState::NpcHandle>(sym.instanceDataHandle);
}

Daedalus::GameState::ItemHandle ScriptEngine::getItemFromSymbol(const std::string& symName)
{
    Daedalus::PARSymbol& sym = m_pVM->getDATFile().getSymbolByName(symName);

    if(sym.instanceDataClass != Daedalus::IC_Npc)
        return Daedalus::GameState::ItemHandle();

    return ZMemory::handleCast<Daedalus::GameState::ItemHandle>(sym.instanceDataHandle);
}




























