package com.regothproject.regoth;

public class InstallerExtract
{
    /**
     * Attempts to extract the given installer file
     * @param installerFile File to extract
     * @return Success
     */
    public static native boolean extractInstaller(String installerFile);

    /**
     * @return Whether valid gamedata to run from is available
     */
    public static native boolean hasGameData();

}
