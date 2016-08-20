//
// Created by desktop on 11.08.16.
//

#include "DialogManager.h"
#include <engine/World.h>
#include <utils/logger.h>
#include <ui/DialogBox.h>
#include <components/VobClasses.h>
#include <engine/BaseEngine.h>
#include <logic/messages/EventManager.h>
#include <logic/PlayerController.h>

/**
 * File containing the dialouges
 * // TODO: Be able to read this from VDF
 */
const char* OU_FILE = "_work/DATA/scripts/content/CUTSCENE/OU.BIN";

using namespace Logic;

DialogManager::DialogManager(World::WorldInstance& world) :
    m_World(world)
{
    m_ActiveDialogBox = nullptr;
    m_ScriptDialogMananger = nullptr;
    m_ActiveSubtitleBox = nullptr;
}

DialogManager::~DialogManager()
{
    delete m_ScriptDialogMananger;

    endDialog();

    m_World.getEngine()->getRootUIView().removeChild(m_ActiveSubtitleBox);
    delete m_ActiveSubtitleBox;
    m_ActiveSubtitleBox = nullptr;
}

void DialogManager::onAIProcessInfos(Daedalus::GameState::NpcHandle self,
                                     std::vector<Daedalus::GameState::InfoHandle> infos)
{
    if(m_ActiveDialogBox)
        return; // Can't open two dialog-boxes at once

    // Set information about the current interaction
    m_Interaction.target = self;
    m_Interaction.infos = infos;

    // Get interaction partner ("other")
    m_Interaction.player = ZMemory::handleCast<Daedalus::GameState::NpcHandle>(
            getVM().getDATFile().getSymbolByName("other").instanceDataHandle);

    if(infos.empty())
        return;

    LogInfo() << "Started talking with: " << getGameState().getNpc(m_Interaction.target).name[0];
    LogInfo() << "Options: ";

    // Acquire all information we should be able to see right now
    m_Interaction.optionsSorted.clear();
    size_t importantChoice = static_cast<size_t>(-1);
    int32_t importantNr = INT32_MAX;
    for(size_t i=0;i<infos.size();i++)
    {
        Daedalus::GEngineClasses::C_Info& info = getVM().getGameState().getInfo(infos[i]);

        // Check condition
        std::string vstr;
        int32_t valid = 0;

        // Test if we should be able to see this info
        if(info.condition)
        {
            m_World.getScriptEngine().prepareRunFunction();
            valid = m_World.getScriptEngine().runFunction(info.condition);
        }

        if(valid)
        {
            if(!info.description.empty()/* || info.important*/)
                m_Interaction.optionsSorted.push_back(std::make_pair(info.nr, i));

            if(info.important)
            {
                if(importantNr > info.nr)
                {
                    importantChoice = i;
                    importantNr = info.nr;
                }
            }
        }
    }

    // Sort by importance index
    std::sort(m_Interaction.optionsSorted.begin(), m_Interaction.optionsSorted.end(), [](const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b){
        return a.first < b.first;
    });

    // Find our important option again
    // FIXME: This is pretty hacky
    for(size_t i=0;i<m_Interaction.optionsSorted.size();i++)
    {
        if(m_Interaction.optionsSorted[i].second == importantChoice)
        {
            importantChoice = i;
            break;
        }
    }

    // FIXME: Debugoutput
    std::vector<std::string> choices;
    for(size_t i=0;i<m_Interaction.optionsSorted.size();i++) {
        auto& info = getGameState().getInfo(infos[m_Interaction.optionsSorted[i].second]);
        LogInfo() << " - [" << i + 1 << "]" << ": " << info.description;

        choices.push_back(info.description);
    }

    // Open dialog box
    m_ActiveDialogBox = new UI::DialogBox();
    m_ActiveDialogBox->fillChoices(choices);

    m_World.getEngine()->getRootUIView().addChild(m_ActiveDialogBox);

    // Do the important choice right now
    // TODO: Let the NPC begin talking in that case
   /* if(importantChoice != static_cast<size_t>(-1))
    {
        if(!performChoice(importantChoice))
        {
            // END was chosen, don't continue the dialog
            endDialog();
        }
    }*/
}

void DialogManager::onAIOutput(Daedalus::GameState::NpcHandle self, Daedalus::GameState::NpcHandle target,
                               const ZenLoad::oCMsgConversationData& msg)
{
    m_ActiveSubtitleBox->setText(getGameState().getNpc(self).name[0], msg.text);

    // Make a new message for the talking NPC
    VobTypes::NpcVobInformation selfnpc = VobTypes::getVobFromScriptHandle(m_World, self);
    VobTypes::NpcVobInformation targetnpc = VobTypes::getVobFromScriptHandle(m_World, target);

    EventMessages::ConversationMessage conv;
    conv.subType = EventMessages::ConversationMessage::ST_Output;
    conv.target = targetnpc.entity;
    conv.name = msg.name;
    conv.text = msg.text;

    // Check if the target is currently talking to us
    EventMessages::EventMessage* otherconv = targetnpc.playerController->getEM().getTalkingWithMessage(selfnpc.entity);

    // Wait for the other npc to complete first
    if(otherconv)
        selfnpc.playerController->getEM().waitForMessage(otherconv);

    // Push the actual conversation-message
    selfnpc.playerController->getEM().onMessage(conv);
}


void DialogManager::update(double dt)
{
    if(m_ActiveDialogBox)
    {
        m_ActiveDialogBox->setHidden(!m_ActiveSubtitleBox->isHidden());

        if (m_ActiveDialogBox->getChoiceTaken() != -1)
        {
            // Perform the choice and check if that was an END-choice
            if (!performChoice(static_cast<size_t>(m_ActiveDialogBox->getChoiceTaken())))
            {
                // END was chosen, don't continue the dialog
                endDialog();
            } else
            {
                // There is more! Start talking again.
                endDialog();
                startDialog(m_Interaction.target);
            }
        }
    }
}

Daedalus::DaedalusVM& DialogManager::getVM()
{
    return m_World.getScriptEngine().getVM();
}

Daedalus::GameState::DaedalusGameState& DialogManager::getGameState()
{
    return m_World.getScriptEngine().getGameState();
}

bool DialogManager::performChoice(size_t choice)
{
    assert(choice < m_Interaction.optionsSorted.size());

    // Hide the options box
    endDialog();

    // Get actual selected info-object
    Daedalus::GEngineClasses::C_Info& info = getGameState().getInfo(m_Interaction.infos[m_Interaction.optionsSorted[choice].second]);

    // Call the script routine attached to the choice
    m_World.getScriptEngine().prepareRunFunction();
    m_World.getScriptEngine().runFunction(info.information);

    // We now know this information
    m_ScriptDialogMananger->setNpcInfoKnown(getGameState().getNpc(m_Interaction.player).instanceSymbol, info.instanceSymbol);

    m_World.getScriptEngine().prepareRunFunction();

    // Finish dialog
    // TODO: Find out how these "loop"-functions work
    m_World.getScriptEngine().runFunction(getVM().getDATFile().getSymbolByName("ZS_Talk_Loop").address);

    return info.nr != 999;

}

void DialogManager::startDialog(Daedalus::GameState::NpcHandle target)
{
    Handle::EntityHandle playerEntity = m_World.getScriptEngine().getPlayerEntity();
    VobTypes::NpcVobInformation playerVob = VobTypes::asNpcVob(m_World, playerEntity);

    // Self is the NPC we're talking to here. Kind of reversed, but what do I know.
    getVM().setInstance("self", ZMemory::toBigHandle(target), Daedalus::IC_Npc);
    getVM().setInstance("other", ZMemory::toBigHandle(VobTypes::getScriptHandle(playerVob)), Daedalus::IC_Npc);

    m_World.getScriptEngine().prepareRunFunction();
    m_World.getScriptEngine().runFunction(getVM().getDATFile().getSymbolByName("ZS_Talk").address);
}

void DialogManager::endDialog()
{
    m_World.getEngine()->getRootUIView().removeChild(m_ActiveDialogBox);

    delete m_ActiveDialogBox;
    m_ActiveDialogBox = nullptr;
}

void DialogManager::init()
{
    m_ScriptDialogMananger = new Daedalus::GameState::DaedalusDialogManager(getVM(), m_World.getEngine()->getEngineArgs().gameBaseDirectory + OU_FILE);

    // Register externals
    auto onAIOutput = [&](Daedalus::GameState::NpcHandle self, Daedalus::GameState::NpcHandle target, const ZenLoad::oCMsgConversationData& msg)
    {
        LogInfo() << getGameState().getNpc(self).name[0] << ": " << msg.text;
        DialogManager::onAIOutput(self, target, msg);
    };

    auto onAIProcessInfos = [&](Daedalus::GameState::NpcHandle self, std::vector<Daedalus::GameState::InfoHandle> infos)
    {
        DialogManager::onAIProcessInfos(self, infos);
    };

    m_ScriptDialogMananger->registerExternals(onAIOutput, onAIProcessInfos);

    // Add subtitle box (Hidden if there is nothing to display)
    m_ActiveSubtitleBox = new UI::SubtitleBox();
    m_World.getEngine()->getRootUIView().addChild(m_ActiveSubtitleBox);
    m_ActiveSubtitleBox->setHidden(true);
}

void DialogManager::displaySubtitle(const std::string& subtitle, const std::string& self)
{
    m_ActiveSubtitleBox->setHidden(false);
    m_ActiveSubtitleBox->setText(self, subtitle);
}

void DialogManager::stopDisplaySubtitle()
{
    m_ActiveSubtitleBox->setHidden(true);
}
