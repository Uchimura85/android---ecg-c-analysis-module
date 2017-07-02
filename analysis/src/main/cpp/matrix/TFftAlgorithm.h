//
// Created by admin on 5/18/2017.
//

#ifndef SPORTANALYSISTESTER_TFFTALGORITHM_H
#define SPORTANALYSISTESTER_TFFTALGORITHM_H

#include <math.h>

#define PI 3.1415926
typedef struct _TKomplex {
    double real;
    double imag;
} TKomplex;

class TFftAlgorithm {
public:
    TKomplex res;
    int N;
    double *xw;
    TKomplex *y, *x;
    TKomplex *we;
public:

    TKomplex ksum(TKomplex a, TKomplex b) {
        res.real = a.real + b.real;
        res.imag = a.imag + b.imag;
        return (res);
    }

    TKomplex kdiff(TKomplex a, TKomplex b) {
        res.real = a.real - b.real;
        res.imag = a.imag - b.imag;
        return (res);
    }

    TKomplex kprod(TKomplex a, TKomplex b) {
        res.real = a.real * b.real - a.imag * b.imag;
        res.imag = a.real * b.imag + a.imag * b.real;
        return (res);
    }

    TFftAlgorithm(int nbOfSamples) {
        int i;
        N = nbOfSamples;
        x = new TKomplex[N + 1];
        y = new TKomplex[N + 1];
        xw = new double[N + 1];
        we = new TKomplex[N / 2];
        for (i = 0; i < N / 2; i++) {
            // Init look up table for sine and cosine values
            we[i].real = cos(2.0 * PI * (double) (i) / (double) (N));
            we[i].imag = sin(2.0 * PI * (double) (i) / (double) (N));
        }
    }

    void Shuffle(TKomplex *a, int n, int lo) {
        if (n > 2) {
            int i, m = n / 2;
            TKomplex *b = new TKomplex[m];
            for (i = 0; i < m; i++)
                b[i] = a[i * 2 + lo + 1];
            for (i = 0; i < m; i++)
                a[i + lo] = a[i * 2 + lo];
            for (i = 0; i < m; i++)
                a[i + lo + m] = b[i];
        }
    }

    void CalcSubFFT(TKomplex *a, int n, int lo) {

        int i, m;
        TKomplex w;
        TKomplex v;
        TKomplex h;
        if (n > 1) {
            Shuffle(a, n, lo);
            m = n / 2;
            CalcSubFFT(a, m, lo);
            CalcSubFFT(a, m, lo + m);
            for (i = lo; i < lo + m; i++) {
                // sine and cosine values from look up table
                w.real = we[(i - lo) * N / n].real;
                w.imag = we[(i - lo) * N / n].imag;
                // classic calculation of sine and cosine values
//                w.real = cos(2.0 * PI * (double)(i - lo) / (double)(n));
//                w.imag = sin(2.0 * PI * (double)(i - lo) / (double)(n));
                h = kprod(a[i + m], w);
                v = a[i];
                a[i] = ksum(a[i], h);
                a[i + m] = kdiff(v, h);
            }
        }
    }

    void CalcFFT() {
        int i;
        CalcSubFFT(y, N, 0);
        for (i = 0; i < N; i++) {
            y[i].imag = y[i].imag / (double) N * 2.0;
            y[i].real = y[i].real / (double) N * 2.0;
        }
//        y[0].imag = y[0].imag / 2.0;
//        y[0].real = y[0].real / 2.0;
    }

    void InvDFT()    // invers Fourier transformation
    {                       // rebuild the signal in real numbers
        int i, k;
        for (k = 0; k < N; k++) {
            xw[k] = 0;
            for (i = 0; i < 30; i++)    // we only take the first 30 fourier components
            {
                xw[k] = xw[k] +
                        (y[i].real * cos(2.0 * PI * (double) (i * k) / (double) (N)) +
                         y[i].imag * sin(2.0 * PI * (double) (i * k) / (double) (N)));
            }
        }
    }

    void InitRectangle() {
        int j;
        for (j = 0; j < 2048; j++) {
            this->y[j].real = 20.0;
            this->y[j].imag = 0.0;
            this->y[j + 2048].real = -20.0;
            this->y[j + 2048].imag = 0.0;
        }
        this->y[0].real = 0.0;
        this->y[0].imag = 0.0;
        this->y[4096].real = 0.0;
        this->y[4096].imag = 0.0;

        for (j = 0; j < 4096; j++)
            this->x[j] = this->y[j];
    }

    void setData(MyArray arr) {
        if (this->N != arr.size()) {
            LOGD("------------------------------------------------ fft error set data");
            return;
        }
        this->y[0].real = 0.0;
        this->y[0].imag = 0.0;
        this->y[this->N].real = 0.0;
        this->y[this->N].imag = 0.0;
        for (int j = 0; j < this->N; ++j) {
            this->y[j].real = arr.at(j);
            this->y[j].imag = 0.0;
        }
        for (int j = 0; j < this->N; ++j) {
            this->x[j] = this->y[j];
        }
    }

    FLOAT fMul = 16.0f;

    MyArray getResult() {
        MyArray data;
        for (int i = 0; i < this->N; i++) {
            data.push_back((FLOAT) this->y[i].real * this->fMul);
        }
        return data;
    }

    MyArray getReal() {
        MyArray data;
        for (int i = 0; i < this->N; i++) {
            data.push_back((FLOAT) this->y[i].real * this->fMul);
        }
        return data;
    }


    MyArray getImage() {
        MyArray data;
        for (int i = 0; i < this->N; i++) {
            data.push_back((FLOAT) this->y[i].imag * this->fMul);
        }
        return data;
    }

    MyArray getAbsPow2() {
        MyArray data;
        fMul = 1;
        for (int i = 0; i < this->N; i++) {

            FLOAT v = (FLOAT) (y[i].real * y[i].real + y[i].imag * y[i].imag);
            data.push_back(v * fMul * fMul);
        }
        return data;
    }

};

void InitTrytangle(TFftAlgorithm fft) {
    int j;
    for (j = 0; j < 2048; j++) {
        fft.y[j].real = (double) (j) * 20.0 / 2048;
        fft.y[j].imag = 0.0;
        fft.y[j + 2049].real = 20.0 - ((double) (j) * 20.0 / 2048);
        fft.y[j + 2049].imag = 0.0;
    }
    fft.y[0].real = 0.0;
    fft.y[0].imag = 0.0;
    fft.y[2048].real = 20.0;
    fft.y[2048].imag = 0.0;
    fft.y[4096].real = 0.0;
    fft.y[4096].imag = 0.0;
    for (j = 0; j < 4096; j++)
        fft.x[j] = fft.y[j];
}

void InitSaw(TFftAlgorithm fft) {
    int j;
    for (j = 0; j < 2048; j++) {
        fft.y[j].real = (double) (j) * 20.0 / 2048.0;
        fft.y[j].imag = 0.0;
        fft.y[j + 2049].real = -(double) (2048 - j) * 20.0 / 2048.0;
        fft.y[j + 2049].imag = 0.0;
    }
    fft.y[0].real = 0.0;
    fft.y[0].imag = 0.0;
    fft.y[2048].real = 20.0;
    fft.y[2048].imag = 0.0;
    fft.y[4096].real = 0.0;
    fft.y[4096].imag = 0.0;
    for (j = 0; j < 4096; j++)
        fft.x[j] = fft.y[j];
}

#endif //SPORTANALYSISTESTER_TFFTALGORITHM_H
