#include <components/VobClasses.h>
#include "Container.h"
#include <logic/PlayerController.h>
#include <ZenLib/utils/logger.h>
#include <stdlib.h>

using namespace Logic;
using namespace MobCores;

Container::Container(World::WorldInstance& world, const Handle::EntityHandle& entity) : MobCore(world, entity)
{

}

Container::~Container()
{

}

void Container::createContents(const std::string& contentString)
{
    std::vector<std::string> items = Utils::split(contentString, ',');

    // Parse items
    for(const std::string& i : items)
    {
        if(i.find_first_of(':') != std::string::npos)
        {
            std::string item = i.substr(0, i.find_first_of(':'));
            std::string num = i.substr(item.size() + 1);

            //LogInfo() << "Chest-put: " << num << "x " << item;
            m_Contents.push_back({item, (unsigned int)atoi(num.c_str())});
        }else
        {
            //LogInfo() << "Chest-put: " << i;
            m_Contents.push_back({i, 1});
        }
    }
}

void Container::onBeginStateChange(Handle::EntityHandle npc, int from, int to)
{
    MobCore::onBeginStateChange(npc, from, to);

    VobTypes::NpcVobInformation n = VobTypes::asNpcVob(m_World, npc);
    if(n.isValid())
    {

    }
}

void Container::onEndStateChange(Handle::EntityHandle npc, int from, int to)
{
    MobCore::onEndStateChange(npc, from, to);

    VobTypes::NpcVobInformation n = VobTypes::asNpcVob(m_World, npc);
    if(n.isValid() && to == 1)
    {
        // TODO: Open chest inventory. Simply giving all the items is temporary!
        for (const Item& item : m_Contents)
        {
            n.playerController->giveItem(item.instance, item.count);
        }
    }

}
