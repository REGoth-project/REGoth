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
/// \sa         AI_StopFX()
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
/// \sa         PrintScreen()
/// \note       The function is only available in G2.
/// \bug        The implementation removes an additional int from
///             the stack after retrieving the defined parameters.
{
	return FALSE;
};

FUNC void AI_Snd_Play(VAR C_NPC Origin, VAR string Name)
/// \brief      Playes a 2D sound effect or file (queued).
/// \param      Origin
///                 Reference to the character object where sound is queued.
/// \param      Name
///                 Optional name of the sound effect (no ".WAV" included)
///                 or name of the sound file (".WAV" included in filename).
/// \details    \p Name is always converted to upper case.
///             If \p Name is empty, the sound effect "NOWAVE" will be played.
///             The message queue handler does nothing if a sound effect or
///             direct sound (file) object with the same name already exists.
/// \todo       Document the sound effect naming convention for multi sounds.
/// \sa         Snd_Play()
/// \sa         Snd_Play3d()
/// \sa         AI_Snd_Play3D()
/// \warning    The ZenGin raises a fault if the \p Name includes ".MP3".
/// \note       The function is only available in G2.
{
};

FUNC void AI_Snd_Play3D(VAR C_NPC Origin, VAR C_VOB Emitter, VAR string Name)
/// \brief      Playes a 3D sound effect or file (queued).
/// \param      Origin
///                 Reference to the character object where sound is queued.
/// \param      Emitter
///                 Reference to a game object (character or item)
///                 that emits the sound.
/// \param      Name
///                 Optional name of the sound effect (no ".WAV" included)
///                 or name of the sound file (".WAV" included in filename).
/// \details    \p Name is always converted to upper case.
///             If \p Name is empty, the sound effect "NOWAVE" will be played.
///             The message queue handler does nothing if a sound effect or
///             direct sound (file) object with the same name already exists.
/// \todo       Document the sound effect naming convention for multi sounds.
/// \sa         Snd_Play()
/// \sa         Snd_Play3d()
/// \sa         AI_Snd_Play()
/// \note       The ZenGin raises a fault if the \p Name includes ".MP3".
/// \note       The function is only available in G2.
{
};

FUNC void AI_StopFX(VAR C_NPC Origin, VAR C_VFX_NAME VfxName)
/// \brief      Stops a visual effect on a character (queued).
/// \param      Origin
///                 Reference to the character object
///                 where the visual effect is playing.
/// \param      VfxName
///                 Name of the visual effect.
/// \details    The name comparison for the existing effect is case sensitive
///             (\p VfxName should be upper case).
/// \sa         AI_PlayFX()
/// \note       The function is only available in G2.
{
};

FUNC void Doc_SetLevelCoords(VAR DOC_HANDLE Map, VAR int Left, VAR int Top, VAR int Right, VAR int Bottom)
/// \brief      Sets the level coordinates of a map document.
/// \param      Map
///                 Handle to a map document.
/// \param      Left
///                 Level coordinate of the left border.
/// \param      Top
///                 Level coordinate of the top border.
/// \param      Right
///                 Level coordinate of the right border.
/// \param      Bottom
///                 Level coordinate of the bottom border.
/// \details    If the function is not used or all coordinates are set to 0,
///             the game uses the bounding box of the current world.
/// \sa         Doc_CreateMap()
/// \sa         Doc_SetLevel()
/// \note       The function is only available in G2.
{
};

FUNC void ExitSession()
/// \brief      Closes the current game session.
/// \details    The main menu is opened and the game cannot be continued.
/// \note       The function is only available in G2.
{
};

FUNC BOOL Game_InitGerman()
/// \brief      Does nothing.
/// \return     Always returns TRUE if present.
/// \details    This function only exists to prohibit loading the
///             compiled game scripts with other engine releases.
/// \note       The function is only available in the German release of G2.
{
	return FALSE;
};

FUNC BOOL Game_InitEnglish()
/// \brief      Does nothing.
/// \return     Always returns TRUE if present.
/// \details    This function only exists to prohibit loading the
///             compiled game scripts with other engine releases.
/// \note       The function is only available in the English release of G2.
{
	return FALSE;
};

FUNC BOOL Game_InitEngIntl()
/// \brief      Does nothing.
/// \return     Always returns TRUE if present.
/// \details    This function only exists to prohibit loading the
///             compiled game scripts with other engine releases.
/// \note       The function is only available in the international release of G2.
{
	return FALSE;
};

FUNC void Npc_ClearInventory(VAR C_NPC Character)
/// \brief      Removes all items, expect equipped and mission items.
/// \param      Character
///                 Reference to the character object.
/// \details    \b SELF, \b OTHER, and \b VICTIM are retained.
/// \note       The function is only available in G2.
{
};

FUNC BOOL Npc_GetActiveSpellIsScroll(VAR C_NPC Character)
/// \brief      Detects if a magic scroll is drawn.
/// \param      Character
///                Reference to the character object.
/// \return     Returns TRUE if the character is in fight mode FMODE_MAGIC
///             and a spell is drawn.
/// \details    The spell vs. rune detection is based on the ITEM_MULTI flag.
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return FALSE;
};

FUNC int Npc_GetHeightToItem(VAR C_NPC Origin, VAR C_ITEM Target)
/// \brief      Calculates the vertical distance to an item.
/// \param      Origin
///                 Reference to the character object.
/// \param      Target
///                 Reference to an item object.
/// \return     Returns the distance in centimeters, or 2147483647 if either
///             \p Origin is not a valid character object reference or
///             \p Target is not a valid item object reference.
/// \note       The function is only available in G2.
{
	return 0;
};

FUNC int Npc_GetHeightToNpc(VAR C_NPC Origin, VAR C_NPC Target)
/// \brief      Calculates the vertical distance to a character.
/// \param      Origin
///                 Reference to the character object.
/// \param      Target
///                 Reference to a character object.
/// \return     Returns the distance in centimeters, or 2147483647 if either
///             \p Origin or \b Target not a valid character object reference.
/// \note       The function is only available in G2.
{
	return 0;
};

FUNC int Npc_GetLastHitSpellCat(VAR C_NPC Character)
/// \todo       Document the visual effect spell tracing.
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC int Npc_GetLastHitSpellID(VAR C_NPC Character)
/// \todo       Document the visual effect spell tracing.
/// \note       The function is only available in G2.
/// \bug        The implementation does not leave the result on the stack
///             if \p Character is not a valid character object reference.
{
	return -1;
};

FUNC C_NPC Npc_GetLookAtTarget(VAR C_NPC Character)
/// \brief      Retrieves the current animation target.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns the target object or NULL.
/// \note       The function is only available in G2.
{
	Hlp_GetNpc(-1);
};

FUNC int Npc_GetPortalGuild(VAR C_NPC Character)
/// \brief      Retrieves the guild of the current room.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns the owner guild of the current portal room or -1.
/// \details    The \p Character has to be in a portal room
///             that is owned by a guild.
/// \note       The function is only available in G2.
{
	return -1;
};

FUNC C_NPC Npc_GetPortalOwner(VAR C_NPC Character)
/// \brief      Retrieves the owner of the current room.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns the owner object of the current portal room or NULL.
/// \details    The \p Character has to be in a portal room
///             that is owned by a character instance, and an object of the
///             owner instance has to exist in the game world.
/// \note       The function is only available in G2.
{
	Hlp_GetNpc(-1);
};

FUNC int Npc_IsDrawingSpell(VAR C_NPC Character)
/// \brief      Returns the spell that a character is readying.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns the spell identifier (SPL_*) of the first message
///             to ready a spell in the \b Character's queue,
///             or -1 if there is no such message.
/// \note       The function is only available in G2.
{
	return -1;
};

FUNC BOOL Npc_IsDrawingWeapon(VAR C_NPC Character)
/// \brief      Detects if a character is drawing a weapon or spell.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns TRUE if the \b Character's queue contains
///             a message to draw a weapon or to ready a spell.
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC BOOL Npc_IsInPlayersRoom(VAR C_NPC Character)
/// \brief      Detects if a character is in the same room as the player.
/// \param      Character
///                 Reference to the character object.
/// \return     Returns TRUE if both are in the same (or no) portal room.
/// \note       The function is only available in G2.
{
	return FALSE;
};

FUNC void Npc_StopAni(VAR C_NPC Character, VAR string AniName)
/// \brief      Stops and dequeues an animation.
/// \param      Character
///                 Reference to the character object
///                 where the animation is playing and/or queued.
/// \param      AniName
///                 Name of the animation to stop and dequeue.
/// \details    Only non-overlay animation messages are dequeued.
/// \sa         Npc_PlayAni() AI_PlayAni() AI_PlayAniBS()
/// \note       The comparison is case sensitive
///             (\p AniName should be upper case).
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
///                 is closed after the video playback is completed.
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
/// \brief      Finds the nearest perceived character by attitude.
/// \param      NonPlayer
///                 Reference to a character object with a perception list
///                 (not the player character).
/// \param      NpcId
///                 Optional instance of the character to find.
///                 Negative value for all instances.
/// \param      AIState
///                 Optional state function of the character to find.
///                 NOFUNC for all state functions.
/// \param      Guild
///                 Optional true guild of the character to find.
///                 Negative value for all guilds.
/// \param      DetectPlayer
///                 Specifies if the player is also found.
/// \param      Attitude
///                 Specifies the attitude (towards \p NonPlayer)
///                 of the character to find.
/// \return     Returns TRUE if a character has been found
///             and written into \b OTHER.
/// \details    Characters that are dead, unconscious, fading away,
///             or locked by magic (SPL_ICECUBE, SPL_PYROKINESIS, SPL_ICEWAVE,
///             SPL_WHIRLWIND, SPL_GREENTENTACLE, SPL_SUCKENERGY) are ignored.
/// \sa         Npc_PerceiveAll() Wld_DetectNpc() Wld_DetectNpcEx()
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
///             (\p VfxName should be upper case).
/// \details    The visual effect objects are searched in the player's world.
/// \sa         Wld_PlayEffect()
/// \note       The function is only available in G2.
{
};

///
/// @}
///
