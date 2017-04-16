/** @file */ 



func void AI_AimAt(var C_NPC Attacker, var C_NPC Target)
/// \brief   NPC starts aming at a NPC.
/// \param   Attacker Reference to the attacking NPC object.
/// \param   Target   Reference to the targeted NPC object.
/// \details An oCMsgAttack::EV_AIMAT message is queued for the Attacker.
/// \remarks Attacker and Target have to be valid references to oCNpc objects.
/// \remarks G1: Attacker is not removed from parser stack if Target is invalid.
/// \remarks G2: zSpy warnings and null pointer access if Attacker is not valid.
{
};
