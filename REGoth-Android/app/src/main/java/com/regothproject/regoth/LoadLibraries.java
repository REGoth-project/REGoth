package com.regothproject.regoth;

import android.app.NativeActivity;

public class LoadLibraries extends NativeActivity {

    static {
        // List of libraries to load in (e.g. "PLCore;PLMath" etc.)
        String sharedLibrariesToLoad = "REGoth";

        // Load in all shared libraries
        String [] libraries = sharedLibrariesToLoad.split(";");
        for (int i=0; i<libraries.length; i++)
            System.loadLibrary(libraries[i]);
    }

}
