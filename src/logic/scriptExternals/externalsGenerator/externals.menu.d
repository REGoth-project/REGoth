///
/// \weakgroup parser_menu MENU parser
/// @{
///

FUNC void Apply_Options_Audio()
/// \brief      Play menu apply sound.
/// \details    Plays C_SFX_DEF "MENU_APPLY" or "TORCH_ENLIGHT" from SFX parser.
{
};

FUNC void Apply_Options_Controls()
/// \brief      Play menu apply sound.
/// \details    Plays C_SFX_DEF "MENU_APPLY" or "TORCH_ENLIGHT" from SFX parser.
{
};

FUNC void Apply_Options_Game()
/// \brief      Play menu apply sound.
/// \details    Plays C_SFX_DEF "MENU_APPLY" or "TORCH_ENLIGHT" from SFX parser.
{
};

FUNC void Apply_Options_Performance()
/// \brief      Play menu apply sound.
/// \details    Plays C_SFX_DEF "MENU_APPLY" or "TORCH_ENLIGHT" from SFX parser.
{
};

FUNC void Apply_Options_Video()
/// \brief      Apply resolution from the video menu.
/// \details    Reads \b INTERNAL.vidResIndex from the Gothic.ini, validates and
///             activates the video mode, and writes the active resolution back
///             into the Gothic.ini (\b VIDEO.zVidResFullscreenX,
///             \b VIDEO.zVidResFullscreenY, and \b VIDEO.zVidResFullscreenBPP).
/// \details    Thereafter the menu apply sound is played
///             (C_SFX_DEF "MENU_APPLY" or "TORCH_ENLIGHT" in SFX parser).
/// \remark G1: Fallback to minimum (640x480x16) for invalid resolutions.
///             The width for valid resolutions is limited to 1600.
/// \remark G2: Fallback to minimum (800x600x16) for invalid resolutions.
///             If \b INTERNAL.extendedMenu in the Gothic.ini is not enabled,
///             the width is limited to 1600, the height is limited to 1200,
///             and the screen ratio has to be between 1.2 and 1.6 (inclusive).
{
};

FUNC int PlayVideo(VAR string FileName)
/// \brief      Plays a Bink video file.
/// \param      FileName
///                 Video filename relative to DIR_VIDEO (System\\Paths.d).
/// \return     Returns FALSE (0) if the video player is not available,
///             else always TRUE (even if the video playback fails).
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
///             session unpaused, and the music system is re-enabled (if the
///             Gothic.ini option \b SOUND.musicEnabled is enabled (default)).
/// \note       The function is also registered in the \ref parser_game.
{
	return FALSE;
};

FUNC void Update_ChoiceBox(VAR string MenuItemName)
/// \brief      Initializes the list of a MENU_ITEM_CHOICEBOX.
/// \param      MenuItemName
///             \arg \c "MENUITEM_AUDIO_PROVIDER_CHOICE"
///                     List of sound providers.
///             \arg \c "MENUITEM_VID_DEVICE_CHOICE"
///                     List of video device names.
///             \arg \c "MENUITEM_VID_RESOLUTION_CHOICE"
///                     List of valid video resolutions.
{
};

///
/// @}
///
