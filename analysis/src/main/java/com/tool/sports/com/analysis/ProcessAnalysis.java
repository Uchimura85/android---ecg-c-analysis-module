package com.tool.sports.com.analysis;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.tool.sports.com.analysis.ECGLib.PipeLine;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.util.Date;
import java.util.Vector;

public class ProcessAnalysis extends AppCompatActivity {
    static String TAG = "testtest";
    public static AnalysisListener onAnalysisCallback, onCalmnessCallback;
    public PipeLine pipleline;
    String mFileName;

    // Constructor
    public ProcessAnalysis() {
        Log.d(TAG, "Constructor");
//        _checkPermission();

        mFileName = getFilenameFromTime();
        pipleline = new PipeLine();
        pipleline.init(250, 5);
    }
/*
    private final static int REQUEST_PERMISSION_REQ_CODE = 34; // any 8-bit number

    private boolean _checkPermission() {
        if (ContextCompat.checkSelfPermission(this.getApplicationContext(), Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // When user pressed Deny and still wants to use this functionality, show the rationale
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_PERMISSION_REQ_CODE);
            }
        }
        if (ContextCompat.checkSelfPermission(this.getApplicationContext(), Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // When user pressed Deny and still wants to use this functionality, show the rationale
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.READ_EXTERNAL_STORAGE)) {
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, REQUEST_PERMISSION_REQ_CODE);
            }
        }

        return false;
    }

    //*/
    @SuppressWarnings("deprecation")
    public String getFilenameFromTime() {
        Date date = new Date();
//        int y = date.getYear();
        int m = date.getMonth();
        int d = date.getDate();
        int h = date.getHours();
        int min = date.getMinutes();
//        int sec = date.getSeconds();
        String[] MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        String str = MONTH[m] + "-" + d + " " + h + "-" + min;
        Log.d("testtest", str);
        return str;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);
        Log.d(TAG, "onCreate");
    }

    private boolean isCSVExport = false;

    public void startCSVExport() {
        isCSVExport = true;
        mFileName = getFilenameFromTime();
        Log.d("csvexport", "" + isCSVExport);
    }

    public void stopCSVExport() {
        isCSVExport = false;
        Log.d("csvexport", "" + isCSVExport);
    }

    public static void setOnAnalysisCallback(AnalysisListener callbacks) {
        onAnalysisCallback = callbacks;
    }

    public static void setOnCalmnessCallback(AnalysisListener callbacks) {
        Log.d("callbackTest", "calmness");
        onCalmnessCallback = callbacks;
    }

    public void startAnalysis() {
        Log.d(TAG, "startAnalysis" + Static1.nact);
        String str = cmdline().length() + " [" + cmdline() + "]\r\n";
        if (Static1.nact == 0) {
            Static1.nact += 1;

            Th1AnalysisResult thread_result = new Th1AnalysisResult();
            thread_result.ac = this;
            thread_result.start();

            Th2Analysis thread_analysis = new Th2Analysis();
            thread_analysis.ac = this;
            thread_analysis.start();
        } else {
            Log.d("testtest", "nact  " + Static1.nact);
        }
    }

    int peakStart = 0;
    boolean prevbAF = false;
    int timeOfPrevAF = 0;

    Vector<Double> mEcgArray = new Vector<>();

    @SuppressWarnings("MismatchedReadAndWriteOfArray")
    public void addEcgData(double[] fData) {

        String saveDirectory = "CALM";
        String strDownload = Environment.getExternalStorageDirectory().getPath();
        String state = Environment.getExternalStorageState();
        strDownload += "/"+saveDirectory;
        boolean res = createDirIfNotExists(saveDirectory);
        Log.d("filenameempty",""+res);


        Log.d("filewrite", "testtest: " + strDownload + "   " + state);
        //          "/sdcard/Download/files"
        int len = mEcgArray.size();
        if (len > 1000) {
            double[] tmpEcgData = new double[len];
            for (int i = 0; i < len; i++) {
                tmpEcgData[i] = mEcgArray.elementAt(i);
            }
            if (this.isCSVExport)
                AddEcgData(tmpEcgData, strDownload, this.mFileName);
            else {
                if(Static1.isCalmDebug)
                AddEcgData(tmpEcgData, strDownload, this.mFileName);
                else
                    AddEcgData(tmpEcgData, strDownload, "");
            }
            mEcgArray.clear();
        }

        //*/
    }
    public static boolean createDirIfNotExists(String path) {
        boolean ret = true;

        File file = new File(Environment.getExternalStorageDirectory(), path);
        if (!file.exists()) {
            if (!file.mkdirs()) {
                Log.e("TravellerLog :: ", "Problem creating Image folder");
                ret = false;
            }
        }
        return ret;
    }
    public void startCalm() {
        Log.d(TAG, "Calm");

        if (Static1.ncalm == 0) {
            Static1.ncalm += 1;

            Th2CalmResult thread = new Th2CalmResult();
            thread.ac = this;
            thread.start();
            if (Static1.isCalmDebug) {
                Th3CalmTester thread3 = new Th3CalmTester();
                thread3.ac = this;
                thread3.start();
            }
        }
    }

    boolean isRecording = false;

    public void startRecord() {
        isRecording = true;
    }

    public void stopRecord() {
        isRecording = false;
    }

    public void callback(String str) {
        Log.d("callbackResult", "test  " + str);
    }

    public native String cmdline();

    public native void calmnessEntry();

    public native String AddEcgData(double[] d, String datapath, String filename);

    public native String AddRRIData(double[] d);


    public native double calmnessGetResult();
    public native double HeartRateGetResult();



    //    public native void TestCallback();
    public native String stdouterr();

    public native void analysisFile(String datapath, String filename);
//    public native void wfdbExport(String datapath,String filename);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}

class Th1AnalysisResult extends Thread {

    ProcessAnalysis ac;

    @SuppressWarnings("InfiniteLoopStatement")
    public void run() {
        try {
            for (; ; ) {
                try {
                    String line = ac.stdouterr();
                    if (line.length() > 0) {
                        Log.d("coutresult", "file write:" + line);
                        if (ProcessAnalysis.onAnalysisCallback != null) {
                            JSONObject json = new JSONObject();
                            json.put("value", line);
                            ProcessAnalysis.onAnalysisCallback.success(json);
                        }
                    }
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }

                try {
                    sleep(50);
                } catch (InterruptedException e) {
                    Log.e("error", e.toString());
                }
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

}

class Th2CalmResult extends Thread {

    ProcessAnalysis ac;

    @SuppressWarnings("InfiniteLoopStatement")
    public void run() {
        try {
            for (; ; ) {
                try {
                    double calm = ac.calmnessGetResult();
                    if (ProcessAnalysis.onCalmnessCallback != null && calm != -1) {
                        Log.d("callbackTest", "callbak ----calm-----run:   " + calm);
                        JSONObject json = new JSONObject();
                        json.put("value", calm);
                        json.put("type", "calm");
                        ProcessAnalysis.onCalmnessCallback.success(json);
                    }
                    double hr = ac.HeartRateGetResult();
                    if (ProcessAnalysis.onCalmnessCallback != null && hr != -1) {
                        Log.d("callbackTest", "callbak -----hr----run:   " + hr);
                        JSONObject json = new JSONObject();
                        json.put("value", hr);
                        json.put("type", "hr");
                        ProcessAnalysis.onCalmnessCallback.success(json);
                    }

                } catch (Exception e) {
                    throw new RuntimeException(e);
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

class Th2Analysis extends Thread {
    ProcessAnalysis ac;

    public void run() {
        String strDownload = Environment.getExternalStorageDirectory().getPath();
        ac.analysisFile("/sdcard/Download/files", "t_slp01a.txt");
    }
}

class Th3CalmTester extends Thread {
    ProcessAnalysis ac;

    public void run() {

        double[] newData = new double[TestData.data.length * 1];
        for (int i = 0; i < TestData.data.length; i++) {
            for (int j = 0; j < 1; j++) {
                newData[TestData.data.length * j + i] = (TestData.data[i] - 1200) / 800f;
            }
        }
        for (int i = 0; i < 30; i++)
            ac.addEcgData(newData);

        try {
            sleep(3000);

        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
