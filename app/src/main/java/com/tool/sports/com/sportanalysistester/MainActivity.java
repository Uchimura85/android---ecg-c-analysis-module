package com.tool.sports.com.sportanalysistester;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.tool.sports.com.analysis.ProcessAnalysis;
import com.tool.sports.com.analysis.Static1 ;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ProcessAnalysis a = new ProcessAnalysis();
        Static1.isCalmDebug = true;
        a.startCalm();
//        a.startAnalysis();
    }
}
