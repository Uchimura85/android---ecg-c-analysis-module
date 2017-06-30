package com.tool.sports.com.analysis;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.tool.sports.com.analysis.ECGLib.PipeLine;

import java.io.File;
import java.util.Date;
import java.util.List;
import java.util.Vector;


public class ProcessAnalysis extends AppCompatActivity {
    static String TAG = "testtest";
    public static CalmAnalysisListener onCalmnessCallback;
    public PipeLine pipeline;
    String mFileName;

    // Constructor
    public ProcessAnalysis() {
        Log.d(TAG, "Constructor");
//        _checkPermission();

        mFileName = getFilenameFromTime();
        pipeline = new PipeLine();
        pipeline.init(250, 5);
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

    private String mMacAddress = "";

    public void startCSVExport(String macAddress) {
        isCSVExport = true;
        mMacAddress = macAddress;
        mFileName = getFilenameFromTime();
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

    Vector<Double> mEcgArray = new Vector<>();

    @SuppressWarnings("MismatchedReadAndWriteOfArray")

    String SAVE_DIRECTORY = "Report";

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

    public int addEcgDataOne(int ecgVal) {
        mEcgArray.add(ecgTransform(ecgVal));
        Log.d("addEcgDataOne", "" + ecgVal + " queue size: " + mEcgArray.size());

        handler.postDelayed(calmRunnable, 0);
        return AlgoProcess(ecgVal);
    }

    public void addEcgData(int[] nArrData) {

        for (int ecgVal : nArrData) {
            mEcgArray.add(ecgTransform(ecgVal));
            AlgoProcess(ecgVal);
        }
        handler.postDelayed(calmRunnable, 0);
    }

    public void addEcgDataDouble(double[] fData) {
        for (double doubleEcgVal : fData) {
            mEcgArray.add(doubleEcgVal);
            AlgoProcess(ecg_No_Transform(doubleEcgVal));
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


    public int AlgoProcess(int data) {
        int strAF_NORMAL_UNKNOWN= IS_UNKNOWN;
        pipeline.add(data);
        if (pipeline.isDected()) {
            String str = "" + pipeline.getHeartRate();
//            setHRSValueOnView(pipeline.getHeartRate());

            Log.d("Heartrate", str);
//			bpmView.setText(str);
            int peakPos = peakStart;
            Log.d("peakpos", "" + peakPos);

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
                    Log.d("afresult", AFStart + ":" + AFEnd);
                }
            }
            prevbAF = bAF;

            Log.d("af result", "" + peakPos);


            String strAFandNormal;
            if (pipeline.typeClassification.processwaveChars.type == 86 || pipeline.typeClassification.processwaveChars.type == 83 || pipeline.typeClassification.processwaveChars.type == 65) {
                strAFandNormal = "AF";
                strAF_NORMAL_UNKNOWN = IS_AF;
            } else {
                strAFandNormal = "N";
                strAF_NORMAL_UNKNOWN = IS_NORMAL;
            }

            Log.d("Heartrate", "out algo: " + strAFandNormal);


            peakStart = sampleCounter;
        }
        sampleCounter++;
        return strAF_NORMAL_UNKNOWN;
    }

    Runnable calmRunnable = new Runnable() {
        @Override
        public void run() {
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

class Th2CalmResult extends Thread {

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
//                newData[TestData.data.length * j + i] = (TestData.data[i] - 1200) / 800f;
                newData[TestData.data.length * j + i] = TestData.data[i];
            }
        }
        Log.d("steve testtesttest", newData.length + "");
//        for (int i = 0; i < 30; i++)
        ac.addEcgDataDouble(newData);
//        ac.AddRRIData(newData);
        try {
            sleep(3000);

        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
