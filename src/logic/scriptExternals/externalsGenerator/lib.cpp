using Daedalus::GameState::NpcHandle;


template <HandleType>
HandleType& getC_Object(int32_t instance)
{
    return ZMemory::handleCast<HandleType>(vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle);
}

using Daedalus::GEngineClasses::C_Npc;

bool IsNpc()
{
    const PARSymbol& parSymbol = vm->getDATFile().getSymbolByIndex(instance)
    return parSymbol.instanceDataClass == Daedalus::EInstanceClass::IC_Npc;
}

NpcHandle& getC_Npc(int32_t instance)
{
    auto& parSymbol = vm->getDATFile().getSymbolByIndex(instance);
	if (parSymbolIsNpc(parSymbol))
    {
        NpcHandle scriptHandle = ZMemory::handleCast<NpcHandle>(parSymbol.instanceDataHandle);
        return scriptHandle;
    }
    return NpcHandle();
}



C_Npc* getC_Npc(int32_t instance)
{
}


VobTypes::NpcVobInformation getNpcVobByInstance(int32_t instance)
{

}
