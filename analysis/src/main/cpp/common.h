#ifndef ex_ecg_sequential_H
#define ex_ecg_sequential_H

#include <deque>

using namespace std;

#include "3p_bmdx/src_code/bmdx_main.h"

using namespace bmdx;
using namespace bmdx_str::conv;

#include "ecgsqa_algos.h"

using namespace ecgsqa;

#include <android/log.h>

#define  LOG_TAG    "C_TAG"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE1(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGEsave(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

struct feed_value {
    double x;
    int type; // 0 - data sample, 1 - sampling frequency
    feed_value() : x(0), type(-1) {}

    feed_value(double x_, int type_) : x(x_), type(type_) {}
};

typedef deque<feed_value> t_QFEED;

struct t_lock_QFEED : critsec_t<t_QFEED> {
    t_lock_QFEED() : critsec_t<t_QFEED>(100, -1) {}
};

struct timed_beat : beat_info {
    double time_beat;
    double time_slpst;
    string slpst;

    timed_beat() : time_beat(0), time_slpst(0) {}
};

typedef deque<timed_beat> t_QOUT;

struct t_lock_QOUT : critsec_t<t_QOUT> {
    t_lock_QOUT() : critsec_t<t_QOUT>(100, -1) {}
};

typedef double FLOAT;
typedef deque<FLOAT> MyArray;
typedef std::vector<double> MyVector;
typedef struct _test {
    MyArray freq;
    MyArray spec;
} MyExArray;
typedef struct _returnValue {
    FLOAT vlf_sum, lf_sum, hf_sum, vlf_dif, lf_dif, hf_dif;
} MyReturnValue;
typedef struct _My6Array {
    MyArray ar_vlf_sum, ar_lf_sum, ar_hf_sum, ar_vlf_dif, ar_lf_dif, ar_hf_dif;
} My6Array;

struct t_lock_MyArray : critsec_t<MyArray> {
    t_lock_MyArray() : critsec_t<MyArray>(100, -1) {}
};

#endif
