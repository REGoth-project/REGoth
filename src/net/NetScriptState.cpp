#include "NetScriptState.h"
#include <daedalus/DaedalusVM.h>
#include <daedalus/DATFile.h>
#include <SFNUL.hpp>

using namespace NetPlay;

enum ENetToken : uint32_t
{
    NT_SymChanged = 1,
};

NetScriptState::NetScriptState() : m_LastKnownState{this}
{

}

NetScriptState::NetScriptState(NetScriptState&& other) 
       : sfn::SyncedObject{ std::forward<sfn::SyncedObject>(other) },
         m_LastKnownState{this, other.m_LastKnownState}
{
}

NetScriptState::~NetScriptState()
{

}

void NetScriptState::syncVM(Daedalus::DaedalusVM& vm)
{
    auto& dat = vm.getDATFile();

    unsigned idx = 0;
    for(const auto& sym : dat.getSymTable().symbols)
    {
        // Only flat integers
        // TODO: Support "replicate" on other symbols, ie. text
        if(sym.properties.elemProps.flags == 0
           && sym.properties.elemProps.type == Daedalus::EParType_Int)
        {
            // Only perfom change if we need to
            if(m_LastKnownState[idx] != sym.intData)
            {
                m_ChangeHashes[idx]++;

                m_LastKnownState[idx].clear();
                
                // Write arrays in order
                for(int32_t i: sym.intData)
                {
                    m_LastKnownState[idx].push_back(i);
                }
            }
        }

        idx++;
    } 
}
