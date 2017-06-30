package com.tool.sports.com.analysis;

/**
 * Created by admin on 5/11/2017.
 */

public abstract interface CalmAnalysisListener {
    public abstract void on_calm_result(double fCalmness);
    public abstract void on_heart_rate_result(double fHeartRate);
    public abstract void on_atrial_fibrillation_result(String strResult);

}
