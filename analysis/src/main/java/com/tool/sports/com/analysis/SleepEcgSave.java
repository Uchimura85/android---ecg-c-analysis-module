package com.tool.sports.com.analysis;

import android.os.Handler;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Vector;

/**
 * Created by admin on 7/4/2017.
 */

public class SleepEcgSave {

    private static String TAG = "SleepEcgSave";
    private static String SLEEP_ECG_DIRECTORY = "ReportSleep";

    private Vector<Double> mArrSleepEcg = new Vector<>();
    private String mFilename = "";

    // constructor
    public SleepEcgSave(String fileName) {
        mFilename = fileName;
    }

    public void addSleepEcg(int sleepEcg) {
        this.mArrSleepEcg.add(ProcessAnalysis.ecgTransform(sleepEcg));
        new Handler().postDelayed(sleepEcgSaveRunnable, 0);
    }

    public void addSleepEcg(double sleepEcg) {
        this.mArrSleepEcg.add(sleepEcg);
        new Handler().postDelayed(sleepEcgSaveRunnable, 0);
    }

    private Runnable sleepEcgSaveRunnable = new Runnable() {
        @Override
        public void run() {
            Log.d("sleepEcgSaveRunnable", "run");
            saveSleepEcg();
        }
    };
    private boolean mIsFirst = true;

    private void saveSleepEcg() {
        int len = mArrSleepEcg.size();
        if (len >= 3000) {
            double[] tmpEcgData = new double[len];
            for (int i = 0; i < len; i++) {
                tmpEcgData[i] = mArrSleepEcg.elementAt(i);
            }
            saveSleepEcgToFile(tmpEcgData, mIsFirst);
            mIsFirst = false;
        }
    }

    private void saveSleepEcgToFile(double[] arr_sleep_ecg, boolean isFirst) {

        if (arr_sleep_ecg.length == 0) {

            return;
        }
        if (mFilename.length() == 0) {
            Log.e(TAG, "File name not set");
            return;
        }

        File root = android.os.Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + "/" + SLEEP_ECG_DIRECTORY);
        boolean res = ProcessAnalysis.createDirIfNotExists(SLEEP_ECG_DIRECTORY);
        Log.d(TAG, "" + arr_sleep_ecg.length + "create dir result = " + res);
        File file = new File(dir, mFilename);

        try {
            FileOutputStream f = new FileOutputStream(file);
            PrintWriter pw = new PrintWriter(f);
            String str = "\t" + arr_sleep_ecg[0] + "\r\n";
            if (isFirst)
                str = String.format("0_1\t%f\tfd=250.; columns=|ECG;\r\n", arr_sleep_ecg[0]);
            for (double sleep_ecg : arr_sleep_ecg) {
                str += "\t";
                str += sleep_ecg + "\r\n";
            }
            pw.print(str);
            pw.flush();
            pw.close();
            f.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            Log.i(TAG, "******* File not found. Did you" +
                    " add a WRITE_EXTERNAL_STORAGE permission to the   manifest?");
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
