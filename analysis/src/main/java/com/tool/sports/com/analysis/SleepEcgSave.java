package com.tool.sports.com.analysis;

import android.os.Handler;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Vector;

/**
 * Created by admin on 7/4/2017.
 */

public class SleepEcgSave {

    private static String TAG = "SleepEcgSave";
    public static String SLEEP_ECG_DIRECTORY = "ReportSleep";
    public static String ECG_SAMPLE_DIRECTORY = "sleepdatasample";


    private Vector<Double> mArrSleepEcg = new Vector<>();
    private String mFilename = "";

    // constructor
    public SleepEcgSave() {
        this("");
    }

    public SleepEcgSave(String fileName) {
        mFilename = fileName;
        boolean res = ProcessAnalysis.createDirIfNotExists(SLEEP_ECG_DIRECTORY);
        Log.d(TAG, "create dir result = " + res);
    }

    private boolean isSave = false;

    public void start() {
        Log.d(TAG, "start( void ) :               mFilename   ->   " + mFilename);
        mArrSleepEcg.clear();
        isSave = true;
        mIsFirst = true;
    }

    public void start(String fileName) {
        Log.d(TAG, "start( file ) : with '" + fileName + "'");
        mArrSleepEcg.clear();
        mFilename = fileName;
        isSave = true;
        mIsFirst = true;
    }

    public void stop() {
        Log.d(TAG, "stop");
        isSave = false;
    }

    public void addSleepEcg(int[] sleepEcg) {
        if (!isSave) return;
        for (int ecgVal : sleepEcg) {
            mArrSleepEcg.add(ProcessAnalysis.ecgTransform(ecgVal));
        }
        int len = mArrSleepEcg.size();
        if (len >= 3000) {
            handler.postDelayed(sleepEcgSaveRunnable, 0);
        }
    }

    public void addSleepEcg(int sleepEcg) {
        if (!isSave) return;
        this.mArrSleepEcg.add(ProcessAnalysis.ecgTransform(sleepEcg));
        int len = mArrSleepEcg.size();
        if (len >= 3000) {
            handler.postDelayed(sleepEcgSaveRunnable, 0);
        }
    }

    Handler handler = new Handler();

    public void addSleepEcg(double[] sleepEcg) {
        if (!isSave) return;
        for (double ecgVal : sleepEcg) {
            mArrSleepEcg.add(ecgVal);
        }
        int len = mArrSleepEcg.size();
        if (len >= 3000 && !isRunning) {
            handler.postDelayed(sleepEcgSaveRunnable, 0);
        }

    }

    public void addSleepEcg(double sleepEcg) {
        if (!isSave) return;
        this.mArrSleepEcg.add(sleepEcg);
        int len = mArrSleepEcg.size();
        if (len >= 3000) {
            handler.postDelayed(sleepEcgSaveRunnable, 0);
        }
    }

    private boolean isRunning = false;
    private Runnable sleepEcgSaveRunnable = new Runnable() {
        @Override
        public void run() {
            isRunning = true;
            int len = mArrSleepEcg.size();
            double[] arrDoubleEcgData = new double[len];
            for (int i = 0; i < len; i++) {
                arrDoubleEcgData[i] = mArrSleepEcg.firstElement();
                mArrSleepEcg.remove(0);
            }

            saveSleepEcgToFile(arrDoubleEcgData, mIsFirst);
            isRunning = false;
        }
    };

    private boolean mIsFirst = true;

    private void saveSleepEcgToFile(double[] arr_sleep_ecg, boolean isFirst) {
        long first = System.currentTimeMillis();

        if (arr_sleep_ecg.length == 0) {
            return;
        }
        if (mFilename.length() == 0) {
            Log.e(TAG, "File name not set");
            return;
        }

        File root = android.os.Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + "/" + SLEEP_ECG_DIRECTORY);

        File file = new File(dir, mFilename);
        try {
            FileOutputStream f = new FileOutputStream(file, true);// Append mode
            PrintWriter pw = new PrintWriter(f);
            String str = "\t" + arr_sleep_ecg[0] + "\r\n";
            if (isFirst) {
                str = String.format("0_1\t%.4f\tfd=250.; columns=|ECG;\r\n", arr_sleep_ecg[0]);
                mIsFirst = false;
            }
            for (double sleep_ecg : arr_sleep_ecg) {
                str += "\t";
                str += String.format("%.4f", sleep_ecg) + "\r\n";
            }

            pw.print(str);
            pw.flush();
            pw.close();
            f.close();
            Log.e(TAG, "file writing duration: " + (System.currentTimeMillis() - first) + ", length" + arr_sleep_ecg.length + ",  filename:" + mFilename);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            Log.i(TAG, "******* File not found. Did you" +
                    " add a WRITE_EXTERNAL_STORAGE permission to the   manifest?");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Vector<String> getSleepAnalsysResult(String _resultFileName) {
        File root = android.os.Environment.getExternalStorageDirectory();
        String readFile = root.getAbsolutePath() + "/" + SLEEP_ECG_DIRECTORY + "/" + _resultFileName;
        Log.d("sleepresult", "filename: " + readFile);
        Vector<String> arrResult = new Vector<>();

        InputStream inStream;
        try {
// open the file for reading
            inStream = new FileInputStream(readFile);

// if file the available for reading
            if (inStream != null) {
                // prepare the file for reading
                InputStreamReader inputreader = new InputStreamReader(inStream);
                BufferedReader buffreader = new BufferedReader(inputreader);

                String line;

                // read every line of the file into the line-variable, on line at the time
                do {
                    line = buffreader.readLine();
                    if (line != null)
                        arrResult.add(line);
                    // do something with the line
                } while (line != null);
                inStream.close();

            }
        } catch (Exception ex) {
            // print stack trace.
        }
        Log.d("sleepresult", "return size: " + arrResult.size() + "");

        return arrResult;
    }
}
