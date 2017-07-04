// Cubic interpolation for 4 double precision points.
// Reviewed: 2017-04-17

#ifndef yk_algs_polynome3_H
#define yk_algs_polynome3_H

#include <android/log.h>
#define  LOGD1(...)  __android_log_print(ANDROID_LOG_DEBUG,"LOGD",__VA_ARGS__)

namespace algs_polynome
{

struct polynome3
{
  double a, b, c, d; bool b_valid;
  polynome3() throw() : a(0.), b(0.), c(0.), d(0.), b_valid(true) {}
  static double fabs(double x) throw() { return x >= 0. ? x : -x; }

  double f(double x) const throw() { return b_valid ? ((a * x + b) * x + c) * x + d : 0.; }

  polynome3& interpolate(const double* points, bool b_xyxyxyxy, double eps = 1.e-20) throw()
  {
    if (!points) { a = 0.; b = 0.; c = 0.; d = 0.; b_valid = false; return *this; }
    if (b_xyxyxyxy) { return interpolate(points[0], points[2], points[4], points[6], points[1], points[3], points[5], points[7], eps); }
    return interpolate(points[0], points[1], points[2], points[3], points[4], points[5], points[6], points[7], eps);
  }

  polynome3& interpolate(double x1, double x2, double x3, double x4, double y1, double y2, double y3, double y4, double eps = 1.e-20) throw()
  {
    if (eps < 1.e-300) { eps = 1.e-300; }
    b_valid = false;
    while (true) // once
    {
      double z0 = x2 - x1; if (fabs(z0) < eps) { break; }
      double z1 = (y2 - y1) / z0;
      double z2 = (x2 * x2 * x2 - x1 * x1 * x1) / z0;
      double z3 = x3 * x3 + (x1 - x3) * (x2 + x1) - x1 * x1; if (fabs(z3) < eps) { break; }
      double z4 = y3 + (x1 - x3) * z1 - y1;
      double z5 = x3 * x3 * x3 + (x1 - x3) * z2 - x1 * x1 * x1;
      double z6 = z4 / z3;
      double z7 = z5 / z3;
      double z8 = z1 - (x2 + x1) * z6;
      double z9 = z2 - (x2 + x1) * z7;
      double z10 = y1 - x1 * x1 * z6 - x1 * z8;
      double z11 = x1 * x1 * (x1 - z7) - x1 * z9;
      double z12 = y4 - x4 * x4 * z6 - x4 * z8 - z10;
      double z13 = x4 * x4 * (x4 - z7) - x4 * z9 - z11; if (fabs(z13) < eps) { break; }
      a = z12 / z13;
      b = z6 - a * z7;
      c = z8 - a * z9;
      d = z10 - a * z11;
      b_valid = true;
      return *this;
    }
    a = 0.; b = 0.; c = 0.; d = 0.;
    return *this;
  }
};

}

#endif
