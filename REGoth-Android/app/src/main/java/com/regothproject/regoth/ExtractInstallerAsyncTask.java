package com.regothproject.regoth;

import android.app.AlertDialog;
import android.app.Application;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Environment;

import java.io.File;

/**
 * Created by desktop on 24.09.17.
 */

class ExtractInstallerAsyncTask extends AsyncTask<String, Integer, Long> {

    private Context context;
    private final String installerFile = Environment.getExternalStorageDirectory().getPath() + "/REGoth/installer/gothic-installer.exe";
    private final String installerTempDir = Environment.getExternalStorageDirectory().getPath() + "/REGoth/Gothic/installer-temp";
    private ProgressDialog progressBar;

    private static final int RESULT_OK = 0;
    private static final int RESULT_NO_DATA = 1;
    private static final int RESULT_FAILED_EXTRACT = 2;
    private static final int RESULT_GAMEDATA_FOUND = 3;

    ExtractInstallerAsyncTask(Context context) {
        this.context = context;
    }

    @Override
    protected void onPreExecute() {
        progressBar = new ProgressDialog(context);
        progressBar.setCancelable(false);
        progressBar.setMessage("Extracting...");
        progressBar.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressBar.show();
    }

    @Override
    protected void onPostExecute(Long result) {
        if (progressBar != null)
            progressBar.dismiss();

        switch (result.intValue()) {
            case RESULT_OK:
                showDialogExtractOk();
                break;

            default:
            case RESULT_FAILED_EXTRACT:
                showDialogFailedExtract();
                break;

            case RESULT_NO_DATA:
                showDialogNoData();
                break;

            case RESULT_GAMEDATA_FOUND:
                showDialogAlreadyExtracted();
                break;
        }
    }

    private void showDialogExtractOk() {
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this.context);
        dlgAlert.setTitle("Successfully extracted the installer");
        dlgAlert.setMessage("You may now try to start the game!");
        dlgAlert.setPositiveButton("OK", null);
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    private void showDialogFailedExtract() {
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this.context);
        dlgAlert.setTitle("Failed to extract game installer!");
        dlgAlert.setMessage("The installer could be invalid or not supported. This function currently only"
                + " supports the original installers (the ones displaying images and playing music).");
        dlgAlert.setPositiveButton("OK", null);
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    private void showDialogNoData() {
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this.context);
        dlgAlert.setTitle("No Gamedata found and no installer available");
        dlgAlert.setMessage("Download the gamedata using the Download-Button or move your games files to '/storage/REGoth/Gothic'");
        dlgAlert.setPositiveButton("OK", null);
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    private void showDialogAlreadyExtracted() {
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this.context);
        dlgAlert.setTitle("Gamedata found - no need to extract");
        dlgAlert.setMessage("If you want to re-extract, please delete the folder '/storage/REGoth/Gothic' and try again.");
        dlgAlert.setPositiveButton("OK", null);
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    @Override
    protected Long doInBackground(String... strings) {
        if (strings.length != 0)
            throw new UnsupportedOperationException("Installer location is implicit");

        return extractInstallerIfNeeded();
    }

    private long extractInstallerIfNeeded() {

        if (InstallerExtract.hasGameData())
            return RESULT_GAMEDATA_FOUND;

        if (!installerExists())
            return RESULT_NO_DATA;

        if (!extractInstaller())
            return RESULT_FAILED_EXTRACT;

        removeInstallerJunk();

        return RESULT_OK;
    }

    private boolean installerExists() {
        return new File(installerFile).exists();
    }

    private boolean extractInstaller() {
        return InstallerExtract.extractInstaller(installerFile);
    }

    private void removeInstallerJunk() {
        removeDirectory(installerTempDir);
    }

    private void removeDirectory(String path) {
        File dir = new File(path);
        if (dir.isDirectory())
        {
            String[] children = dir.list();
            for (String aChildren : children) {
                new File(dir, aChildren).delete();
            }
        }
    }
}
