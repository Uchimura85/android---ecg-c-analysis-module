#ifndef yk_ecgsqa_utils2_H
#define yk_ecgsqa_utils2_H

#include <string>
#include <vector>
#include <cmath>
#include "3p_bmdx/src_code/bmdx_main.h"

namespace ecgsqa_utils
{
    using namespace bmdx;

    // Program confguration tree. Initialized with _cfg_init() on entering main().
#ifndef ecgsqa_utils_extern_cfg
    unity cfg;
#else
    extern unity cfg;
#endif
    inline void _cfg_init(const std::wstring& s) { cfg = paramline().decode_tree(s, 24); }


    // Converting program arguments
    //   into bmdx::paramline::decode_tree format.
    // 1. Joins args [i1..i2) with space.
    // 2. Converts regexp "(^|[^\\])(\\\\)*;([ \t]*)=" to "\1\2;\n\3=",
    //   so all records starting from "=" will start from new line.
    inline std::string _argv2pltree(const char** argv, int i1, int i2)
    {
        std::string s, s2;
        while (i1 < i2)
        {
            if (!s.empty()) { s += ' '; }
            s += argv[i1++];
        }
        size_t pos = 0;
        size_t pos2, pos3, pos4;
        const size_t end = std::string::npos;
        while (true)
        {
            pos2 = s.find(';', pos);
            if (pos2 == end) { s2 += s.substr(pos); break; }
            if (pos2 == pos) { s2 += ';'; pos += 1; continue; }

            pos3 = pos2;
            while (pos3 > pos) { if (s[pos3 - 1] != '\\') { break; } pos3 -= 1; }

            bool b_lf = false;
            if (((pos2 - pos3) & 1) == 0) // found non-escaped semicolon, that possibly needs appending LF
            {
                pos3 = s.find_first_not_of(" \t\r\n", pos2 + 1);
                pos4 = s.find_first_of("\r\n", pos2 + 1);
                b_lf = pos3 != end && s[pos3] == '=' && !(pos4 > pos2 && pos4 < pos3);
            }

            s2 += s.substr(pos, pos2 + 1 - pos);
            if (b_lf) { s2 += '\n'; }
            pos = pos2 + 1;
        }
        return s2;
    }
    inline std::string _argv2pltree(const char* arg1) { return _argv2pltree(&arg1, 0, 1); }

}

#endif
