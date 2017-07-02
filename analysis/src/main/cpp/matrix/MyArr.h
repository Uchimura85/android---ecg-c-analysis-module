//
// Created by admin on 5/18/2017.
//

#ifndef SPORTANALYSISTESTER_MYARR_H
#define SPORTANALYSISTESTER_MYARR_H

#include "../common.h"

MyArray rep(FLOAT value, int rep);
MyArray concat(MyArray a1, MyArray a2);
MyArray concat(MyArray a1, FLOAT f);

MyArray subArray(MyArray arr,int from, int length);
MyArray subArray(MyArray arr, int from);

MyArray pow2(MyArray a1);
MyArray fft(MyArray a1);
MyArray arrayMul(MyArray a1, double v);
//MyArray arrayDiv(MyArray a1, FLOAT v);

MyArray seq(int from, int to);

FLOAT* myArrToFLOAT(MyArray a1);
FLOAT sum(MyArray a);
FLOAT sum(FLOAT f1,FLOAT f2,FLOAT f3);
FLOAT sum(FLOAT f1,FLOAT f2);
FLOAT max(FLOAT f1,FLOAT f2,FLOAT f3);
FLOAT min(FLOAT f1, FLOAT f2, FLOAT f3);
MyArray condition1(MyExArray exArray);
MyArray condition2(MyExArray exArray);
MyArray condition3(MyExArray exArray);

void show(MyArray a1);
void show(MyArray a1,string strFilter); // show array with filter(Log filter)
void show(MyArray a1, int from, int _len);
MyExArray subsetC1(MyExArray exArray);
MyExArray subsetC2(MyExArray exArray);
MyExArray subsetC3(MyExArray exArray);
MyArray getByIndexArr(MyArray dataArr,MyArray indexArr);
int length(MyArray a);
int length(MyExArray a);

MyExArray frame(MyArray a1,MyArray a2);
int nrow(MyExArray aEx);

int length(My6Array a6);
#endif //SPORTANALYSISTESTER_MYARR_H
