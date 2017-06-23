#include <jni.h>
#include <string>
#include <deque>
#include <iostream>
#include <iomanip>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

void initForCallback(JNIEnv *env, jobject thiz);

int test_ex_ecg_sequential(int argc, char **argv);

int wfdb_export(int argc, char **argv);

// ********************************** global function declare
char *str2ch(std::string str);
// ********************************** end

//#include "3p_bmdx/src_code_tests/_test_bmdx_cpiomt.h"

//and/or 1,2,16 for _test_yk_c_vecm_hashx.h
//4 and/or 8 for _test_yk_cx.h
//#define bmdx_test_sections 0xff

//#include "3p_bmdx/src_code/bmdx_config.h"
//#define _test_yk_c_common_PREINCL
//  #include "3p_bmdx/src_code_tests/_test_yk_c_common.h"
//#define yk_c_cm_vecm_STATIC
//#define yk_c_cm_std_string_STATIC

//#include "3p_bmdx/src_code_tests/_test_yk_c_vecm_hashx.h"
//#include "3p_bmdx/src_code_tests/_test_yk_cx.h"
#include "3p_bmdx/src_code_tests/_test_bmdx_unity.h"
#include "common.h"
#include "matrix/MyArr.h"

using namespace bmdx;
namespace {
    typedef std::wstring::size_type _t_wz;
    namespace hlpj {
//        jstring j_jstring(JNIEnv *env, const std::wstring &s0) {
//            std::basic_string<jchar> s;
//            for (_t_wz i = 0; i < s0.length(); ++i) { s += jchar(s0[i]); }
//            return env->NewString(s.c_str(), s0.length());
//        }

        std::wstring j_wstring(JNIEnv *env, jstring s) {
            std::wstring x;
            if (s) {
                jsize n = env->GetStringLength(s);
                if (n > 0) {
                    std::basic_string<jchar> x0((unsigned int) n, jchar(' '));
                    env->GetStringRegion(s, 0, n, &x0[0]);
                    for (std::basic_string<jchar>::size_type i = 0;
                         i < x0.length(); ++i) { x += wchar_t(x0[i]); }
                }
            }
            return x;
        }
    } // end namespace hlpj
} // end anon. namespace
using namespace hlpj;

struct pipe_buf_lks {
};

struct pipe_buf {
    int input;
    int output;
    std::deque<char> buf;

    pipe_buf() : input(0), output(0) {}
};

static pipe_buf pb;
//
//struct th1 : public threadctl::ctx_base {
//    void _thread_proc() {
//        pipe_buf *ppb = *pdata<pipe_buf *>();
////        int res = 0;
////        res = setvbuf(stdout, 0, _IOLBF, 1024);
////        res = pipe(&ppb->input);
////        res = dup2(ppb->output, STDOUT_FILENO);
////        res = dup2(ppb->output, STDERR_FILENO);
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wmissing-noreturn"
//        while (true) {
//            enum {
//                nbuf = 1000
//            };
//            char bytes[nbuf];
//            int n;
//            do {
//                n = read(ppb->input, bytes, (size_t)nbuf);
//                if (n > 0) {
//                    critsec_t<pipe_buf_lks> __lock(1, -1);
//                    (void) __lock;
//                    for (int i = 0; i < n; ++i) { ppb->buf.push_back(bytes[i]); }
//                }
//            } while (n > 0);
//            sleep_mcs(100);
//        }
//#pragma clang diagnostic pop
//    }
//};
extern "C"
JNIEXPORT jstring JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_cmdline(
        JNIEnv *env,
        jobject /* this */) {
    return env->NewStringUTF(wsToBsUtf8(cmd_myexe() + L":" + cmd_string()).c_str());
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_stdouterr(
        JNIEnv *env,
        jobject /* this */) {
    critsec_t<pipe_buf_lks> __lock(1, -1);
    (void) __lock;
    std::string s;
    for (size_t i = 0; i < pb.buf.size(); ++i) {
        s += pb.buf.front();
        pb.buf.pop_front();
    }
    return env->NewStringUTF(wsToBsUtf8(bsToWs(s)).c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_analysisFile(
        JNIEnv *env,
        jobject, /* this */
        jstring _datapath, jstring _filename) {
    LOGE("testtest analysisFile function");

    cout << "Analysis Start: " << unity(d_now()).vcstr() << "\r\n";
    std::string dp = wsToBs(j_wstring(env, _datapath));

    // BEGIN ==================== dir =========================
    cout << "ls " + dp + ":\r\n";
    std::string __strSystem = "ls -l ";
    __strSystem.append(dp);
    system(__strSystem.c_str());
    // END   ==================== dir =========================

    cout << "chdir(" << dp << "): " << chdir(dp.c_str()) << "\r\n";
    int res1(0);

    try {
        char *chPtr_FilePath = (char *) env->GetStringUTFChars(_filename,
                                                               0);// jstring To std::string
        char *argv[3] = {(char *) "_", chPtr_FilePath, (char *) "30"};

        res1 = test_ex_ecg_sequential(3, argv);
        cout << "RETVAL: " << res1 << endl;
        LOGE("testtest res1 = %d", res1);
    } catch (...) {}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_wfdbExport(JNIEnv *env,
                                                             jobject, /* this */
                                                             jstring _data_path,
                                                             jstring _filename) {
    cout << "wfdb export " << "\r\n";
    std::string dp = wsToBs(j_wstring(env, _data_path));
    cout << "ls " + dp + ":\r\n";
    std::string __strSystem = "ls -l ";
    __strSystem.append(dp);
    system(__strSystem.c_str());
    cout << "chdir(" << dp << "): " << chdir(dp.c_str()) << "\r\n";
    int res1(0);

    try {
        std::string strFilename = wsToBs(j_wstring(env, _filename)); // java string To std::string
        std::vector<char> v(strFilename.length() + 1);               // string to char *
        std::strcpy(&v[0], strFilename.c_str());                     // string copy
//        char *chPtr_FilePath = &v[0];                                // string to char *

        std::string strParam = "name_src_wfdb = .\\slp04; channels = |0|ECG; fnp_dest = t_slp04__.txt; append = false; save_beats = true; save_slpst = true";
//        char *argv[7] = {".\\slp04" ,"|0|ECG",",t_slp04__.txt", "false" ,"true", "true"};
        char *argv[1] = {str2ch(strParam)};
        cout << "===========================\r\n";

        res1 = wfdb_export(1, argv);
        cout << "RETVAL: " << res1 << endl;
        system(__strSystem.c_str());

    } catch (...) {}
}


MyReturnValue VLFLFHF(MyArray data) {
    int window_size = 250 * 30; // 5 minutes
    int samplefreq = 250; // 250Hz
    MyArray n = seq(0, (window_size - 1));
    MyArray freq = arrayMul(n, samplefreq / (FLOAT) window_size);

    //MyArray spec = pow2(fft(data));
    MyArray spec = fft(data); // calculate abs(fft(data)) ^ 2 in fft() function
//    show(spec);
    LOGD("VLFLFHF length of data %d %d", length(n), length(data));
    MyExArray PS = frame(freq, spec);

    // summarize spectrum power
    FLOAT vlf_sum = sum(condition1(PS)) / (FLOAT) length(condition1(PS));
    FLOAT lf_sum = sum(condition2(PS)) / (FLOAT) length(condition2(PS));
    FLOAT hf_sum = sum(condition3(PS)) / (FLOAT) length(condition3(PS));
//    MyArray testT = condition1(PS);
//        show(testT);
    // summarize spectrum difference
    MyExArray vlf = subsetC1(PS);
    MyExArray lf = subsetC2(PS);
    MyExArray hf = subsetC3(PS);
    LOGD("VLFLFHF step3 %d %d %d", length(vlf), length(lf), length(hf));

    FLOAT vlf_dif = 0;
    for (int x = 0; x < (nrow(vlf) - 1); x++) {
        vlf_dif = sum(abs(vlf.spec.at((unsigned int) x + 1) - vlf.spec.at((unsigned int) x)),
                      vlf_dif);
    }
    LOGD("vlf_dif                    %f", vlf_dif);
    vlf_dif = vlf_dif / (FLOAT) length(condition1(PS));

    FLOAT lf_dif = 0;
    for (int x = 0; x < (nrow(lf) - 1); x++) {
        lf_dif = sum(abs(lf.spec.at((unsigned int) x + 1) - lf.spec.at((unsigned int) x)), lf_dif);
    }
    lf_dif = lf_dif / (FLOAT) length(condition2(PS));

    FLOAT hf_dif = 0;
    for (int x = 0; x < (nrow(hf) - 1); x++) {
        hf_dif = sum(abs(hf.spec.at((unsigned int) x + 1) - hf.spec.at((unsigned int) x)), hf_dif);
    }
    hf_dif = hf_dif / (FLOAT) length(condition3(PS));

    MyReturnValue rtn;
    LOGD("-----------  %f, %f, %f, %f, %f, %f", vlf_sum, lf_sum, hf_sum, vlf_dif, lf_dif, hf_dif);
    rtn.vlf_sum = vlf_sum;
    rtn.lf_sum = lf_sum;
    rtn.hf_sum = hf_sum;
    rtn.vlf_dif = vlf_dif;
    rtn.lf_dif = lf_dif;
    rtn.hf_dif = hf_dif;
    return rtn;
}

void calmnessDataSrc(MyArray RRI);

struct thread_CALMNESS_src : threadctl::ctx_base {
    void _thread_proc() {
        LOGD("thread_CALMNESS_src");
        unity &args = *pdata<unity>();
        MyArray &QFEED = args["QFEED"].ref<MyArray>();

        calmnessDataSrc(QFEED);
        LOGD("src data size %d", QFEED.size());

    }
};

void calmnessAlgo(MyArray src);

struct thread_CALMNESS_algo : threadctl::ctx_base {
    void _thread_proc() {
        LOGD("thread_CALMNESS_algo");
        unity &args = *pdata<unity>();
        MyArray &QFEED = args["QFEED"].ref<MyArray>();
        MyArray &QOUT = args["QOUT"].ref<MyArray>();


        while (true) {

            if (QFEED.size() > 0) {
                LOGD("enough %d", QFEED.size());
//                calmnessAlgo(QFEED, QOUT);
            } else {
                LOGD("no enough for calm algo");
            }
            sleep_mcs(1000000);
        }

    }
};

void calmnessOut(MyArray _calmness);

struct thread_CALMNESS_out : threadctl::ctx_base {
    void _thread_proc() {
        LOGD("thread_CALMNESS_out");
        unity &args = *pdata<unity>();
        MyArray &QOUT = args["QOUT"].ref<MyArray>();
        while (true) {
            calmnessOut(QOUT);
            sleep_mcs(1000000);
        }
    }
};

MyArray gCalmOut;
MyArray gHROut;

cref_t<MyArray> q1;
extern "C"
JNIEXPORT void JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_calmnessEntry(JNIEnv *, jobject) {

    threadctl CALM_alg, CALM_src, CALM_out;
    try {

        q1.create0(false);

        cref_t<MyArray> q2;
        q2.create0(false);

        CALM_alg.start_auto<thread_CALMNESS_algo, unity>(paramline().list_m(
                "QFEED", q1,
                "QOUT", q2
        ));

        CALM_src.start_auto<thread_CALMNESS_src, unity>(paramline().list_m(
                "QFEED", q1
        ));

        CALM_out.start_auto<thread_CALMNESS_out, unity>(paramline().list_m(
                "QOUT", q2
        ));
        return;
    }
    catch (std::exception &e) {
        LOGE("err");
        cerr << endl << "ERR " << e.what() << endl;
    }
    catch (...) {
        LOGE("err");
        cerr << endl << "C++ exception" << endl;
    }
}
int const FILETYPE_ECG = 0;
int const FILETYPE_RRI = 1;
int const FILETYPE_CALM = 2;

void saveCSV(MyVector _arrEcg, string _str_dp, string _str_filename, int type = 0) {
    string filename = _str_filename;
    LOGD("filenameempty: %s, %d", filename.c_str(), filename.length());
    if (filename.length() == 0) {
        LOGD("filenameempty");
        return;
    }
    switch (type) {
        case FILETYPE_ECG:
            filename += " ecg.csv";
            break;
        case FILETYPE_RRI:
            filename += " rri.csv";
            break;
        case FILETYPE_CALM:
            filename += " calm.csv";
            break;
        default:
            break;
    }
    // BEGIN file write
    chdir(_str_dp.c_str());
    int len = _arrEcg.size();
    string strForFile = "length = " + itocs(len, 12) + "\r\n";
    strForFile = "";// ignore Length;
    for (unsigned int i = 0; i < len; i++) {
        strForFile += ftocs(_arrEcg.at(i), 6, 8);
        strForFile += "\r\n";
    }
    int fres = file_io::save_bytes(filename.c_str(), strForFile, true);
    LOGE("filewrite %s %d fres = %d", filename.c_str(), strForFile.length(), fres);
    // END file write
}

void saveCSV(MyArray _arrEcg, string _str_dp, string _str_filename, int type = 0) {
    string filename = _str_filename;

    if (filename.length() == 0) {
        LOGD("filenameempty");
        return;
    }
    switch (type) {
        case FILETYPE_ECG:
            filename += " ecg.csv";
            break;
        case FILETYPE_RRI:
            filename += " rri.csv";
            break;
        case FILETYPE_CALM:
            filename += " calm.csv";
            break;
        default:
            break;
    }
    LOGD("filenameempty: %s, %d", filename.c_str(), filename.length());
    // BEGIN file write
    chdir(_str_dp.c_str());
    int len = _arrEcg.size();
    string strForFile = "length = " + itocs(len, 12) + "\r\n";
    strForFile = "";// ignore Length;
    for (unsigned int i = 0; i < len; i++) {
        strForFile += ftocs(_arrEcg.at(i), 6, 8);
        strForFile += "\r\n";
    }
    int fres = file_io::save_bytes(filename.c_str(), strForFile, true);
    LOGE("filenameempty %s %d fres = %d", filename.c_str(), strForFile.length(), fres);
    // END file write
}

string dp, filename;

void outHeartRate(MyArray arr_RRI) {
    int len = arr_RRI.size();
    for (unsigned int i = 0; i < len; ++i) {
        FLOAT rri = arr_RRI.at(i);
        rri = rri > 0 ? rri : 1;
        FLOAT hr = 60 / rri;
        LOGE("heartrate %f", hr);
        gHROut.push_back(hr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_AddEcgData(JNIEnv *env, jobject,
                                                             jdoubleArray fArray, jstring _jstrPath,
                                                             jstring _jstrfilename) {
    cout << "add ecg data: " << unity(d_now()).vcstr() << "\r\n";

    dp = wsToBs(j_wstring(env, _jstrPath));
    filename = wsToBs(j_wstring(env, _jstrfilename));

    jsize size = env->GetArrayLength(fArray);
    MyVector vecEcg((unsigned int) size);

    env->GetDoubleArrayRegion(fArray, 0, size, &vecEcg[0]);
    saveCSV(vecEcg, dp, filename, FILETYPE_ECG);
    ecg_data d(250);
    try {
        for (unsigned int i = 0; i < size; i++) {
            d.ecg.push_back(vecEcg.at(i));
        }
        alg_sqrs_nc a1(d.fd, d.ecg, d.beats);
        int res = a1.run(false);

        int nums = a1.arr_RRI.size();
        saveCSV(a1.arr_RRI, dp, filename, FILETYPE_RRI);
        LOGEsave("rrisize steven from %d to  %d", size, nums);
        show(a1.arr_RRI, "arr_rri_array");
        calmnessDataSrc(a1.arr_RRI);
        outHeartRate(a1.arr_RRI);
    }
    catch (std::exception &e) {
        LOGE("err");
        cerr << endl << "ERR " << e.what() << endl;
    }
    catch (...) {
        LOGE("err");
        cerr << endl << "C++ exception" << endl;
    }
}
extern "C"
JNIEXPORT jdouble JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_calmnessGetResult(JNIEnv *, jobject) {
    if (gCalmOut.size() == 0)return -1;
    jdouble f_Calm = gCalmOut.front();
    gCalmOut.pop_front();
    return f_Calm;
}
extern "C"
JNIEXPORT jdouble JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_HeartRateGetResult(JNIEnv *, jobject) {
    if (gHROut.size() == 0)return -1;
    jdouble f_hearrRate = gHROut.front();
    gHROut.pop_front();
    return f_hearrRate;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tool_sports_com_analysis_ProcessAnalysis_AddRRIData(JNIEnv *env, jobject,
                                                             jdoubleArray frriArray) {
    jsize size = env->GetArrayLength(frriArray);
    MyVector vecRRI((unsigned int) size);

    env->GetDoubleArrayRegion(frriArray, 0, size, &vecRRI[0]);
    MyArray myarrayRRI;
    for (unsigned int i = 0; i < size; i++) {
        myarrayRRI.push_back(vecRRI.at(i));
        LOGE1("RRI value %d             %f", i, vecRRI.at(i));
    }

    calmnessDataSrc(myarrayRRI);

}

void calmnessDataSrc(MyArray RRI) {
    LOGE1("---------------------------src size %d", length(RRI));
    //2. Generate IRRI from RRI data

    int allLength = length(RRI);

    MyArray IRRI;
    for (unsigned int i = 0; i < allLength; i++) {
        MyArray X = rep(RRI.at(i), (int) (RRI.at(i) * 250));
        if (1) {
            t_lock_MyArray __lock;
            (void) __lock;
            IRRI = concat(IRRI, X);
        }
    }
    calmnessAlgo(IRRI);
}

MyArray preIRRI;

void calmnessAlgo(MyArray IRRI) {
    //3. Calculate HRV Power Spectrum using fourier analysis
    int window = 250 * 30;// 5 minutes
    int interval = 250 * 10;// 1 minutes

    // Calculate
    MyArray ar_vlf_sum;
    MyArray ar_lf_sum;
    MyArray ar_hf_sum;

    MyArray ar_vlf_dif;
    MyArray ar_lf_dif;
    MyArray ar_hf_dif;

    IRRI = concat(preIRRI, IRRI);
    int count = length(IRRI) - window;
    int nTimes = count / interval;
    int remain = count % interval;

    LOGE1("count of for (ceil) %d,preIRRI=%d    IRRI size = %d remain = %d, nTimes= %d", count,
          length(preIRRI), length(IRRI),
          remain, nTimes);

    if (count <= 0 || nTimes <= 0) {
        LOGE1("count of nTimes < 0");
        preIRRI = IRRI;
        return;
    } else {
        preIRRI = subArray(IRRI, nTimes * interval, remain);
    }

//    count = count - remain;
    for (int i = 0; i < nTimes * interval; i += interval) {
//        LOGE1(" %d   count of for (ceil) %d,    IRRI size = %d remain = %d rLength = %d, nTimes= %d, ",
//              i, count, length(IRRI), remain,length(preIRRI),nTimes);
        MyArray arrSeq = seq(i, i + window - 1);
        MyArray arrByIndex = getByIndexArr(IRRI, arrSeq);

        MyReturnValue _VLFLFHF = VLFLFHF(arrByIndex);
        ar_vlf_sum = concat(ar_vlf_sum, _VLFLFHF.vlf_sum);
        ar_lf_sum = concat(ar_lf_sum, _VLFLFHF.lf_sum);
        ar_hf_sum = concat(ar_hf_sum, _VLFLFHF.hf_sum);

        ar_vlf_dif = concat(ar_vlf_dif, _VLFLFHF.vlf_dif);
        ar_lf_dif = concat(ar_lf_dif, _VLFLFHF.lf_dif);
        ar_hf_dif = concat(ar_hf_dif, _VLFLFHF.hf_dif);
    }

    My6Array df_VLFLFHF;
    df_VLFLFHF.ar_vlf_sum = ar_vlf_sum;
    df_VLFLFHF.ar_lf_sum = ar_lf_sum;
    df_VLFLFHF.ar_hf_sum = ar_hf_sum;
    df_VLFLFHF.ar_vlf_dif = ar_vlf_dif;
    df_VLFLFHF.ar_lf_dif = ar_lf_dif;
    df_VLFLFHF.ar_hf_dif = ar_hf_dif;

    // calculate calmness score
    FLOAT a = 50;
    FLOAT b = 50;
    int len = length(df_VLFLFHF);
    MyArray calmness;
    for (unsigned int i = 0; i < len; i++) {
        FLOAT PR = max(df_VLFLFHF.ar_vlf_sum.at(i), df_VLFLFHF.ar_lf_sum.at(i),
                       df_VLFLFHF.ar_hf_sum.at(i))
                   / sum(df_VLFLFHF.ar_vlf_sum.at(i), df_VLFLFHF.ar_lf_sum.at(i),
                         df_VLFLFHF.ar_hf_sum.at(i));

        FLOAT DR = max(df_VLFLFHF.ar_vlf_dif.at(i), df_VLFLFHF.ar_lf_dif.at(i),
                       df_VLFLFHF.ar_hf_dif.at(i))
                   / sum(df_VLFLFHF.ar_vlf_dif.at(i), df_VLFLFHF.ar_lf_dif.at(i),
                         df_VLFLFHF.ar_hf_dif.at(i));
        if (1) {
            t_lock_MyArray __lock;
            (void) __lock;
            calmness.push_back(a * PR + b * DR);
        }

        LOGE1("calmness %d   PR= %f DR= %f calm=%f", i, PR, DR, a * PR + b * DR);
    }
    show(calmness);
    calmnessOut(calmness);
}

void calmnessOut(MyArray _calmness) {
    int len = _calmness.size();
    LOGE1("---------------------- out -------------------  %d", len);
    saveCSV(_calmness, dp, filename, FILETYPE_CALM);
    for (unsigned int i = 0; i < len; i++) {
        gCalmOut.push_back(_calmness.front());
        _calmness.pop_front();
    }
}

char *str2ch(std::string str) {
    std::vector<char> v(str.length() + 1);               // string to char *
    std::strcpy(&v[0], str.c_str());                     // string copy
    char *chPtr = &v[0];                                 // string to char *
    return chPtr;
}

#pragma clang diagnostic pop