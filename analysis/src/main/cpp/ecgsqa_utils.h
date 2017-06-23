#ifndef yk_ecgsqa_utils_H
#define yk_ecgsqa_utils_H

#include <string>
#include <vector>
#include <cmath>

namespace ecgsqa_utils
{

inline double round_mult(double x, double mult = 1.e10, double add = 0.5) { return std::floor(x * mult + add) / mult; }

inline double myfmax(double a, double b) { return a > b ? a : b; }
inline double myfmin(double a, double b) { return a < b ? a : b; }
inline double myfmax(double a, double b, double c) { return myfmax(a, myfmax(b, c)); }
inline double myfmin(double a, double b, double c) { return myfmin(a, myfmin(b, c)); }
inline long mylmax(long a, long b) { return a > b ? a : b; }
inline long mylmin(long a, long b) { return a < b ? a : b; }
inline long mylmax(long a, long b, long c) { return mylmax(a, mylmax(b, c)); }
inline long mylmin(long a, long b, long c) { return mylmin(a, mylmin(b, c)); }

inline std::string join(const std::vector<std::string>& v, const std::string& delim)
{
    std::string s;
    for(size_t i = 0; i < v.size(); ++i)
      { if (i > 0) { s += delim; } s += v[i]; }
    return s;
}

}

#endif
