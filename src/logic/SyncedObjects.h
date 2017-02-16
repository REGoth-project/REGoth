#pragma once
#include <logic/PlayerInput.h>
#include <SFNUL.hpp>
#include <handle/HandleDef.h>
#include <utils/staticReferencedAllocator.h>
#include <daedalus/DATFile.h>
#include <daedalus/DaedalusStdlib.h>

namespace sfn
{
    //sfn::Message& operator<<( sfn::Message& message, const blatype& input) { return message; }  
    //sfn::Message& operator>>( sfn::Message& message, const blatype& input) { return message; }  

    sfn::Message& operator<<( sfn::Message& message, const Logic::PlayerInput& input);
    sfn::Message& operator>>( sfn::Message& message, Logic::PlayerInput& output);

    sfn::Message& operator<<( sfn::Message& message, const Math::float3& input);
    sfn::Message& operator>>( sfn::Message& message, Math::float3& output);

    sfn::Message& operator<<( sfn::Message& message, const Math::float2& input);
    sfn::Message& operator>>( sfn::Message& message, Math::float2& output);

    sfn::Message& operator<<( sfn::Message& message, const Math::Matrix& input);
    sfn::Message& operator>>( sfn::Message& message, Math::Matrix& output);

    sfn::Message& operator<<( sfn::Message& message, const Handle::EntityHandle& input);
    sfn::Message& operator>>( sfn::Message& message, Handle::EntityHandle& output);

    template<typename T, int N>
    sfn::Message& operator<<(sfn::Message& message, const T (&input)[N])
    {
        for(int i=0;i<N;i++)
            message << input[i];

        return message;
    }

    template<typename T, int N>
    sfn::Message& operator>>(sfn::Message& message, T (&output)[N])
    {
        for(int i=0;i<N;i++)
            message >> output[i];

        return message;
    }

    // Implemented in NetScriptEngine.cpp
    sfn::Message& operator<<(sfn::Message& message, const ZMemory::BigHandle& input);
    sfn::Message& operator>>(sfn::Message& message, ZMemory::BigHandle& output);

    sfn::Message& operator<<(sfn::Message& message, const Daedalus::PARSymbol& input);
    sfn::Message& operator>>(sfn::Message& message, Daedalus::PARSymbol& output);

    sfn::Message& operator<<(sfn::Message& message, const Daedalus::GEngineClasses::C_Npc& input);
    sfn::Message& operator>>(sfn::Message& message, Daedalus::GEngineClasses::C_Npc& output);
}

namespace World
{
    class WorldInstance;
}

namespace Net
{
    enum EntityType
    {
        ET_Item,
    };

    // Our IDs for the different streams.
    enum StreamID : sfn::LinkBase::stream_id_type {
        Default = 0, // You shouldn't touch this unless you don't use the default stream anyway.
        PlayerInputStream = 1,
        LoginStream = 2,
        ChatStream = 3,
        ScriptStream = 4,
        WorldSyncStream = 5,
        PlayerActionsStream = 6,
        SyncStream = 200 // DO NOT TOUCH STREAM IDS >= 200 They are used for internal purposes.
    };

    enum LoginPacket : uint16_t {
        LP_Error = 0,
        LP_AskPlayerID = 1, // Client asks for a player ID. Response: Packet, PlayerID, Character-Serverhandle
    };

    enum ChatPacket : uint16_t {
        CP_Message = 0
    };

    enum ScriptPacket : uint16_t {
        SP_SymChanged = 0,          // Packet, Sym-Index, Sym-Data
        SP_InstanceChanged = 1,     // Packet, Instance-class, Instance-Handle, Instance-Data
        SP_SymInstanceChanged = 2,  // Packet, Instance-class, Instance-Handle
        SP_InstanceCreated = 3,     // Packet, Instance-class, Instance-Handle
        SP_InstanceRemoved = 4,     // Packet, Instance-class, Instance-Handle
        SP_NPC_Created = 5,         // Packet, Instance-Symbol, Instance-Handle, Startpoint
        SP_Client_NPC_Assigned = 6, // Packet, Serverhandle
        SP_NPC_Teleport = 7,        // Packet, Instance-Handle, New-Position, New-Direction
        SP_NPC_Killed = 8,          // Packet, Serverhandle(Killed), Serverhandle(Killer)
        SP_WorldTimeSync = 9,       // Packet, time(float)
        SP_Item_Created = 10,       // Packet, (Server)Entity-handle, instance-idx, transform
        SP_Item_Removed = 11,       // Packet, (Server)Entity-handle
        SP_NPC_AddInventory = 12,   // Packet, Serverhandle, Instance, Count
        SP_NPC_PlayAnim = 13,       // Packet, Serverhandle, Anim-Name
        SP_NPC_Interrupt = 14,      // Packet, Serverhandle
        SP_NPC_StartDialog = 15,    // Packet, Serverhandle(dialogue-partner), [Info-Instances]
        SP_NPC_AIOutput = 16,       // Packet, Serverhandle(source), Serverhandle(target), OU-Name, Text
    };

    enum PlayerActionPacket : uint16_t
    {
        PA_NPC_Killed = 0,          // Packet, Serverhandle(Killed), Serverhandle(Killer)
        PA_Item_Taken = 1,          // Packet, Serverentity(Item)
        PA_Play_Animation = 2,      // Packet, Animation-name
        PA_Interrupt = 3,           // Packet
        PA_AI_Output = 4,           // Packet, Serverhandle(target)(Can be invalid), OU-Name, Text
    };

    // Max number of chat messages sent to the clients
    const size_t MAX_CHAT_HISTORY_SIZE = 6;

    // Max number of characters in a message
    const size_t MAX_CHAT_MESSAGE_SIZE = 1024;

    class Chatlog : public sfn::SyncedObject
    {
    public:

        // SFNUL ID
        const static object_type_id_type type_id;

        Chatlog() :
                m_Messages{this}
        {
        }

        Chatlog(const Chatlog& other) :
                m_Messages{this, other.m_Messages},
                sfn::SyncedObject()
        {
        }

        Chatlog(Chatlog&& other) :
        sfn::SyncedObject{ std::forward<sfn::SyncedObject>(other) },
        m_Messages{this, other.m_Messages}
        {
        }

        Chatlog& operator=(const Chatlog& other)
        {
            m_Messages = other.m_Messages;

            return *this;
        }

        Chatlog& operator=(Chatlog&& other)
        {
            static_cast<sfn::SyncedObject*>(this)->operator=( std::forward<sfn::SyncedObject>( other ));
            m_Messages = other.m_Messages;

            return *this;
        }

        virtual object_type_id_type GetTypeID() const override
        {
            return type_id;
        }

        void pushMessage(const std::string& message)
        {
            m_Messages.Get().push_front(message);

            if(m_Messages.Get().size() > MAX_CHAT_HISTORY_SIZE)
                m_Messages.Get().pop_back();
        }

        std::list<std::string>& getMessages(){ return m_Messages; }

    protected:
        sfn::SyncedType<std::list<std::string>> m_Messages;
    };

    class PlayerState : public sfn::SyncedObject
    {
    public:
        // SFNUL ID
        const static object_type_id_type type_id;

        PlayerState() :
                m_Input{this},
                m_Name{this},
                m_PlayerID{this, 0},
                m_Position{this, Math::float3(0,0,0) },
                m_Direction{this, Math::float3(0,0,0) },
                m_ServerHandle{this}
        {
        }
         
        PlayerState(const PlayerState& other) :
            m_Input{this, other.m_Input},
            m_Name{this, other.m_Name},
            m_PlayerID{this, other.m_PlayerID},
            m_PlayerEntity{other.m_PlayerEntity},
            m_Position{this, other.m_Position},
            m_Direction{this, other.m_Position},
            m_ServerHandle{this, other.m_ServerHandle},
            sfn::SyncedObject()
        {
        }

        PlayerState(PlayerState&& other) :
            sfn::SyncedObject{ std::forward<sfn::SyncedObject>(other) },
            m_Input{this, other.m_Input},
            m_Name{this, other.m_Name},
            m_PlayerID{this, other.m_PlayerID},
            m_PlayerEntity{other.m_PlayerEntity},
            m_Position{this, other.m_Position},
            m_Direction{this, other.m_Position},
            m_ServerHandle{this, other.m_ServerHandle}
        {
        }

        PlayerState& operator=(const PlayerState& other)
        {
            m_Input = other.m_Input;
            m_Name = other.m_Name;
            m_PlayerID = other.m_PlayerID;
            m_PlayerEntity = other.m_PlayerEntity;
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;
            m_ServerHandle = other.m_ServerHandle;

            return *this;
        }

        PlayerState& operator=(PlayerState&& other)
        {
            static_cast<sfn::SyncedObject*>(this)->operator=( std::forward<sfn::SyncedObject>( other ));
            m_Input = other.m_Input;
            m_Name = other.m_Name;
            m_PlayerID = other.m_PlayerID;
            m_PlayerEntity = other.m_PlayerEntity;
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;
            m_ServerHandle = other.m_ServerHandle;

            return *this;
        }


        virtual object_type_id_type GetTypeID() const override
        {
            return type_id;
        }

        const Logic::PlayerInput& getPlayerInput() const
        {
            return m_Input.GetValue();
        }

        void setPlayerInput(const Logic::PlayerInput& input)
        {
            m_Input = input;
        }

        void setPlayerEntity(Handle::EntityHandle e)
        {
            m_PlayerEntity = e;
        }

        Handle::EntityHandle getPlayerEntity()
        {
            return m_PlayerEntity;
        }

        void setPlayerID(uint16_t id)
        {
            m_PlayerID = (int16_t)id;
        }

        uint16_t getPlayerID()
        {
            return (uint16_t)m_PlayerID;
        }

        Math::float3& getPosition(){ return m_Position; };
        Math::float3& getDirection(){ return m_Direction; };
        std::string& getName(){ return m_Name; }
        ZMemory::BigHandle& getServerHandle(){ return m_ServerHandle; }

        /**
         * Called on the worlds frame-update loop
         * @param deltatime Time since last frame
         * @param world World the player-entity is in
         * @param localPlayer Whether this is the clients own player
         */
        void onFrameUpdate(double deltatime, World::WorldInstance& world, bool localPlayer = false);

    private:
        sfn::SyncedType<Logic::PlayerInput> m_Input;

        /**
         * Current position/direction
         */
        sfn::SyncedType<Math::float3, sfn::SynchronizationType::Stream> m_Position;
        sfn::SyncedType<Math::float3, sfn::SynchronizationType::Stream> m_Direction;

        sfn::SyncedType<ZMemory::BigHandle> m_ServerHandle;

        /**
         * Name of this player
         */
        sfn::SyncedType<std::string> m_Name;

        /**
         * Unique ID for every player
         */
        sfn::SyncedInt16 m_PlayerID;

        /**
         * Non-Synched entity handle, only valid on server
         */
        Handle::EntityHandle m_PlayerEntity;
 
    };


}
