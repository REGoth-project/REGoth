package com.regothproject.regoth;

import android.app.AlertDialog;
import android.app.DownloadManager;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Looper;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class StartupActivity extends ActionBarActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_startup);

        unzipAppData();
    }

    public void onStartButtonPressed(View v) {
        Intent myIntent = new Intent(this, LoadLibraries.class);
        startActivity(myIntent);
    }

    public void onDownloadGamefilesButtonPressed(View v) {
        EditText text = (EditText)findViewById(R.id.demoFilesURL);
        downloadData(Uri.parse(text.getText().toString()), v);
    }

    public void onExtractGamefilesButtonPressed(View v) {
        ExtractInstallerAsyncTask task = new ExtractInstallerAsyncTask(this);
        task.execute();
    }




    private void downloadData(Uri uri, View v) {

        DownloadManager.Request request = new DownloadManager.Request(uri);
        request.setDescription("Gamefiles of Gothic for use in REGoth");
        request.setTitle("Gothic Gamefiles");

        String datafolder = "/REGoth";

        // FIXME: Not sure how to get the real filename in all cases
        request.setDestinationInExternalPublicDir(datafolder + "/installer", "gothic-installer.exe");

        // get download service and enqueue file
        DownloadManager manager = (DownloadManager) getSystemService(Context.DOWNLOAD_SERVICE);
        manager.enqueue(request);
    }



    private void unzipAppData() {
        try {
            InputStream s = getAssets().open("shaders.zip");

            File datadir = new File(Environment.getExternalStorageDirectory().getPath() + "/REGoth");
            datadir.mkdirs();

            unzipPack(s, Environment.getExternalStorageDirectory().getPath() + "/REGoth/");

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean unzipPack(InputStream stream, String rootDirectory) {
        FileOutputStream out;
        byte buf[] = new byte[16384];
        try {
            ZipInputStream zis = new ZipInputStream(stream);
            ZipEntry entry = zis.getNextEntry();
            while (entry != null) {
                if (entry.isDirectory()) {
                    File newDir = new File(rootDirectory + entry.getName());
                    newDir.mkdir();
                } else {
                    String name = entry.getName();
                    File outputFile = new File(rootDirectory + name);
                    String outputPath = outputFile.getCanonicalPath();
                    name = outputPath
                            .substring(outputPath.lastIndexOf("/") + 1);
                    outputPath = outputPath.substring(0, outputPath
                            .lastIndexOf("/"));
                    File outputDir = new File(outputPath);
                    outputDir.mkdirs();
                    outputFile = new File(outputPath, name);
                    outputFile.createNewFile();
                    out = new FileOutputStream(outputFile);

                    int numread = 0;
                    do {
                        numread = zis.read(buf);
                        if (numread <= 0) {
                            break;
                        } else {
                            out.write(buf, 0, numread);
                        }
                    } while (true);
                    out.close();
                }
                entry = zis.getNextEntry();
            }
            return true;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    static {
        // List of libraries to load in (e.g. "PLCore;PLMath" etc.)
        String sharedLibrariesToLoad = "REGoth";

        // Load in all shared libraries
        String [] libraries = sharedLibrariesToLoad.split(";");
        for (int i=0; i<libraries.length; i++)
            System.loadLibrary(libraries[i]);
    }
}
