package com.tool.sports.com.analysis;

import android.util.Log;

/**
 * Created by admin on 5/10/2017.
 */

public class TestTest implements PAnanlysisInterface{
    public TestTest(){
        Log.d("TestTest","test123123123");
    }
    public void show(){
        Log.d("TestTest","test123123123");
    }

    @Override
    public void onReceivedResult(String strResult) {
        Log.d("TestTest", strResult);
    }
}
