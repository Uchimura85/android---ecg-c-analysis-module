package com.tool.sports.com.sportanalysistester;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import com.tool.sports.com.analysis.ProcessAnalysis;
import com.tool.sports.com.analysis.SleepAnalysisStatusListener;
import com.tool.sports.com.analysis.SleepEcgSave;
import com.tool.sports.com.analysis.StaticGlobalVar;

import java.util.Vector;

public class MainActivity extends AppCompatActivity implements SleepAnalysisStatusListener {
    private final static int REQUEST_PERMISSION_REQ_CODE = 34; // any 8-bit number
    String mStringFileResult;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        _checkPermission();
        ProcessAnalysis a = new ProcessAnalysis();
        a.startCSVExport();
        StaticGlobalVar.isCalmDebug = true;
//        a.startCalm();
        ProcessAnalysis.setOnSleepAnalysisStatusCallback(this);
        mStringFileResult = ProcessAnalysis.getFilenameFromTime();

//        a.startSleepAnalysis("realecg.csv", "realecg.csv_result.csv");// ,t_slp01a_simple.txt
                a.startSleepAnalysis("t_slp01a.txt", "t_slp01a_simple.txt_result.csv");// ,t_slp01a_simple.txt
    }

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

    @Override
    public void on_finish_sleep_analysis(final int nStatus) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this, "on_finish_sleep_analysis: " + nStatus, Toast.LENGTH_SHORT).show();
                SleepEcgSave m = new SleepEcgSave();
                Vector<String> result = m.getSleepAnalsysResult(mStringFileResult);
                Log.d("resultresult", "size: " + result.size());
                for (int i = 0; i < result.size(); i++) {
                    Log.d("resultresult", result.elementAt(i) != null ? result.elementAt(i) : "null");
                }
            }
        });

    }
}
