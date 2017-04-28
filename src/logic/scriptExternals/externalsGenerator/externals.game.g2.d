///
/// \weakgroup parser_game GAME parser
/// @{
///

FUNC void AI_PlayFX(VAR C_NPC Origin, VAR C_VOB Target, VAR C_VFX_NAME VfxName)
/// \brief      Starts a visual effect on a character (queued).
/// \param      Origin
///                 Reference to the character object
///                 where the visual effect is played.
/// \param      Target
///                 Optional reference to a game object (character or item)
///                 that will be used as the target for the visual effect.
/// \param      VfxName
///                 Name of the visual effect.
/// \details    The message queue handler does nothing if \b GAME.itemEffects
///             is disabled in the Gothic.ini, or if \p VfxName is empty, or
///             if an unfinished visual effect with the same name exists for
///             the \p Origin. The name comparison for the existing effect
///             is case sensitive, but \p VfxName will be converted to upper
///             case before creating the visual effect.
/// \details    The effect is created with level 1 (VFX parser instance
///             "<VFXNAME>_L1" instead of "<VFXNAME>" if present),
///             without damage, and not as projectile.
/// \note       The function is only available in G2.
{
};

FUNC BOOL AI_PrintScreen(VAR string Text, VAR int PosX, VAR int PosY, VAR string Font, VAR int Seconds)
/// \brief      Displays a single text line on the game view (queued).
/// \param      Text
///                 Message that is displayed on a single line.
/// \param      PosX
///                 Left position in percent, or -1 for horizontal centering.
/// \param      PosY
///                 Top position in percent, or -1 for vertical centering.
/// \param      Font
///                 Optional filename of the font (including .TGA extension).
/// \param      Seconds
///                 Specifies how long the message is displayed.
/// \return     Returns TRUE if the game view exists, even if the player does
///             not exist and no message has been queued.
/// \details    If \p Font is not an empty string, the message queue handler
///             verifies that the game view font has been successfully changed.
///             The filename comparison for the active font is case sensitive,
///             but the font manager converts all filenames to upper case.
/// \details    If the game view already displays a line at the specified
///             position, the existing line is replaced.
/// \note       The function is only available in G2.
/// \bug        The implementation removes an additional int from
///             the stack after retrieving the defined parameters.
{
	return FALSE;
};

FUNC void AI_Snd_Play(VAR C_NPC Origin, VAR string Name)
/// \note       The function is only available in G2.
{
};

FUNC void AI_Snd_Play3D(VAR C_NPC Origin, VAR C_VOB Emitter, VAR string Name)
/// \note       The function is only available in G2.
{
};

FUNC void AI_StopFX(VAR C_NPC Origin, VAR C_VFX_NAME VfxName)
/// \note       The function is only available in G2.
{
};

FUNC void Doc_SetLevelCoords(VAR DOC_HANDLE Map, VAR int LevelLeft, VAR int LevelTop, VAR int LevelRight, VAR int LevelBottom)
/// \note       The function is only available in G2.
{
};

FUNC void ExitSession()
/// \note       The function is only available in G2.
{
	ExitGame();
};

FUNC BOOL Game_InitGerman()
/// \note       The function is only available in the German version of G2.
{
	return FALSE;
};

FUNC BOOL Game_InitEnglish()
/// \note       The function is only available in the English version of G2.
{
	return FALSE;
};

FUNC BOOL Game_InitEngIntl()
/// \note       The function is only available in the international version of G2.
{
	return FALSE;
};

FUNC void Npc_ClearInventory(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
};

FUNC BOOL Npc_GetActiveSpellIsScroll(VAR C_NPC Character)
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return FALSE;
};

FUNC int Npc_GetHeightToItem(VAR C_NPC Origin, VAR C_ITEM Target)
/// \note       The function is only available in G2.
{
	return 0;
};

FUNC int Npc_GetHeightToNpc(VAR C_NPC Origin, VAR C_NPC Target)
/// \note       The function is only available in G2.
{
	return 0;
};

FUNC int Npc_GetLastHitSpellCat(VAR C_NPC Character)
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC int Npc_GetLastHitSpellID(VAR C_NPC Character)
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC C_NPC Npc_GetLookAtTarget(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	Hlp_GetNpc(-1);
};

FUNC int Npc_GetPortalGuild(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	return -1;
};

FUNC C_NPC Npc_GetPortalOwner(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	Hlp_GetNpc(-1);
};

FUNC int Npc_IsDrawingSpell(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	return -1;
};

FUNC BOOL Npc_IsDrawingWeapon(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC BOOL Npc_IsInPlayersRoom(VAR C_NPC Character)
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC void Npc_StopAni(VAR C_NPC Character, VAR string AniName)
/// \note       The function is only available in G2.
{
};

FUNC BOOL PlayVideoEx(VAR string FileName, VAR BOOL ScreenBlend, VAR BOOL SessionExit)
/// \brief      Extended version of PlayVideo().
/// \param      FileName
///                 Video filename relative to DIR_VIDEO (System\\Paths.d).
/// \param      ScreenBlend
///                 If \p SessionExit is FALSE it specifies if the
///                 visual effect "BLACK_SCREEN" is played on the player
///                 after the video playback is completed.
/// \param      SessionExit
///                 Specifies if \p ScreenBlend is ignored and the game session
///                 is closed after the after the video playback is completed.
/// \return     Returns FALSE if the video player is not available and
///             \p SessionExit is FALSE, else always TRUE
///             (even if the video playback fails).
/// \details    The video file has to exists on the disk (physical),
///             playback of virtual files (in VDF volumes) is not supported.
/// \details    All sounds are stopped, the music system is disabled, the game
///             session is paused, and a new view with black background and
///             "videoback.tga" as background texture is created before the
///             video playback starts. The current video resolution has to be
///             at least the size of the video, else the video is not played.
///             The video is scaled to the current resolution if the Gothic.ini
///             option \b GAME.scaleVideos is enabled (default), else the video
///             is centered. After the video has been skipped or ends, the game
///             session unpaused or closed, and the music system is re-enabled
///             (if the Gothic.ini option \b SOUND.musicEnabled is enabled).
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC BOOL Wld_DetectNpcExAtt(VAR C_NPC NonPlayer, VAR C_NPC_ID NpcId, VAR func AIState, VAR int Guild, VAR BOOL DetectPlayer, VAR int Attitude)
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC BOOL Wld_IsRaining()
/// \brief      Determines if it's currently raining or snowing.
/// \return     Returns TRUE if the outdoor sky controller is active and
///             the current precipitation intensity is greater than 30%.
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC void Wld_StopEffect(VAR C_VFX_NAME VfxName)
/// \brief      Stops all visual effects with a specified name.
/// \param      VfxName
///                 Name of the visual effect.
/// \note       The comparison is case sensitive
///             (\p VfxName should be in upper case).
/// \details    The visual effect objects are searched in the player's world.
/// \note       The function is only available in G2.
{
};

///
/// @}
///
