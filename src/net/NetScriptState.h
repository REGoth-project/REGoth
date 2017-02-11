#include <string>
#include <map>
#include <vector>
#include <SFNUL.hpp>

namespace Daedalus
{
    class DaedalusVM;
}

namespace sfn 
{
    // Operator overloads for our convenience.
    template<typename T, typename U>
    sfn::Message& operator<<( sfn::Message& message, const std::map<T, U>& m)
    {
        message << m.size();

        for(const auto& v : m)
            message << v.first << v.second;

        return message;
    }
    
    template<typename T, typename U>
    sfn::Message& operator>>( sfn::Message& message, std::map<T,U>& m)
    {
        size_t s; message >> s;

        for(size_t i=0;i<s;i++)
        {
            T key;
            U value;

            message >> key;
            message >> value;

            m.emplace(key, value);
        }

        return message;
    }
}

namespace NetPlay
{

    /**
     * Tracks a daedalus-VM and allows other REGoth-Script-Engines to sync their important values
     */
    class NetScriptState : public sfn::SyncedObject
    {
    public:
        const static object_type_id_type type_id;
        
        NetScriptState();
        NetScriptState(NetScriptState&& other);
        virtual ~NetScriptState();

        /**
         * Synchronizes the current state with the given VM 
         * @param vm VM to sync
         */
        void syncVM(Daedalus::DaedalusVM& vm);
    protected:
       
        /**
         * Last known script-state of integer values
         */
        sfn::SyncedType<std::map<unsigned, std::vector<int32_t>>> m_LastKnownState; 

        /**
         * Incremented on every change of a symbol
         */
        std::map<unsigned, unsigned> m_ChangeHashes;
    };
}
