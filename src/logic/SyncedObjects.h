#pragma once
#include <logic/PlayerInput.h>
#include <SFNUL.hpp>
#include <handle/HandleDef.h>

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
      
}

namespace World
{
    class WorldInstance;
}

namespace Net
{
    // Our IDs for the different streams.
    enum StreamID : sfn::LinkBase::stream_id_type {
        Default = 0, // You shouldn't touch this unless you don't use the default stream anyway.
        PlayerInputStream = 1,
        LoginStream = 2,
        SyncStream = 200 // DO NOT TOUCH STREAM IDS >= 200 They are used for internal purposes.
    };

    enum LoginPacket : uint16_t {
        LP_Error = 0,
        LP_AskPlayerID = 1, // Client asks for a player ID. Response is uint16_t player id


    };

    class PlayerState : public sfn::SyncedObject
    {
    public:
        // SFNUL ID
        const static object_type_id_type type_id;

        PlayerState() :
            m_Input{this},
            m_PlayerID{this, 0},
            m_Position{this, Math::float3(0,0,0) },
            m_Direction{this, Math::float3(0,0,0) }
        {
        }
         
        PlayerState(const PlayerState& other) :
            m_Input{this, other.m_Input},
            m_PlayerID{this, other.m_PlayerID},
            m_PlayerEntity{other.m_PlayerEntity},
            m_Position{this, other.m_Position},
            m_Direction{this, other.m_Position},
            sfn::SyncedObject()
        {
        }

        PlayerState(PlayerState&& other) :
            sfn::SyncedObject{ std::forward<sfn::SyncedObject>(other) },
            m_Input{this, other.m_Input},
            m_PlayerID{this, other.m_PlayerID},
            m_PlayerEntity{other.m_PlayerEntity},
            m_Position{this, other.m_Position},
            m_Direction{this, other.m_Position}
        {
        }

        PlayerState& operator=(const PlayerState& other)
        {
            m_Input = other.m_Input;
            m_PlayerID = other.m_PlayerID;
            m_PlayerEntity = other.m_PlayerEntity;
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;

            return *this;
        }

        PlayerState& operator=(PlayerState&& other)
        {
            static_cast<sfn::SyncedObject*>(this)->operator=( std::forward<sfn::SyncedObject>( other ));
            m_Input = other.m_Input;
            m_PlayerID = other.m_PlayerID;
            m_PlayerEntity = other.m_PlayerEntity;
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;

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
