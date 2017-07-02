#include "MyArr.h"
#include "TFftAlgorithm.h"

MyArray subArray(MyArray arr, int from, int length) {
    MyArray data;

    int until = from + length;
    for (unsigned int i = (unsigned int) from; i < until; ++i) {
        data.push_back(arr.at(i));
    }

    return data;
}

MyArray subArray(MyArray arr, int from) {
    MyArray data;
    int len = arr.size();

    for (unsigned int i = (unsigned int) from; i < len; ++i) {
        data.push_back(arr.at(i));
    }
    return data;
}

MyArray pow2(MyArray a1) {
    MyArray data;
    int len = a1.size();
    for (unsigned int i = 0; i < len; ++i) {
        data.push_back(a1.at(i) * a1.at(i));
    }
    return data;
}

MyArray fft(MyArray a1) {

//    LOGD("------------------ fft begin       length %d", length(a1));
    //a1= seq(1,2048); //16->8// 24->  //32->16  // 64 ->32  //128->64
    int len = a1.size();
    TFftAlgorithm *fft = new TFftAlgorithm(len);

    fft->fMul = len / 2.0f;

    fft->setData(a1);
    fft->CalcFFT();

//    MyArray real = fft->getReal();
//    MyArray img= fft->getImage();
//    MyArray abs2= fft->getAbsPow2();
//    show(real,0,10);
//    show(img,0,10);

    MyArray arrData = fft->getAbsPow2();
//    LOGD("------------------ fft end      length %d", length(arrData));
    return arrData;
}

//MyArray fft(MyArray a1) {
//    CFourier fft;
//    FLOAT *data = myArrToFLOAT(a1);
//    LOGD("----------------fft test BEGIN----------------");
//    for (unsigned int i = 0; i < 20; ++i) {
//        LOGD("float test %f", data[i]);
//    }
//    fft.ComplexFFT(data,a1.size(), a1.size(),1);
//    LOGD("----------------fft test AFTER----------------");
//    for (unsigned int i = 0; i < 20; ++i) {
//        LOGD("float test %f", data[i]);
//    }
//    LOGD("----------------fft test   END----------------");
//    return a1;
//}

MyArray rep(FLOAT value, int rep) {
    MyArray data;
    for (unsigned int i = 0; i < rep; i++) {
        data.push_back(value);
    }
    return data;
}

MyArray concat(MyArray a1, MyArray a2) {
    MyArray data;
    int len1 = a1.size();
    for (unsigned int i = 0; i < len1; i++) {
        data.push_back(a1.at(i));
    }
    int len2 = a2.size();
    for (unsigned int i = 0; i < len2; i++) {
        data.push_back(a2.at(i));
    }
    return data;
}

MyArray concat(MyArray a1, FLOAT f) {
    MyArray data;
    int len = a1.size();
    for (unsigned int i = 0; i < len; i++) {
        data.push_back(a1.at(i));
    }
    data.push_back(f);

    return data;
}

MyArray arrayMul(MyArray a1, double v) {
    MyArray data;
    LOGD("mul %lf", v);
    int len = a1.size();
    for (unsigned int i = 0; i < len; i++) {
        data.push_back(a1.at(i) * v);
    }
    return data;
}

MyArray seq(int from, int to) {
    MyArray data;
    for (unsigned int i = from; i <= to; ++i) {
        data.push_back(i);
    }
    return data;
}

int length(MyArray a) {
    return a.size();
}

FLOAT *myArrToFLOAT(MyArray a1) {
    FLOAT *arrFLOAT;
    int len = a1.size();
    arrFLOAT = new FLOAT[len];
    for (unsigned int i = 0; i < len; i++) {
        arrFLOAT[i] = a1.at(i);
    }
    return arrFLOAT;
}

FLOAT sum(MyArray a) {
    FLOAT data = 0;
    int len = a.size();
    for (unsigned int i = 0; i < len; ++i) {
        data += a.at(i);
    }
    return data;
}


MyExArray frame(MyArray a1, MyArray a2) {
    MyExArray data;
    int len1 = a1.size();
    int len2 = a2.size();
    if (len1 != len2) {
        return data;
    }
    for (unsigned int i = 0; i < len1; ++i) {
        data.freq.push_back(a1.at(i));
        data.spec.push_back(a2.at(i));
    }
    return data;
}

int nrow(MyExArray aEx) {
    return aEx.freq.size();
}

FLOAT sum(FLOAT f1, FLOAT f2, FLOAT f3) {
    return f1 + f2 + f3;
}

FLOAT sum(FLOAT f1, FLOAT f2) {
    return f1 + f2;
}

FLOAT max(FLOAT f1, FLOAT f2, FLOAT f3) {
    FLOAT r1 = 0;
    if (f1 < f2)r1 = f2;
    else r1 = f1;
    if (r1 < f3)r1 = f3;
    return r1;
}

FLOAT min(FLOAT f1, FLOAT f2, FLOAT f3) {
    FLOAT r1 = 0;
    if (f1 > f2)r1 = f2;
    else r1 = f1;
    if (r1 > f3)r1 = f3;
    return r1;
}

int length(My6Array a6) {
    return a6.ar_vlf_sum.size();
}

int length(MyExArray a) {
    return a.freq.size();
}

MyArray getByIndexArr(MyArray dataArr, MyArray indexArr) {
    MyArray data;
    int len = length(indexArr);
    for (unsigned int i = 0; i < len; i++) {
//      LOGD("for index %d length %d",i,length(data));
        data.push_back(dataArr.at((unsigned int) indexArr.at(i)));
    }
    return data;
}

MyArray condition1(MyExArray exArray) {
    MyArray data;
    int len = length(exArray);
    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.003 && exArray.freq.at(i) <= 0.04) {
            data.push_back(exArray.spec.at(i));
        }
    }
    return data;
}

MyArray condition2(MyExArray exArray) {
    MyArray data;
    int len = length(exArray);
    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.04 && exArray.freq.at(i) <= 0.15) {
            data.push_back(exArray.spec.at(i));
        }
    }
    return data;
}

MyArray condition3(MyExArray exArray) {
    MyArray data;
    int len = length(exArray);
    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.15 && exArray.freq.at(i) <= 0.40) {
            data.push_back(exArray.spec.at(i));
        }
    }
    return data;
}

void show(MyArray a1) {
    int len = length(a1);
    for (unsigned int i = 0; i < len; ++i) {
        LOGE1("%d      %f", i, a1.at(i));
    }
}

void show(MyArray a1, string strFilter) {
    int len = length(a1);
    for (unsigned int i = 0; i < len; ++i) {
        LOGE1("%s      %d      %f", strFilter.c_str(), i, a1.at(i));
    }
}

void show(MyArray a1, int from, int _len) {
    int to = from + _len;
    int len = length(a1);
    if (to > len) {
        to = len;
    }
    for (unsigned int i = (unsigned int) from; i < to; ++i) {
        LOGD("%d      %f", i, a1.at(i));
    }
}

MyExArray subsetC1(MyExArray exArray) {
    MyExArray result;
    int len = length(exArray);

    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.003 && exArray.freq.at(i) <= 0.04) {
            result.spec.push_back(exArray.spec.at(i));
            result.freq.push_back(exArray.freq.at(i));
        }
    }
    return result;
}

MyExArray subsetC2(MyExArray exArray) {
    MyExArray result;
    int len = length(exArray);
    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.04 && exArray.freq.at(i) <= 0.15) {
            result.spec.push_back(exArray.spec.at(i));
            result.freq.push_back(exArray.freq.at(i));
        }
    }
    return result;
}

MyExArray subsetC3(MyExArray exArray) {
    MyExArray result;
    int len = length(exArray);
    for (unsigned int i = 0; i < len; i++) {
        if (exArray.freq.at(i) >= 0.15 && exArray.freq.at(i) <= 0.40) {
            result.spec.push_back(exArray.spec.at(i));
            result.freq.push_back(exArray.freq.at(i));
        }
    }
    return result;
}