///
/// \weakgroup parser_game GAME parser
/// @{
///

FUNC void AI_AimAt(VAR C_NPC Attacker, VAR C_NPC Target)
{
};

FUNC void AI_AlignToFP(VAR C_NPC Character)
{
};

FUNC void AI_AlignToWP(VAR C_NPC Character)
{
};

FUNC void AI_Ask(VAR C_NPC Character, VAR func OnYes, VAR func OnNo)
{
};

FUNC void AI_AskText(VAR C_NPC Character, VAR func OnYes, VAR func OnNo, VAR string TextYes, VAR string TextNo)
{
};

FUNC void AI_Attack(VAR C_NPC Character)
{
};

FUNC void AI_CanSeeNpc(VAR C_NPC Character, VAR C_NPC Target, VAR func AIState)
{
};

FUNC void AI_CombatReactToDamage(VAR C_NPC Character)
/// \bug        The implementation leaves the \p Character on the stack.
{
};

FUNC void AI_ContinueRoutine(VAR C_NPC Character)
{
};

FUNC void AI_Defend(VAR C_NPC Character)
{
};

FUNC void AI_Dodge(VAR C_NPC Character)
{
};

FUNC void AI_DrawWeapon(VAR C_NPC Character)
{
};

FUNC void AI_DropItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void AI_DropMob(VAR C_NPC Character)
{
};

FUNC void AI_EquipArmor(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void AI_EquipBestArmor(VAR C_NPC Character)
{
};

FUNC int AI_EquipBestMeleeWeapon(VAR C_NPC Character)
/// \bug        The implementation does not push a result on the stack.
{
};

FUNC int AI_EquipBestRangedWeapon(VAR C_NPC Character)
/// \bug        The implementation does not push a result on the stack.
{
};

FUNC void AI_FinishingMove(VAR C_NPC Attacker, VAR C_NPC Target)
{
};

FUNC void AI_Flee(VAR C_NPC Character)
{
};

FUNC void AI_GotoFP(VAR C_NPC Character, VAR string FreePointName)
{
};

FUNC void AI_GotoItem(VAR C_NPC Character, VAR C_Item Target)
{
};

FUNC void AI_GotoNextFP(VAR C_NPC Character, VAR string FreePointName)
{
};

FUNC void AI_GotoNpc(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_GotoSound(VAR C_NPC Character)
{
};

FUNC void AI_GotoWP(VAR C_NPC Character, VAR string WaypointName)
{
};

FUNC void AI_LookAt(VAR C_NPC Character, VAR string TargetName)
{
};

FUNC void AI_LookAtNpc(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_Output(VAR C_NPC Character, VAR C_NPC Target, VAR string OuName)
{
};

FUNC void AI_OutputSVM(VAR C_NPC Character, VAR C_NPC Target, VAR string SvmName)
{
};

FUNC void AI_OutputSVM_Overlay(VAR C_NPC Character, VAR C_NPC Target, VAR string SvmName)
{
};

FUNC void AI_PlayAni(VAR C_NPC Character, VAR string AniName)
{
};

FUNC void AI_PlayAniBS(VAR C_NPC Character, VAR string AniName, VAR int NextBodyState)
{
};

FUNC void AI_PlayCutscene(VAR C_NPC Character, VAR string CsName)
{
};

FUNC void AI_PointAt(VAR C_NPC Character, VAR string TargetName)
{
};

FUNC void AI_PointAtNpc(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_ProcessInfos(VAR C_NPC NonPlayer)
{
};

FUNC void AI_QuickLook(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_ReadyMeleeWeapon(VAR C_NPC Character)
/// \bug        The implementation leaves an additional int (0) on the stack.
{
};

FUNC void AI_ReadyRangedWeapon(VAR C_NPC Character)
/// \bug        The implementation leaves an additional int (0) on the stack.
{
};

FUNC void AI_ReadySpell(VAR C_NPC Character, VAR int SpellId, VAR int ManaInvested)
{
};

FUNC void AI_RemoveWeapon(VAR C_NPC Character)
{
};

FUNC void AI_SetNpcsToState(VAR C_NPC Origin, VAR func AIState, VAR int Distance)
{
};

FUNC void AI_SetWalkMode(VAR C_NPC Character, VAR int WalkMode)
{
};

FUNC void AI_ShootAt(VAR C_NPC Attacker, VAR C_NPC Target)
{
};

FUNC void AI_StandUp(VAR C_NPC Character)
{
};

FUNC void AI_StandUpQuick(VAR C_NPC Character)
{
};

FUNC void AI_StartState(VAR C_NPC Character, VAR func AIState, VAR BOOL EndPrevState, VAR string WaypointName)
{
};

FUNC void AI_StopAim(VAR C_NPC Attacker)
{
};

FUNC void AI_StopLookAt(VAR C_NPC Character)
{
};

FUNC void AI_StopPointAt(VAR C_NPC Character)
{
};

FUNC void AI_StopProcessInfos(VAR C_NPC Character)
{
};

FUNC void AI_TakeItem(VAR C_NPC Character, VAR C_Item Target)
{
};

FUNC void AI_TakeMob(VAR C_NPC Character, VAR string SchemeName)
{
};

FUNC void AI_Teleport(VAR C_NPC Character, VAR string TargetName)
{
};

FUNC void AI_TurnAway(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_TurnToNPC(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_TurnToSound(VAR C_NPC Character)
{
};

FUNC void AI_UnequipArmor(VAR C_NPC Character)
{
};

FUNC void AI_UnequipWeapons(VAR C_NPC Character)
{
};

FUNC void AI_UnreadySpell(VAR C_NPC Character)
{
};

FUNC void AI_UseItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void AI_UseItemToState(VAR C_NPC Character, VAR C_ITEM_ID ItemId, VAR int TargetState)
{
};

FUNC BOOL AI_UseMob(VAR C_NPC Character, VAR string SchemeName, VAR int TargetState)
{
	return FALSE;
};

FUNC void AI_Wait(VAR C_NPC Character, VAR float Seconds)
{
};

FUNC void AI_WaitForQuestion(VAR C_NPC NonPlayer, VAR func OnYesNo)
{
};

FUNC void AI_Waitms(VAR C_NPC Character, VAR int Milliseconds)
{
};

FUNC void AI_WaitTillEnd(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_WhirlAround(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void AI_WhirlAroundToSource(VAR C_NPC Character)
{
};

FUNC string ConcatStrings(VAR string Str, VAR string Append)
{
	return "";
};

FUNC void CreateInvItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void CreateInvItems(VAR C_NPC Character, VAR C_ITEM_ID ItemId, VAR int Amount)
{
};

FUNC int Doc_Create() 
{
	return -1;
};

FUNC int Doc_CreateMap()
{
	return -1;
};

FUNC void Doc_Font(VAR string Font)
{
};

FUNC void Doc_MapCoordinates(VAR string LevelName, VAR float LevelLeft, VAR float LevelTop, VAR float TextureLeft, VAR float TextureTop, VAR float LevelRight, VAR float LevelBottom, VAR float TextureRight, VAR float TextureBottom)
{
};

FUNC void Doc_Open(VAR string Texture)
{
};

FUNC void Doc_Print(VAR string Text)
{
};

FUNC void Doc_PrintLine(VAR int Doc, VAR int Page, VAR string Text)
{
};

FUNC void Doc_PrintLines(VAR int Doc, VAR int Page, VAR string Text)
{
};

FUNC void Doc_SetFont(VAR int Doc, VAR int Page, VAR string Font)
{
};

FUNC void Doc_SetLevel(VAR int Doc, VAR string LevelName)
{
};

FUNC void Doc_SetMargins(VAR int Doc, VAR int Page, VAR int Left, VAR int Top, VAR int Right, VAR int Bottom, VAR BOOL SetMargin)
{
};

FUNC void Doc_SetPage(VAR int Doc, VAR int Page, VAR string Texture, VAR BOOL Scale)
{
};

FUNC void Doc_SetPages(VAR int Doc, VAR int PageCount)
{
};

FUNC void Doc_Show(VAR int Doc)
{
};

FUNC void EquipItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void ExitGame()
{
};

FUNC int FloatToInt(VAR float Value)
{
	return 0;
};

FUNC string FloatToString(VAR float Value)
{
	return "0.0";
};

FUNC int Hlp_CutscenePlayed(VAR string CSName)
{
	return 0;
};

FUNC C_VOB_ID Hlp_GetInstanceID(VAR C_VOB Object)
{
	return -1;
};

FUNC C_NPC Hlp_GetNpc(VAR C_NPC_ID NpcId)
{
	return NULL;
};

FUNC BOOL Hlp_IsItem(VAR C_Item ItemObject, VAR C_ITEM_ID ItemId)
{
	return FALSE;
};

FUNC BOOL Hlp_IsValidItem(VAR C_Item ItemObject)
{
	return FALSE;
};

FUNC BOOL Hlp_IsValidNpc(VAR C_NPC Character)
{
	return FALSE;
};

FUNC int Hlp_Random(VAR int Dividend)
{
	return 0;
};

FUNC BOOL Hlp_StrCmp(VAR string Str1, VAR string Str2)
{
	return FALSE;
};

FUNC void Info_AddChoice(VAR C_INFO_ID InfoId, VAR string ChoiceText, VAR func ChoiceFunc)
{
};

FUNC void Info_ClearChoices(VAR C_INFO_ID InfoId)
{
};

FUNC BOOL InfoManager_HasFinished()
{
	return TRUE;
};

FUNC void IntroduceChapter(VAR string Title, VAR string Text, VAR string Texture, VAR string SfxName, VAR int Milliseconds)
/// \bug        The implementation leaves an additional int (0) on the stack.
{
};

FUNC float IntToFloat(VAR int Value)
{
	return 0.0;
};

FUNC string IntToString(VAR int Value)
{
	return "0";
};

FUNC void Log_AddEntry(VAR string Topic, VAR string Entry)
{
};

FUNC void Log_CreateTopic(VAR string Topic, VAR int LogSection)
{
};

FUNC void Log_SetTopicStatus(VAR string Topic, VAR int TopicStatus)
{
};

FUNC void Mdl_ApplyOverlayMds(VAR C_NPC Character, VAR string AniOverlayName)
{
};

FUNC void Mdl_ApplyOverlayMdsTimed(VAR C_NPC Character, VAR string AniOverlayName, VAR int Milliseconds)
{
};

FUNC void Mdl_ApplyRandomAni(VAR C_NPC Character, VAR string StateAniName, VAR string RandomAniName)
{
};

FUNC void Mdl_ApplyRandomAniFreq(VAR C_NPC Character, VAR string RandomAniName, VAR float Seconds)
{
};

FUNC void Mdl_ApplyRandomFaceAni(VAR C_NPC Character, VAR string FaceAniName, VAR float MinTime, VAR float MinTimeVar, VAR float MaxTime, VAR float MaxTimeVar, VAR float MinProb)
{
};

FUNC void Mdl_RemoveOverlayMds(VAR C_NPC Character, VAR string AniOverlayName)
{
};

FUNC void Mdl_SetModelFatness(VAR C_NPC Character, VAR float ModelFatness)
{
};

FUNC void Mdl_SetModelScale(VAR C_NPC Character, VAR float ModelScaleX, VAR float ModelScaleY, VAR float ModelScaleZ)
{
};

FUNC void Mdl_SetVisual(VAR C_NPC Character, VAR string VisualName)
{
};

FUNC void Mdl_SetVisualBody(VAR C_NPC Character, VAR string Body, VAR int BodyNr, VAR int BodyClr, VAR string Head, VAR int HeadNr, VAR int TeethNr, VAR C_ITEM_ID ArmorId)
{
};

FUNC void Mdl_StartFaceAni(VAR C_NPC Character, VAR string FaceAniName, VAR float Intensity, VAR float Milliseconds)
{
};

FUNC void Mis_AddMissionEntry(VAR C_Mission Mission, VAR string Text)
/// \bug        The implementation removes an int instead of a C_Mission
///             from the stack.
{
};

FUNC int Mis_GetStatus(VAR C_MISSION_ID MissionId)
{
	return -1;
};

FUNC BOOL Mis_OnTime(VAR C_MISSION_ID MissionId)
/// \bug        The external is linked to \ref Mis_SetStatus.
{
	return FALSE;
};

FUNC void Mis_RemoveMission(VAR C_Mission Mission)
/// \bug        The implementation removes an int instead of a C_Mission
///             from the stack.
{
};

FUNC void Mis_SetStatus(VAR C_MISSION_ID MissionId, VAR int NewStatus)
{
};

FUNC void Mob_CreateItems(VAR string MobContainerName, VAR C_ITEM_ID ItemId, VAR int Amount)
{
};

FUNC int Mob_HasItems(VAR string MobContainerName, VAR C_ITEM_ID ItemId)
{
	return 0;
};

FUNC BOOL Npc_AreWeStronger(VAR C_NPC Character, VAR C_NPC Target)
{
	return FALSE;
};

FUNC BOOL Npc_CanSeeItem(VAR C_NPC Character, VAR C_Item Target)
{
	return FALSE;
};

FUNC BOOL Npc_CanSeeNpc(VAR C_NPC Character, VAR C_NPC Target)
{
	return FALSE;
};

FUNC BOOL Npc_CanSeeNpcFreeLOS(VAR C_NPC Character, VAR C_NPC Target)
{
	return FALSE;
};

FUNC BOOL Npc_CanSeeSource(VAR C_NPC Character)
{
	return FALSE;
};

FUNC void Npc_ChangeAttribute(VAR C_NPC Character, VAR int Attribute, VAR int Value)
{
};

FUNC BOOL Npc_CheckAvailableMission(VAR C_NPC NonPlayer, VAR int Status, VAR BOOL Important)
{
	return FALSE;
};

FUNC BOOL Npc_CheckInfo(VAR C_NPC NonPlayer, VAR BOOL Important)
{
	return FALSE;
};

FUNC BOOL Npc_CheckOfferMission(VAR C_NPC NonPlayer, VAR BOOL Important)
{
	return FALSE;
};

FUNC BOOL Npc_CheckRunningMission(VAR C_NPC NonPlayer, VAR BOOL Important)
{
	return FALSE;
};

FUNC void Npc_ClearAIQueue(VAR C_NPC Character)
{
};

FUNC void Npc_CreateSpell(VAR C_NPC Character, VAR int SpellId)
{
};

FUNC BOOL Npc_DeleteNews(VAR C_NPC Character, VAR int NewsId)
{
	return FALSE;
};

FUNC void Npc_ExchangeRoutine(VAR C_NPC Character, VAR string RoutineName)
{
};

FUNC int Npc_GetActiveSpell(VAR C_NPC Character)
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC int Npc_GetActiveSpellCat(VAR C_NPC Character)
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC int Npc_GetActiveSpellLevel(VAR C_NPC Character)
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC int Npc_GetAttitude(VAR C_NPC Character, VAR C_NPC Target)
/// \bug        The implementation does not leave the result on the stack
///             if \p Character or \p Target are invalid character references.
{
	return ATT_NEUTRAL;
};

FUNC int Npc_GetBodyState(VAR C_NPC Character)
{
	return 0;
};

FUNC int Npc_GetComrades(VAR C_NPC Character)
{
	return 0;
};

FUNC string Npc_GetDetectedMob(VAR C_NPC Character)
{
	return "NONE";
};

FUNC int Npc_GetDistToItem(VAR C_NPC Origin, VAR C_Item Target)
{
	return 2147483647;
};

FUNC int Npc_GetDistToNpc(VAR C_NPC Origin, VAR C_NPC Target)
{
	return 2147483647;
};

FUNC int Npc_GetDistToPlayer(VAR C_NPC Target)
{
	return 2147483647;
};

FUNC int Npc_GetDistToWP(VAR C_NPC Origin, VAR string WaypointName)
{
	return 2147483647;
};

FUNC C_Item Npc_GetEquippedArmor(VAR C_NPC Character)
{
	return NULL;
};

FUNC C_Item Npc_GetEquippedMeleeWeapon(VAR C_NPC Character)
{
	return NULL;
};

FUNC C_Item Npc_GetEquippedRangedWeapon(VAR C_NPC Character)
{
	return NULL;
};

FUNC int Npc_GetGuildAttitude(VAR C_NPC Character, VAR C_NPC Target)
{
	return ATT_NEUTRAL;
};

FUNC BOOL Npc_GetInvItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
	return FALSE;
};

FUNC int Npc_GetInvItemBySlot(VAR C_NPC Character, VAR int Category, VAR int Slot)
{
	return 0;
};

FUNC string Npc_GetNearestWP(VAR C_NPC Character)
{
	return "";
};

FUNC C_NPC Npc_GetNewsOffender(VAR C_NPC Character, VAR int NewsId)
{
	return NULL;
};

FUNC C_NPC Npc_GetNewsVictim(VAR C_NPC Character, VAR int NewsId)
{
	return NULL;
};

FUNC C_NPC Npc_GetNewsWitness(VAR C_NPC Character, VAR int NewsId)
{
	return NULL;
};

FUNC BOOL Npc_GetNextTarget(VAR C_NPC Character)
{
	return FALSE;
};

FUNC string Npc_GetNextWP(VAR C_NPC Character)
{
	return "";
};

FUNC int Npc_GetPermAttitude(VAR C_NPC Character, VAR C_NPC Target)
/// \bug        The implementation does not leave the result on the stack
///             if \p Character or \p Target are invalid character references.
{
	return ATT_NEUTRAL;
};

FUNC C_Item Npc_GetReadiedWeapon(VAR C_NPC Character)
{
	return NULL;
};

FUNC int Npc_GetStateTime(VAR C_NPC Character)
{
	return 0;
};

FUNC int Npc_GetTalentSkill(VAR C_NPC Character, VAR int Talent)
{
	return 0;
};

FUNC int Npc_GetTalentValue(VAR C_NPC Character, VAR int Talent)
{
	return 0;
};

FUNC BOOL Npc_GetTarget(VAR C_NPC Character)
{
	return FALSE;
};

FUNC int Npc_GetTrueGuild(VAR C_NPC Character)
{
	return 0;
};

FUNC BOOL Npc_GiveInfo(VAR C_NPC NonPlayer, VAR BOOL Important)
{
	return FALSE;
};

FUNC void Npc_GiveItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId, VAR C_NPC Target)
{
};

FUNC BOOL Npc_HasBodyFlag(VAR C_NPC Character, VAR int BodyStates)
{
	return FALSE;
};

FUNC BOOL Npc_HasDetectedNpc(VAR C_NPC NonPlayer, VAR C_NPC Target)
/// \bug        The implementation does not leave the result on the stack
///             if \p NonPlayer or \p Target are invalid character references.
{
	return FALSE;
};

FUNC BOOL Npc_HasEquippedArmor(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasEquippedMeleeWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasEquippedRangedWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasEquippedWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC int Npc_HasItems(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
	return 0;
};

FUNC int Npc_HasNews(VAR C_NPC Character, VAR int NewsId, VAR C_NPC NewsOffender, VAR C_NPC NewsVictim)
{
	return 0;
};

FUNC BOOL Npc_HasOffered(VAR C_NPC Character, VAR C_NPC Player, VAR C_ITEM_ID ItemId)
{
	return FALSE;
};

FUNC BOOL Npc_HasRangedWeaponWithAmmo(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasReadiedMeleeWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasReadiedRangedWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasReadiedWeapon(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_HasSpell(VAR C_NPC Character, VAR int SpellId)
{
	return FALSE;
};

FUNC BOOL Npc_IsAiming(VAR C_NPC Attacker, VAR C_NPC Target)
{
	return FALSE;
};

FUNC BOOL Npc_IsDead(VAR C_NPC Character)
{
	return TRUE;
};

FUNC BOOL Npc_IsDetectedMobOwnedByGuild(VAR C_NPC Character, VAR int Guild)
{
	return FALSE;
};

FUNC BOOL Npc_IsDetectedMobOwnedByNpc(VAR C_NPC Character, VAR C_NPC Owner)
{
	return FALSE;
};

FUNC BOOL Npc_IsInCutscene(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_IsInFightMode(VAR C_NPC Character, VAR int WeaponMode)
{
	return FALSE;
};

FUNC BOOL Npc_IsInRoutine(VAR C_NPC Character, VAR func Routine)
{
	return FALSE;
};

FUNC BOOL Npc_IsInState(VAR C_NPC Character, VAR func AIState)
{
	return FALSE;
};

FUNC BOOL Npc_IsNear(VAR C_NPC Origin, VAR C_NPC Target)
{
	return FALSE;
};

FUNC BOOL Npc_IsNewsGossip(VAR C_NPC Character, VAR int NewsId)
{
	return FALSE;
};

FUNC BOOL Npc_IsNextTargetAvailable(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_IsOnFP(VAR C_NPC Character, VAR string FreePointName)
{
	return FALSE;
};

FUNC BOOL Npc_IsPlayer(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_IsPlayerInMyRoom(VAR C_NPC Owner)
{
	return FALSE;
};

FUNC BOOL Npc_IsVoiceActive(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_IsWayBlocked(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_KnowsInfo(VAR C_NPC Character, VAR C_INFO_ID InfoId)
{
	return FALSE;
};

FUNC BOOL Npc_KnowsPlayer(VAR C_NPC Character, VAR C_NPC Player)
{
	return FALSE;
};

FUNC void Npc_LearnSpell(VAR C_NPC Character, VAR int SpellId)
{
};

FUNC void Npc_MemoryEntry(VAR C_NPC Witness, VAR BOOL Gossip, VAR C_NPC Offender, VAR int NewsId, VAR C_NPC NewsVictim)
{
};

FUNC void Npc_MemoryEntryGuild(VAR C_NPC Witness, VAR BOOL Gossip, VAR C_NPC Offender, VAR int NewsId, VAR C_NPC NewsVictim)
{
};

FUNC BOOL Npc_OwnedByGuild(VAR C_Item ItemObject, VAR int Guild)
{
	return FALSE;
};

FUNC BOOL Npc_OwnedByNpc(VAR C_Item ItemObject, VAR C_NPC Owner)
{
	return FALSE;
};

FUNC void Npc_PercDisable(VAR C_NPC Character, VAR int PerceptionId)
{
};

FUNC void Npc_PerceiveAll(VAR C_NPC NonPlayer)
{
};

FUNC void Npc_PercEnable(VAR C_NPC Character, VAR int PerceptionId, VAR func OnPerception)
{
};

FUNC void Npc_PlayAni(VAR C_NPC Character, VAR string AniName)
{
};

FUNC BOOL Npc_RefuseTalk(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Npc_RemoveInvItem(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
	return FALSE;
};

FUNC BOOL Npc_RemoveInvItems(VAR C_NPC Character, VAR C_ITEM_ID ItemId, VAR int Amount)
{
	return FALSE;
};

FUNC void Npc_SendPassivePerc(VAR C_NPC Character, VAR int PerceptionId, VAR C_NPC PercVictim, VAR C_NPC Offender)
{
};

FUNC void Npc_SendSinglePerc(VAR C_NPC Origin, VAR C_NPC Target, VAR int PerceptionId)
{
};

FUNC int Npc_SetActiveSpellInfo(VAR C_NPC Character, VAR int TransformInstance)
/// \bug        The implementation does not leave the result on the stack.
{
	return 0;
};

FUNC void Npc_SetAttitude(VAR C_NPC Character, VAR int Attitude)
{
};

FUNC void Npc_SetKnowsPlayer(VAR C_NPC Character, VAR C_NPC Player)
{
};

FUNC void Npc_SetPercTime(VAR C_NPC Character, VAR float Seconds)
{
};

FUNC void Npc_SetRefuseTalk(VAR C_NPC Character, VAR int Seconds)
{
};

FUNC void Npc_SetStateTime(VAR C_NPC Character, VAR int Seconds)
{
};

FUNC void Npc_SetTalentSkill(VAR C_NPC Character, VAR int Talent, VAR int Skill)
/// \bug        The implementation leaves an additional int (Hlp_IsValidNpc)
///             on the stack.
{
};

FUNC void Npc_SetTalentValue(VAR C_NPC Character, VAR int Talent, VAR int Value)
/// \bug        The implementation leaves an additional int (Hlp_IsValidNpc)
///             on the stack.
{
};

FUNC void Npc_SetTarget(VAR C_NPC Character, VAR C_NPC Target)
{
};

FUNC void Npc_SetTempAttitude(VAR C_NPC Character, VAR int Attitude)
{
};

FUNC void Npc_SetToFightMode(VAR C_NPC Character, VAR C_ITEM_ID ItemId)
{
};

FUNC void Npc_SetToFistMode(VAR C_NPC Character)
{
};

FUNC int Npc_SetTrueGuild(VAR C_NPC Character, VAR int Guild)
/// \bug        The implementation does not leave a result on the stack.
{
	return 0;
};

FUNC BOOL Npc_StartItemReactModules(VAR C_NPC Taker, VAR C_NPC Giver, VAR C_Item ItemObject)
{
	return FALSE;
};

FUNC BOOL Npc_WasInState(VAR C_NPC Character, VAR func AIState)
{
	return FALSE;
};

FUNC BOOL Npc_WasPlayerInMyRoom(VAR C_NPC Owner)
{
	return FALSE;
};

FUNC void Perc_SetRange(VAR int PerceptionId, VAR int Range)
{
};

FUNC void Print(VAR string Text)
{
};

FUNC void PrintDebug(VAR string Text)
{
};

FUNC void PrintDebugCh(VAR int Channel, VAR string Text)
{
};

FUNC void PrintDebugInst(VAR string Text)
{
};

FUNC void PrintDebugInstCh(VAR int Channel, VAR string Text)
{
};

FUNC BOOL PrintDialog(VAR int GameViewId, VAR string Text, VAR int PosX, VAR int PosY, VAR string Font, VAR int Seconds)
{
	return FALSE;
};

FUNC void PrintMulti(VAR string TextVariant1, VAR string TextVariant2, VAR string TextVariant3, VAR string TextVariant4, VAR string TextVariant5)
{
};

FUNC BOOL PrintScreen(VAR string Text, VAR int PosX, VAR int PosY, VAR string Font, VAR int Seconds)
/// \bug        The implementation removes an additional int from
///             the stack after retrieving the defined parameters.
{
	return FALSE;
};

FUNC void Rtn_Exchange(VAR string OldRoutineName, VAR string NewRoutineName)
{
};

FUNC void SetPercentDone(VAR int WorldProgress)
{
};

FUNC int Snd_GetDistToSource(VAR C_NPC Character)
{
	return 2147483647;
};

FUNC BOOL Snd_IsSourceItem(VAR C_NPC Character)
{
	return FALSE;
};

FUNC BOOL Snd_IsSourceNpc(VAR C_NPC Character)
{
	return FALSE;
};

FUNC void Snd_Play(VAR string Name)
{
};

FUNC void Snd_Play3d(VAR C_VOB Object, VAR string Name)
{
};

FUNC void TA(VAR C_NPC Character, VAR int StartHour, VAR int StopHour, VAR func Routine, VAR string WaypointName)
{
};

FUNC void TA_BeginOverlay(VAR C_NPC Character)
{
};

FUNC void TA_CS(VAR C_NPC Character, VAR string CsName, VAR string RoleName)
{
};

FUNC void TA_EndOverlay(VAR C_NPC Character)
{
};

FUNC void TA_Min(VAR C_NPC Character, VAR int BeginHour, VAR int BeginMinute, VAR int EndHour, VAR int EndMinute, VAR func Routine, VAR string WaypointName)
{
};

FUNC void TA_RemoveOverlay(VAR C_NPC Character)
{
};

FUNC void Tal_Configure(VAR int Talent, VAR int Relevance)
/// \bug        The implementation removes an additional C_NPC from the stack
///             and leaves an additional integer (Hlp_IsValidNpc) on the stack.
{
};

FUNC void Wld_AssignRoomToGuild(VAR string RoomName, VAR int Guild)
{
};

FUNC void Wld_AssignRoomToNpc(VAR string RoomName, VAR C_NPC Owner)
{
};

FUNC BOOL Wld_DetectItem(VAR C_NPC NonPlayer, VAR int ItemFlag)
{
	return FALSE;
};

FUNC BOOL Wld_DetectNpc(VAR C_NPC NonPlayer, VAR C_NPC_ID NpcId, VAR func AIState, VAR int Guild)
{
	return FALSE;
};

FUNC BOOL Wld_DetectNpcEx(VAR C_NPC NonPlayer, VAR C_NPC_ID NpcId, VAR func AIState, VAR int Guild, VAR BOOL DetectPlayer)
{
	return FALSE;
};

FUNC BOOL Wld_DetectPlayer(VAR C_NPC NonPlayer)
{
	return FALSE;
};

FUNC void Wld_ExchangeGuildAttitudes(VAR string TableName)
{
};

FUNC int Wld_GetDay()
{
	return 0;
};

FUNC int Wld_GetFormerPlayerPortalGuild()
{
	return -1;
};

FUNC C_NPC Wld_GetFormerPlayerPortalOwner()
{
	return NULL;
};

FUNC int Wld_GetGuildAttitude(VAR int Guild, VAR int TargetGuild)
{
	return ATT_NEUTRAL;
};

FUNC int Wld_GetMobState(VAR C_NPC Character, VAR string SchemeName)
{
	return -1;
};

FUNC int Wld_GetPlayerPortalGuild()
{
	return -1;
};

FUNC C_NPC Wld_GetPlayerPortalOwner()
{
	return NULL;
};

FUNC void Wld_InsertItem(VAR C_ITEM_ID ItemId, VAR string SpawnPoint)
{
};

FUNC void Wld_InsertNpc(VAR C_NPC_ID NpcId, VAR string SpawnPoint)
{
};

FUNC void Wld_InsertNpcAndRespawn(VAR C_NPC_ID NpcId, VAR string SpawnPoint, VAR int RespawnHours)
{
};

FUNC void Wld_InsertObject(VAR string MobVisualName, VAR string SpawnPoint)
{
};

FUNC BOOL Wld_IsFPAvailable(VAR C_NPC Character, VAR string FreePointName)
{
	return FALSE;
};

FUNC BOOL Wld_IsMobAvailable(VAR C_NPC Character, VAR string SchemeName)
{
	return FALSE;
};

FUNC BOOL Wld_IsNextFPAvailable(VAR C_NPC Character, VAR string FreePointName)
{
	return FALSE;
};

FUNC BOOL Wld_IsTime(VAR int BeginHour, VAR int BeginMinute, VAR int EndHour, VAR int EndMinute)
{
	return FALSE;
};

FUNC void Wld_PlayEffect(VAR string VfxName, VAR C_VOB Origin, VAR C_VOB Target, VAR int EffectLevel, VAR int Damage, VAR int DamageType, VAR BOOL IsProjectile)
{
};

FUNC BOOL Wld_RemoveItem(VAR C_Item ItemObject)
{
	return FALSE;
};

FUNC void Wld_RemoveNpc(VAR C_NPC_ID NpcId)
{
};

FUNC void Wld_SendTrigger(VAR string ObjectName)
{
};

FUNC void Wld_SendUntrigger(VAR string ObjectName)
{
};

FUNC void Wld_SetGuildAttitude(VAR int Guild, VAR int Attitude, VAR int TargetGuild)
{
};

FUNC void Wld_SetMobRoutine(VAR int Hour, VAR int Minute, VAR string SchemeName, VAR int TargetState)
{
};

FUNC void Wld_SetObjectRoutine(VAR int Hour, VAR int Minute, VAR string ObjectName, VAR BOOL Trigger)
{
};

FUNC void Wld_SetTime(VAR int Hour, VAR int Minute)
{
};

FUNC void Wld_SpawnNpcRange(VAR C_NPC Evocator, VAR C_NPC_ID NpcId, VAR int Count, VAR float Range)
{
};

///
/// @}
///
