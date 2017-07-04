package com.tool.sports.com.analysis;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.tool.sports.com.analysis.ECGLib.PipeLine;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Vector;


public class ProcessAnalysis extends AppCompatActivity {
    static String TAG = "testtest";
    public static CalmAnalysisListener onCalmnessCallback;
    public PipeLine pipeline;
    String mFileName;

    // Constructor
    public ProcessAnalysis() {
        Log.d(TAG, "Constructor");

        mFileName = getFilenameFromTime();
        pipeline = new PipeLine();
        pipeline.init(250, 5);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);
        Log.d(TAG, "onCreate");
    }

    public void startSleepAnalysis() {
        ThAnalysis threadSleepAnalysis = new ThAnalysis();
        threadSleepAnalysis.ac = this;
        threadSleepAnalysis.start();

        ThAnalysisResult threadSleepResult = new ThAnalysisResult();
        threadSleepResult.ac = this;
        threadSleepResult.start();
    }

    public void startCalm() {
        Log.d(TAG, "Calm");

        if (Static1.ncalm == 0) {
            Static1.ncalm += 1;

            ThCalmResult thread = new ThCalmResult();
            thread.ac = this;
            thread.start();
            if (Static1.isCalmDebug) {
                startCSVExport();
                String strCSVSaveDirectory = getCalmSaveDirectory();
                double[] d = {3, 4};
                AddEcgData(d, strCSVSaveDirectory, "test");
                ThCalmTester thread3 = new ThCalmTester();
                thread3.ac = this;
                thread3.start();
            }
        }
    }

    public String getFilenameFromTime() {
        final Calendar c = Calendar.getInstance();
        int m = c.get(Calendar.MONTH);
        int d = c.get(Calendar.DATE);
        int h = c.get(Calendar.HOUR_OF_DAY);
        int min = c.get(Calendar.MINUTE);

        String[] MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        return MONTH[m] + "-" + d + " " + h + "-" + min;
    }

    private boolean isCSVExport = false;

    public void startCSVExport() {
        isCSVExport = true;
        mFileName = getFilenameFromTime();
        initCalmnessInC();
        Log.d("csvexport", "" + isCSVExport);
    }

    private String mMacAddress = "";

    public void startCSVExport(String macAddress) {
        isCSVExport = true;
        mMacAddress = macAddress;
        mFileName = getFilenameFromTime();
        initCalmnessInC();
        Log.d("startcsvexport", macAddress + ", " + isCSVExport);
    }

    public void stopCSVExport() {
        isCSVExport = false;
        Log.d("csvexport", "" + isCSVExport);
    }


    public static void setOnCalmnessCallback(CalmAnalysisListener callbacks) {
        Log.d("callbackTest", "calmness");
        onCalmnessCallback = callbacks;
    }

    public static String SAVE_DIRECTORY = "Report";

    private String getCalmSaveDirectory() {
        String strSDCard = Environment.getExternalStorageDirectory().getPath();
        String strDir = SAVE_DIRECTORY;
        if (mMacAddress.length() > 0) {
            strDir += "/" + mMacAddress;
        }

        boolean res = createDirIfNotExists(strDir);
        Log.d("createdir", "" + res);
        return strSDCard + "/" + strDir;
    }

    Handler handler = new Handler();

    private double ecgTransform(int ecgVal) {
        return (double) ((ecgVal - 1200) / 800f);
    }

    private int ecg_No_Transform(double ecgVal) {
        return (int) ((ecgVal) * 800f + 1200);
    }

    Vector<Double> mEcgArray = new Vector<>();

    public int addEcgDataOne(int ecgVal) {
        mEcgArray.add(ecgTransform(ecgVal));
        Log.d("addEcgDataOne", "" + ecgVal + " queue size: " + mEcgArray.size());

        handler.postDelayed(calmRunnable, 0);
        return ecg_af_analysis(ecgVal);
    }

    public void addEcgData(int[] nArrData) {

        for (int ecgVal : nArrData) {
            mEcgArray.add(ecgTransform(ecgVal));
            ecg_af_analysis(ecgVal);
        }
        handler.postDelayed(calmRunnable, 0);
    }

    public void addEcgDataDouble(double[] fData) {
        for (double doubleEcgVal : fData) {
            mEcgArray.add(doubleEcgVal);
            ecg_af_analysis(ecg_No_Transform(doubleEcgVal));
        }
        handler.postDelayed(calmRunnable, 0);
    }

    boolean prevbAF = false;
    int timeOfPrevAF = 0;
    public int sampleCounter = 0;
    int peakStart = 0;

    public static int IS_UNKNOWN = -1;
    public static int IS_NORMAL = 0;
    public static int IS_AF = 1;


    public int ecg_af_analysis(int data) {
        int strAF_NORMAL_UNKNOWN = IS_UNKNOWN;
        pipeline.add(data);
        if (pipeline.isDected()) {
            String str = "" + pipeline.getHeartRate();

//            Log.d("Heartrate", str);
//			bpmView.setText(str);
            int peakPos = peakStart;
//            Log.d("peakpos", "" + peakPos);

            Boolean bAF = false;
            if (pipeline.typeClassification.processwaveChars.type == 86 || pipeline.typeClassification.processwaveChars.type == 83 || pipeline.typeClassification.processwaveChars.type == 65)
                bAF = true;

            if (bAF) {
                if (!prevbAF) {
                    timeOfPrevAF = peakStart;
                }
            } else {
                if (prevbAF) {
                    String AFStart = "" + timeOfPrevAF;
                    String AFEnd = "" + peakStart;
//                    Log.d("afresult", AFStart + ":" + AFEnd);
                }
            }
            prevbAF = bAF;
//            Log.d("af result", "" + peakPos);

            String strAFandNormal;
            if (pipeline.typeClassification.processwaveChars.type == 86 || pipeline.typeClassification.processwaveChars.type == 83 || pipeline.typeClassification.processwaveChars.type == 65) {
                strAFandNormal = "AF";
                strAF_NORMAL_UNKNOWN = IS_AF;
            } else {
                strAFandNormal = "N";
                strAF_NORMAL_UNKNOWN = IS_NORMAL;
            }

//            Log.d("Heartrate", "out algo: " + strAFandNormal);
            peakStart = sampleCounter;
        }
        sampleCounter++;
        return strAF_NORMAL_UNKNOWN;
    }

    Runnable calmRunnable = new Runnable() {
        @Override
        public void run() {
            Log.d("runnableCalm", "run");
            calm();
        }
    };

    // check queue and start analysis
    private void calm() {
        int len = mEcgArray.size();
        if (len >= 3000) {
            Log.d("calmness", "" + len + ", isCSV: " + isCSVExport + " ,file: " + this.mFileName);
            double[] tmpEcgData = new double[len];
            for (int i = 0; i < len; i++) {
                tmpEcgData[i] = mEcgArray.elementAt(i);
            }
            String strCSVSaveDirectory = getCalmSaveDirectory();
            if (this.isCSVExport)
                AddEcgData(tmpEcgData, strCSVSaveDirectory, this.mFileName);
            else {
                AddEcgData(tmpEcgData, strCSVSaveDirectory, "");
            }
            mEcgArray.clear();
        }
    }


    public static boolean createDirIfNotExists(String path) {
        boolean ret = true;

        File file = new File(Environment.getExternalStorageDirectory(), path);
        if (!file.exists()) {
            if (!file.mkdirs()) {
                Log.e("createdir", "Problem creating Image folder");
                ret = false;
            }
        } else {
            Log.d("createdir", "already exist folder: '" + path + "'");
        }

        return ret;
    }

    public void callback(String str) {
        Log.d("callbackResult", "test  " + str);
    }

    public native String cmdline();

    public native void calmnessEntry();

    public native String AddEcgData(double[] d, String datapath, String filename);

    public native String AddRRIData(double[] d);

    public native void initCalmnessInC();

    public native double calmnessGetResult();

    public native double HeartRateGetResult();

    //    public native void TestCallback();
    public native String stdouterr();

    public native void analysisFile(String datapath, String filename);
//    public native void wfdbExport(String datapath,String filename);

    static {
        System.loadLibrary("native-lib");
    }
}

class ThCalmResult extends Thread {

    ProcessAnalysis ac;

    @SuppressWarnings("InfiniteLoopStatement")
    public void run() {
        try {
            for (; ; ) {

                double calm = ac.calmnessGetResult();
                if (ProcessAnalysis.onCalmnessCallback != null && calm != -1) {
                    Log.d("callbackTest", "callbak ----calm-----run:   " + calm);
                    ProcessAnalysis.onCalmnessCallback.on_calm_result(calm);
                }
                double hr = ac.HeartRateGetResult();
                if (ProcessAnalysis.onCalmnessCallback != null && hr != -1) {
                    Log.d("callbackTest", "callbak -----hr----run:   " + hr);
                    ProcessAnalysis.onCalmnessCallback.on_heart_rate_result(hr);
                }

                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                }
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}

class ThAnalysis extends Thread {
    ProcessAnalysis ac;

    public void run() {
        String strDownload = Environment.getExternalStorageDirectory().getPath();
        ac.analysisFile("/sdcard/Download/files", "t_slp01a.txt");
    }
}

class ThAnalysisResult extends Thread {
    ProcessAnalysis ac;
    FileOutputStream fos;

    String projDir() {
        String dirPath = "";
        try {
            dirPath = ac.getFilesDir().getCanonicalPath();
        } catch (IOException e) {
        }
        File dir2 = new File(dirPath);
        if (!dir2.exists()) {
            dir2.mkdirs();
        }
        dir2.setWritable(true, false);
        return dir2.getParent();
    }

    public void run() {
        try {
            ac.cmdline();
            for (; ; ) {

                String line = ac.stdouterr();
//                Log.d("sleepresult", "kkk" + line);
                if (line.length() > 0) {
                    Log.d("sleepresult", line);
                }

                try {
                    sleep(50);
                } catch (InterruptedException e) {
                    Log.e("sleep result error", e.toString());
                }
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}


class ThCalmTester extends Thread {
    ProcessAnalysis ac;

    public void run() {

        double[] newData = new double[TestDataECG.data.length * 1];
        for (int i = 0; i < TestDataECG.data.length; i++) {
            for (int j = 0; j < 1; j++) {
                newData[TestDataECG.data.length * j + i] = TestDataECG.data[i];
            }
        }
        Log.d("calmness source length", newData.length + "");
        for (int i = 0; i < 13; i++)
        {
            ac.addEcgDataDouble(newData);
            try {
                sleep(2000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
//        ac.AddRRIData(newData);

    }
}
