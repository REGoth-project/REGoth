#include <engine/BaseEngine.h>
#include <engine/GameEngine.h>

namespace Net
{
    class ClientState;
    class ServerState;
}

namespace Engine
{
    class NetEngine : public GameEngine
    {
    public:
        NetEngine();
        virtual ~NetEngine();

		/**
		 * @brief Initializes the Engine
		 */
		virtual void initEngine(int argc, char** argv);

    protected:
        
		/**
		 * Called when a world was added
		 */
		virtual void onWorldCreated(Handle::WorldHandle world);
		virtual void onWorldRemoved(Handle::WorldHandle world);

		/**
		 * Update-method for subclasses
		 */
		virtual void onFrameUpdate(double dt, uint16_t width, uint16_t height);

		/**
		 * @brief Called to load archives into the main VDFS-Index
		 *		  Overwrite to load your own default archives
		 */
		virtual void loadArchives();

        /** 
         * Whether this is a client or a server, one of them is non-nullptr
         */
        Net::ClientState* m_Client;
        Net::ServerState* m_Server;

    };
}
