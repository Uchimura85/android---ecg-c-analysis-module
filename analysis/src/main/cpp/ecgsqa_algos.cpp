#include "ecgsqa_algos.h"
#include "common.h"
#include "matrix/MyArr.h"

//#include <iostream>
//using namespace std;

namespace ecgsqa {
    string beat_info::plstr() const {
        string s;
        if (empty()) { return s; }

        if (flags) {
            ecg_data::append_text(s, "pqrst=");
            for (int i = 0; i < nart; ++i) {
                int b = (flags >> i) & 1;
                s += "|";
                s += ntocs(b);
                s += "|";
                s += b && inds[i] ? ftocs(inds[i]) : "0";
                s += "|";
                s += b && f[i] ? ftocs(f[i], 8) : "0";
            }
        }

        if (hr) { ecg_data::append_text(s, "HR=" + ftocs(hr, 10)); }
        if (Lp) { ecg_data::append_text(s, "Lp=" + ftocs(Lp, 8)); }
        if (Lt) { ecg_data::append_text(s, "Lt=" + ftocs(Lt, 8)); }

        return s;
    }

    int beat_info::pl_set(const unity &anns) {
        const unity *uall = anns.path("R|beats_eval");
        if (!uall) { return -1; }

        clear();

        const unity *ubeat = uall->path("pqrst");
        if (ubeat && ubeat->isArray() && ubeat->arrsz() >= 3 * nart) {
            vec2_t<double> a;
            try { a = ubeat->val<utFloatArray>(); } catch (...) {
                clear();
                return 0;
            }
            a.vec2_set_nbase(0);
            flags = 0;
            for (int i = 0; i < nart; ++i) {
                int j = i * 3;
                if (a[j]) {
                    flags |= 1 << i;
                    inds[i] = float(a[j + 1]);
                    f[i] = ecgfvsmp(a[j + 2]);
                }
                else {
                    inds[i] = 0;
                    f[i] = 0;
                }
            }
        }

        const unity *u;
        if (1) {
            u = uall->path("HR");
            if (u && !u->isEmpty()) {
                try { hr = u->vfp(); } catch (...) {
                    clear();
                    return 0;
                }
            }
        }
        if (1) {
            u = uall->path("Lp");
            if (u && !u->isEmpty()) {
                try { Lp = u->vfp(); } catch (...) {
                    clear();
                    return 0;
                }
            }
        }
        if (1) {
            u = uall->path("Lt");
            if (u && !u->isEmpty()) {
                try { Lt = u->vfp(); } catch (...) {
                    clear();
                    return 0;
                }
            }
        }

        return 1;
    }

    string sleep_info::plstr() const {
        string s;
        if (empty()) { return s; }

        const char *stg[7] = {"_?", "_W", "_R", "_1", "_2", "_3", "_4"};
        int q = istage + 2;
        if (q < 0) { q = 0; }
        if (q >= 7) { q = 0; }
        ecg_data::append_text(s, stg[q]);

        if (mt) { ecg_data::append_text(s, "_MT"); }

        if (nw) { ecg_data::append_text(s, "nw=" + ntocs(nw)); }

        if (addann.size()) { ecg_data::append_text(s, "addann=|" + join(addann, "|")); }

        return s;
    }

    int sleep_info::pl_set(const unity &anns) {
        const unity *uall = anns.path("Slpst");
        if (!uall) { return -1; }

        clear();

        if (uall->path("_W")) { istage = -1; }
        else if (uall->path("_1")) { istage = 1; }
        else if (uall->path("_2")) { istage = 2; }
        else if (uall->path("_3")) { istage = 3; }
        else if (uall->path("_4")) { istage = 4; }
        else if (uall->path("_R")) { istage = 0; }
        else if (uall->path("_?")) { istage = -2; }

        if (uall->path("_MT")) { mt = true; }

        const unity *aa = uall->path("addann");
        if (aa) {
            try {
                if (aa->isArray()) {
                    for (s_long i = aa->arrlb(); i <= aa->arrub(); ++i) {
                        addann.push_back(aa->vcstr(i));
                    }
                }
            } catch (...) {
                clear();
                return 0;
            }
        }

        const unity *u;
        if (1) {
            u = uall->path("nw");
            if (u && !u->isEmpty()) {
                try { nw = ecgismp(u->vint()); } catch (...) {
                    clear();
                    return 0;
                }
            }
        }

        return 1;
    }

    ecgismp ecg_data::nmax(int sec) const {
        ecgismp n = 0;
        if (sec & db_ECG) {
            ecgismp n2 = ecgismp(ecg.size());
            if (n2 > n) { n = n2; }
        }
        if (sec & db_Resp) {
            ecgismp n2 = ecgismp(resp.size());
            if (n2 > n) { n = n2; }
        }
        if (sec & db_EEG) {
            ecgismp n2 = ecgismp(eeg.size());
            if (n2 > n) { n = n2; }
        }
        if (sec & db_BP) {
            ecgismp n2 = ecgismp(bp.size());
            if (n2 > n) { n = n2; }
        }
        if (sec & (dba_beats | dba_beats_eval)) {
            ecgismp n2 = beats.n() ? beats(beats.n() - 1)->k : 0;
            if (n2 > n) { n = n2; }
        }
        if (sec & dba_slpst) {
            ecgismp n2 = slpst.n() ? slpst(slpst.n() - 1)->k : 0;
            if (n2 > n) { n = n2; }
        }
        return n;
    }

    void ecg_data::clear(int sec) {
        if (sec & db_ECG) { ecg.clear(); }
        if (sec & db_Resp) { resp.clear(); }
        if (sec & db_EEG) { eeg.clear(); }
        if (sec & db_BP) { bp.clear(); }
        if (sec & dba_beats) { beats.ordhs_clear(); }
        if (sec & dba_beats_eval) { for (s_long i = 0; i < beats.n(); ++i) { beats(i)->v.clear(); }}
        if (sec & dba_slpst) { slpst.ordhs_clear(); }
    }

    bool ecg_data::save(const string &fnp, int sec, bool b_append) {
        ecgismp n = nmax(sec);
        int ncols = 0;
        int iecg = (sec & db_ECG) ? ncols++ : -1;
        int iresp = (sec & db_Resp) ? ncols++ : -1;
        int ieeg = (sec & db_EEG) ? ncols++ : -1;
        int ibp = (sec & db_BP) ? ncols++ : -1;
        string saccu, sdat, sann;
        for (ecgismp i = 0; i < n; ++i) {
            sdat.clear();
            sann.clear();
            if (i == 0) {
                sdat += "0_" + ntocs(ncols);
                append_text(sann, "fd=" + ftocs(fd, 12));
                if (ncols > 0) {
                    append_text(sann, string("columns=") + (iecg >= 0 ? "|ECG" : "") +
                                      (iresp >= 0 ? "|Resp" : "") + (ieeg >= 0 ? "|EEG" : "") +
                                      (ibp >= 0 ? "|BP" : ""));
                }
                if (ncols > 0) {
                    append_text(sann, string("mu=") + (iecg >= 0 ? "|mV" : "") +
                                      (iresp >= 0 ? "|L" : "") + (ieeg >= 0 ? "|mV" : "") +
                                      (ibp >= 0 ? "|mmHg" : ""));
                }
            } else if (i % 100 == 0) {
                sdat += ntocs(i / 100);
            }

            if (iecg >= 0) {
                sdat += '\t';
                if (i < ecgismp(ecg.size())) { sdat += ftocs(ecg[i], 8); }
            }
            if (iresp >= 0) {
                sdat += '\t';
                if (i < ecgismp(resp.size())) { sdat += ftocs(resp[i], 8); }
            }
            if (ieeg >= 0) {
                sdat += '\t';
                if (i < ecgismp(eeg.size())) { sdat += ftocs(eeg[i], 8); }
            }
            if (ibp >= 0) {
                sdat += '\t';
                if (i < ecgismp(bp.size())) { sdat += ftocs(bp[i], 8); }
            }

            if (sec & (dba_beats | dba_beats_eval)) {
                const ordhs_t<ecgismp, beat_info>::entry *e1 = beats.find(i);
                if (e1) {
                    append_text(sann, "=|R");
                    if (sec & dba_beats_eval) {
                        append_text(sann, "=|R|beats_eval");
                        append_text(sann, e1->v.plstr());
                    }
                }
            }

            if (sec & dba_slpst) {
                const ordhs_t<ecgismp, sleep_info>::entry *e1 = slpst.find(i);
                if (e1) {
                    append_text(sann, "=|Slpst");
                    append_text(sann, e1->v.plstr());
                }
            }

            saccu += sdat;
            if (!sann.empty()) {
                saccu += '\t';
                saccu += sann;
            }
            saccu += "\r\n";
        }
        return file_io::save_bytes(fnp, saccu, b_append) > 0;
    }

    bool ecg_data::load(const string &fnp, int sec) {
        string sdata;
        if (file_io::load_bytes(fnp, sdata) <= 0) { return false; }

        this->clear(sec);
        size_t pos = 0, pos2, pos3, pc1;
        const size_t end = string::npos;
        int ncols = 0;
        vector<basic_string<ecgfvsmp> *> cols;
        ecgismp ismp = 0;
        while (pos < sdata.size()) {
            pos3 = pos2 = sdata.find('\n', pos);
            if (pos2 == end) { pos2 = sdata.size(); }
            if (pos2 > pos && sdata[pos2 - 1] == '\r') { --pos3; }
            string s = sdata.substr(pos, pos3 - pos);

            unity anns;
            bool b_anndec = false;

            pc1 = s.find('\t');
            if (pc1 == end) { pc1 = s.length(); }

            if (s.length() >= 2 && s[0] == '0' && s[1] == '_') // new data block
            {
                cols.clear();
                ncols = str2i(s.substr(2, pc1 - 2), -1, true);
                if (ncols < 0) {
                    this->clear(sec);
                    return false;
                }
                cols.resize(ncols, 0);

                for (int i = 0; i < ncols; ++i) {
                    pc1 = s.find('\t', pc1 + 1);
                    if (pc1 == end) {
                        pc1 = s.length();
                        break;
                    }
                }
                ++pc1;
                if (pc1 < s.length()) {
                    anns = paramline().decode_tree(bsToWs(s.substr(pc1)));
                    b_anndec = true;
                    const unity *u;
                    if (1) {
                        u = anns.path("fd");
                        if (u && !u->isEmpty()) {
                            try { fd = u->vfp(); } catch (...) {
                                this->clear(sec);
                                return false;
                            }
                        }

                        u = anns.path("columns");
                        if (u && u->isArray()) {
                            for (s_long i = 1; i <= u->arrsz(); ++i) {
                                string k = u->vcstr(i);
                                if (sec & db_ECG) { if (k == "ECG") { cols[i - 1] = &ecg; }}
                                else if (sec & db_Resp) { if (k == "Resp") { cols[i - 1] = &resp; }}
                                else if (sec & db_EEG) { if (k == "EEG") { cols[i - 1] = &eeg; }}
                                else if (sec & db_BP) { if (k == "BP") { cols[i - 1] = &bp; }}
                            }
                        }
                    }
                }
            }

            if (sec & db_ECG) { ecg.resize(ismp + 1, 0); }
            if (sec & db_Resp) { resp.resize(ismp + 1, 0); }
            if (sec & db_EEG) { eeg.resize(ismp + 1, 0); }
            if (sec & db_BP) { bp.resize(ismp + 1, 0); }

            for (int i = 0; i < ncols; ++i) {
                ++pc1;
                if (pc1 >= s.length()) { break; }
                size_t pc2 = s.find('\t', pc1);
                if (pc2 == end) { pc2 = s.length(); }

                if (cols[i]) { (*cols[i])[ismp] = strn2f(&s[pc1], s_ll(pc2) - s_ll(pc1), 0, true); }

                pc1 = pc2;
            }

            if (!b_anndec) {
                ++pc1;
                if (pc1 < s.length()) { anns = paramline().decode_tree(bsToWs(s.substr(pc1))); }
                b_anndec = true;
            }

            if (!anns.isEmpty()) {
                if (!!(sec & (dba_beats | dba_beats_eval)) && anns.path("R")) {
                    beat_info &bi = beats[ismp];
                    bi.clear();
                    if (sec & dba_beats_eval) { bi.pl_set(anns); }
                }
                if (!!(sec & dba_slpst) && anns.path("Slpst")) {
                    sleep_info &si = slpst[ismp];
                    si.clear();
                    si.pl_set(anns);
                }
            }

            ismp += 1;
            pos = pos2 + 1;
        }
        return true;
    }

    bool ecg_data::loadFromEcgFile(const string &fnp, int sec) {
        string sdata;
        if (file_io::load_bytes(fnp, sdata) <= 0) { return false; }

        this->clear(sec);
        size_t pos = 0, pos2, pos3, pc1;
        const size_t end = string::npos;
        int ncols = 0;
        vector<basic_string<ecgfvsmp> *> cols;
        ecgismp ismp = 0;
        while (pos < sdata.size()) {
            pos3 = pos2 = sdata.find('\n', pos);
            if (pos2 == end) { pos2 = sdata.size(); }
            if (pos2 > pos && sdata[pos2 - 1] == '\r') { --pos3; }
            string s = sdata.substr(pos, pos3 - pos);

            unity anns;
            bool b_anndec = false;

            pc1 = s.find('\t');
            if (pc1 == end) { pc1 = s.length(); }

            if (s.length() >= 2 && s[0] == '0' && s[1] == '_') // new data block
            {
                cols.clear();
                ncols = str2i(s.substr(2, pc1 - 2), -1, true);
                if (ncols < 0) {
                    this->clear(sec);
                    return false;
                }
                cols.resize(ncols, 0);

                for (int i = 0; i < ncols; ++i) {
                    pc1 = s.find('\t', pc1 + 1);
                    if (pc1 == end) {
                        pc1 = s.length();
                        break;
                    }
                }
                ++pc1;
                if (pc1 < s.length()) {
                    anns = paramline().decode_tree(bsToWs(s.substr(pc1)));
                    b_anndec = true;
                    const unity *u;
                    if (1) {
                        u = anns.path("fd");
                        if (u && !u->isEmpty()) {
                            try { fd = u->vfp(); } catch (...) {
                                this->clear(sec);
                                return false;
                            }
                        }

                        u = anns.path("columns");
                        if (u && u->isArray()) {
                            for (s_long i = 1; i <= u->arrsz(); ++i) {
                                string k = u->vcstr(i);
                                if (sec & db_ECG) { if (k == "ECG") { cols[i - 1] = &ecg; }}
                                else if (sec & db_Resp) { if (k == "Resp") { cols[i - 1] = &resp; }}
                                else if (sec & db_EEG) { if (k == "EEG") { cols[i - 1] = &eeg; }}
                                else if (sec & db_BP) { if (k == "BP") { cols[i - 1] = &bp; }}
                            }
                        }
                    }
                }
            }

            if (sec & db_ECG) { ecg.resize(ismp + 1, 0); }
            if (sec & db_Resp) { resp.resize(ismp + 1, 0); }
            if (sec & db_EEG) { eeg.resize(ismp + 1, 0); }
            if (sec & db_BP) { bp.resize(ismp + 1, 0); }

            for (int i = 0; i < ncols; ++i) {
                ++pc1;
                if (pc1 >= s.length()) { break; }
                size_t pc2 = s.find('\t', pc1);
                if (pc2 == end) { pc2 = s.length(); }

                if (cols[i]) { (*cols[i])[ismp] = strn2f(&s[pc1], s_ll(pc2) - s_ll(pc1), 0, true); }

                pc1 = pc2;
            }

            if (!b_anndec) {
                ++pc1;
                if (pc1 < s.length()) { anns = paramline().decode_tree(bsToWs(s.substr(pc1))); }
                b_anndec = true;
            }

            if (!anns.isEmpty()) {
                if (!!(sec & (dba_beats | dba_beats_eval)) && anns.path("R")) {
                    beat_info &bi = beats[ismp];
                    bi.clear();
                    if (sec & dba_beats_eval) { bi.pl_set(anns); }
                }
                if (!!(sec & dba_slpst) && anns.path("Slpst")) {
                    sleep_info &si = slpst[ismp];
                    si.clear();
                    si.pl_set(anns);
                }
            }

            ismp += 1;
            pos = pos2 + 1;
        }
        return true;
    }

    void ecg_data::trim(ecgismp i_start, ecgismp i_end) {
        size_t i0, i2, n;
        const size_t n2 = mylmax(ecg.size(), resp.size());

        n = ecg.size();
        i0 = size_t(i_start);
        if (i_start < 0) { i0 = 0; }
        if (i0 > n) { i0 = n; }
        i2 = size_t(i_end);
        if (i_end < 0) { i2 = n; }
        if (i2 > n) { i2 = n; }
        if (i2 > i0) {
            n = i2 - i0;
            if (i0 > 0) { memmove(&ecg[0], &ecg[i0], n * sizeof(ecgfvsmp)); }
            ecg.resize(n);
        } else { ecg.clear(); }

        n = resp.size();
        i0 = size_t(i_start);
        if (i_start < 0) { i0 = 0; }
        if (i0 > n) { i0 = n; }
        i2 = size_t(i_end);
        if (i_end < 0) { i2 = n; }
        if (i2 > n) { i2 = n; }
        if (i2 > i0) {
            n = i2 - i0;
            if (i0 > 0) { memmove(&resp[0], &resp[i0], n * sizeof(ecgfvsmp)); }
            resp.resize(n);
        } else { resp.clear(); }

        i0 = size_t(i_start);
        if (i_start < 0) { i0 = 0; }
        i2 = size_t(i_end);
        if (i_end < 0) { i2 = n2; }
        if (i2 > i0) {
            while (beats.n() > 0) {
                ecgismp k = beats(0)->k;
                if (k >= ecgismp(i0)) { break; }
                if (beats.remove(k) != 1) { throw exc_ecg1(); }
            }
            while (slpst.n() > 0) {
                ecgismp k = slpst(0)->k;
                if (k >= ecgismp(i0)) { break; }
                if (slpst.remove(k) != 1) { throw exc_ecg1(); }
            }

            s_long ind;
            beats.find(i2, &ind);
            if (ind >= 0) {
                while (ind < beats.n()) {
                    if (beats.remove(beats(ind)->k) != 1) { throw exc_ecg1(); }
                }
            }
            slpst.find(i2, &ind);
            if (ind >= 0) {
                while (ind < slpst.n()) {
                    if (slpst.remove(slpst(ind)->k) != 1) { throw exc_ecg1(); }
                }
            }

            shift_keys(beats, -ecgismp(i0));
            shift_keys(slpst, -ecgismp(i0));
        } else {
            beats.ordhs_clear();
            slpst.ordhs_clear();
        }
    }

    ecgismp resampler::interp_gidest_min(bool b_max, ecgismp dflt) const {
        if (!b_resample && !b_max) { return gi0src; }
        if (b_down) {
            ecgismp i = gidest_ge(gi0src, dflt);
            if (i == dflt) { return dflt; }
            if (gisrc_le(i) < gi0src) { i += 1; }
            return i + 1;
        }
        else {
            ecgismp i = gidest_ge(gi0src + 1, dflt);
            if (i == dflt) { return dflt; }
            if (gisrc_le(i) - 1 < gi0src) { i += 1; }
            return i;
        }
    }

    int resampler::v_interp(ecgismp gidest, ecgfvsmp *ret_pv, double *ret_pts, bool b_max,
                            bool b_strict) const {
        if (!pecg || pecg->empty()) { return 0; }
        const basic_string<ecgfvsmp> &re = *pecg;
        if (!b_resample && !b_max) {
            ecgismp i = gidest - gi0src;
            if (i < 0) { return -1; }
            if (size_t(i) >= re.length()) { return -2; }
            if (ret_pv) { *ret_pv = re[i]; }
            if (ret_pts) { *ret_pts = ts_dest(gidest); } // tsdest, tssrc make same value here
            return 2;
        }
        ecgismp gi2 = gisrc_le(gidest);
        ecgismp i2 = gi2 - gi0src;
        ecgismp gi1 = b_down ? gisrc_le(gidest - 1) : gi2 - 1;
        ecgismp i1 = gi1 - gi0src;
        ecgismp gi3 = b_down ? gisrc_le(gidest + 1) : gi2 + 1;
        ecgismp i3 = gi3 - gi0src;
        ecgismp gi4 = b_down ? gisrc_le(gidest + 2) : gi2 + 2;
        ecgismp i4 = gi4 - gi0src;
        if (i1 < 0) { return -1; }
        if (size_t(i4) >= re.length()) { return -2; }

        polynome3 p;
        p.interpolate(gi1 / fsrc, gi2 / fsrc, gi3 / fsrc, gi4 / fsrc, re[i1], re[i2], re[i3],
                      re[i4]);
        if (!p.b_valid) {
            if (b_strict) { return -3; }
            if (ret_pv) { *ret_pv = re[i2]; }
            if (ret_pts) { *ret_pts = ts_dest(gidest); }
            return 1;
        }
        double t = gidest / fdest;
        double f = p.f(t);
        if (b_max) {
            // Extremums are where 1st derivative == 0:
            // 3a x^2 + 2b x + c = 0
            // det = 4b^2 - 12ac
            // x1, x2 = (-2b +/- sqrt(det)) / 6a
            double d = 4 * p.b * p.b - 12 * p.a * p.c;
            if (d >= 0) {
                double tleft = (gidest - 1) / fdest, tright = (gidest + 1) / fdest;
                d = sqrt(d);
                double q1 = -2 * p.b;
                double q2 = 6 * p.a;
                double t1 = (q1 - d) / q2;
                double t2 = (q1 + d) / q2;
                if (t1 > tleft && t1 < tright) {
                    double fm = p.f(t1);
                    if (fm > f) {
                        f = fm;
                        t = t1;
                    }
                }
                if (t2 > tleft && t2 < tright) {
                    double fm = p.f(t2);
                    if (fm > f) {
                        f = fm;
                        t = t2;
                    }
                }
            }
        }
        if (ret_pv) { *ret_pv = ecgfvsmp(f); }
        if (ret_pts) { *ret_pts = t; }
        return 1;
    }

    ecgismp resampler::v_max_abs_ge(ecgismp gi1, ecgismp gi2, ecgismp dflt) {
        if (!pecg) { return dflt; }
        const basic_string<ecgfvsmp> &re = *pecg;
        ecgismp i1 = gi1 - gi0src;
        if (i1 < 0) { i1 = 0; }
        ecgismp i2 = gi2 - gi0src;
        if (i2 < 0) { i2 = 0; } else { if (size_t(i2) >= re.size()) { i2 = ecgismp(re.size()); }}
        if (i2 <= i1) { return dflt; }
        ecgfvsmp amax = fabs(re[i1]);
        ecgismp imax = i1;
        while (++i1 < i2) {
            ecgfvsmp a = fabs(re[i1]);
            if (a > amax) {
                amax = a;
                imax = i1;
            }
        }
        return imax + gi0src;
    }


    alg_sqrs::_sqrsdata::_sqrsdata() {
        time = 0;
        sign = 0;
        qtime = 0;
        maxtime = 0;
        for (int i = 0; i < 10; ++i) { f[i] = 0; } // formal init.
        maxslope = 0;
        nslope = 0;
        ms160 = floor(fdest() * 0.16);
        ms200 = floor(fdest() * 0.2);
        ms2000 = floor(fdest() * 2);
        slopecrit = scmax = 10;
        scmin = 1; // all in mV
        b_f = false;
    }

    alg_sqrs::_sqrsretval alg_sqrs::_sqrsdata::sqrs_step(ecgismp gi_, ecgfvsmp f_) {
        time = gi_;
        if (!b_f) {
            b_f = true;
            for (int i = 0; i < 9; ++i) { f[i] = f_; }
        }
        memmove(&f[1], &f[0], sizeof(ecgfvsmp) * 9);
        f[0] = f_;

        _sqrsretval retv;

        ecgfvsmp filter =
                f[0] + 4 * f[1] + 6 * f[2] + 4 * f[3] + f[4] - f[5] - 4 * f[6] - 6 * f[7] -
                4 * f[8] - f[9];

        if (time % ms2000 == 0) // each 2 s
        {
            // Recalculate threshold.
            if (nslope == 0) {
                slopecrit *= ecgfvsmp(15) / 16;
                if (slopecrit < scmin) { slopecrit = scmin; }
            }
            else if (nslope > 4) {
                slopecrit *= ecgfvsmp(17) / 16;
                if (slopecrit > scmax) { slopecrit = scmax; }
            }
        }
        if (nslope == 0 && fabs(filter) > slopecrit) // first threshold crossing after filter reset
        {
            nslope = 1;
            maxtime = ms160;
            sign = (filter > 0) ? 1 : -1;
            qtime = time; // potential QRS start time
        }
        if (nslope != 0) {
            filter *= sign;
            if (filter < -slopecrit) {
                sign = -sign;
                maxtime = (++nslope > 4) ? ms200
                                         : ms160; // after each threshold crossing, the current QRS time window is prolonged
            } else if (filter > slopecrit && fabs(filter) > maxslope) { maxslope = fabs(filter); }
            if (maxtime-- < 0) {
                if (2 <= nslope && nslope <= 4) {
                    retv = _sqrsretval(true, false, qtime - 3);

                    // Every time a normal beat is detected, the threshold is recalculated,
                    //  asymptotically converging to 1/4 of the maximum filter output obtained so far.
                    slopecrit += (maxslope / 4 - slopecrit) / 8;
                    if (slopecrit < scmin) { slopecrit = scmin; }
                    else if (slopecrit > scmax) { slopecrit = scmax; }
                    time = 0;
                } else if (nslope >= 5) // "isolated QRS-like artifact"
                { retv = _sqrsretval(false, true, qtime - 3); }
                nslope = 0;
            }
        }
        return retv;
    }

    ecgismp alg_sqrs::min_gisrc() const {
        ecgismp gi1 = gisrc();
        if (_d.nslope > 0) {
            ecgismp i = _d.qtime - 2;
            i = r1.b_down ? r1.gisrc_le(i - 1) : r1.gisrc_le(i) - 1;
            if (i < gi1) { gi1 = i; }
        }
        if (gi1 < r1.gi0src) { gi1 = r1.gi0src; }
        return gi1;
    }

    int alg_sqrs::run(bool b_final) {
        if (_state != 1) { return -3; }
        bool b1 = false;
        while (true) {
            // Get data sample from sequence.
            ecgfvsmp f; // the current sample value.
            int res = r1.v_interp(_gidest, &f);

            if (res == -1) {
                if (b_final) { _state = 2; }
                return -1;
            }
            if (res == 0 || res == -2) {
                if (b_final) { _state = 2; }
                return b1 ? 1 : 0;
            }
            if (res < 0) { throw exc_ecg1(); } // integrity
            b1 = true;

            // SQRS.
            _sqrsretval pk = _d.sqrs_step(_gidest, f);

            // Save peak pos. and value.
            if (pk.b_R()) {
                ecgismp isrc = r1.gisrc_le(pk.gi) - r1.gi0src;
                pbeats->insert(isrc);
            }

            ++_gidest;
        }
    }


    alg_sqrs_nc::_sqrsdata::_sqrsdata() {
        time = 0;
        sign = 0;
        qtime = 0;
        maxtime = 0;
        for (int i = 0; i < 10; ++i) { f[i] = 0; } // formal init.
        maxslope = 0;
        nslope = 0;
        ms160 = floor(fdest() * 0.16);
        ms200 = floor(fdest() * 0.2);
        ms2000 = floor(fdest() * 2);
        slopecrit = scmax = 10;
        scmin = 1; // all in mV
        b_f = false;
        b_nslope_a = false;
    }


    alg_sqrs_nc::_sqrsretval alg_sqrs_nc::_sqrsdata::sqrs_step(ecgismp gi_, ecgfvsmp f_) {
        time = gi_;
        if (!b_f) {
            b_f = true;
            for (int i = 0; i < 9; ++i) { f[i] = f_; }
        }
        memmove(&f[1], &f[0], sizeof(ecgfvsmp) * 9);
        f[0] = f_;

        _sqrsretval retv;

        ecgfvsmp filter =
                f[0] + 4 * f[1] + 6 * f[2] + 4 * f[3] + f[4] - f[5] - 4 * f[6] - 6 * f[7] -
                4 * f[8] - f[9];

        if (time % ms2000 == 0) // each 2 s
        {
            // Recalculate limits.
            if (maxslope > 4 * scmax) {
                maxslope = 0.8 * maxslope + 0.8 * scmax;
            } // converges down to 4*scmax (against very strong interference peaks)
            if (nslope == 0) {
                slopecrit *= ecgfvsmp(15) / 16;
                if (slopecrit < scmin) { slopecrit = scmin; }
            }
            else if (nslope > 4) {
                slopecrit *= ecgfvsmp(17) / 16;
                if (slopecrit > scmax) { slopecrit = scmax; }
            }
        }
        if (nslope == 0 && (filter > slopecrit || filter < -1.5 *
                                                           slopecrit)) // first threshold crossing after filter reset (negative threshold is larger against noise)
        {
            nslope = 1;
            b_nslope_a = true;
            maxtime = ms160;
            sign = (filter > 0) ? 1 : -1;
            qtime = time; // potential QRS start time
        }
        if (nslope != 0) {
            filter *= sign;
            if (filter < -slopecrit) {
                sign = -sign;
                maxtime = (++nslope > 4) ? ms200
                                         : ms160; // after each threshold crossing, the current QRS time window is prolonged
            }
            if (fabs(filter) > maxslope && nslope <= 4) {
                maxslope = fabs(filter);
            } // slope limit is updated only on expected slopes

            --maxtime;
            if (nslope >= 2 && nslope <= 4) {
                if (maxtime < 0) {
                    retv = _sqrsretval(true, false, qtime - 2);

                    // Recalculate threshold at each normal beat.
                    slopecrit = 0.9 * slopecrit + 0.02 * maxslope; // converges to 0.2*maxslope
                    if (slopecrit < scmin) { slopecrit = scmin; }
                    else if (slopecrit > scmax) { slopecrit = scmax; }
                    time = 0;
                    nslope = 0;
                    b_nslope_a = false;
                }
            } else if (nslope >= 5) // "isolated QRS-like artifact" (too many adjacent peaks)
            {

                if (b_nslope_a) {
                    retv = _sqrsretval(false, true, qtime - 2);
                    b_nslope_a = false;
                }
                if (maxtime < 0) { nslope = 0; }
            } else // nslope == 1
            {
                if (maxtime < 0) {
                    nslope = 0;
                    b_nslope_a = false;
                }
            } // if timeout: the impulse is too long, or DC offset jump occured
        }
        return retv;
    }

    ecgfvsmp alg_sqrs_nc::_nc_control::range::get_approx(ecgismp i, ecgfvsmp xfact) const {
        if (is_empty()) { return xfact; }
        ecgismp i_linear = i2 - (cnahead - 2);
        if (i < i_linear) { return f1; }
        return f1 + (i - i_linear) * (f2 - f1) / (i2 - i_linear);
    }

    bool alg_sqrs_nc::_nc_control::feed(ecgismp gidest, ecgfvsmp x) {
        if (gidest != nd.ifeed()) { return false; }
        nd.feed(x);
        qfeed.push_back(x);
        if (qfeed.size() > size_t(cnintsl + 2)) {
            qfeed.erase(qfeed.begin(), qfeed.end() - (cnintsl + 2));
        }
        _reduce_nwdata();
        bool b_nw2 = nd.Lw() > 0;
        if (b_nw2 == b_nw) { return true; }
        if (b_nw2) {
            // New noise window, at slope beginning -- still ahead of SQRS,
            //   because cnintsl < cnahead by at least 6 (n smp. cubic interpolation + n smp. noise detector lag).
            ecgismp i = ecgismp(qfeed.size()) - (nd.ifeed() - nd.islope());
            if (i < 0) { i = 0; }
            ecg_noise[nd.islope()] = nwinfo(1, qfeed[i]);
            b_nw = true;
        } else {
            // End of noise window (at the current position).
            ecg_noise[gidest] = nwinfo(0, x);
            b_nw = false;
        }
        return true;
    }

    void alg_sqrs_nc::_nc_control::_reduce_nwdata() {
        if (ecg_noise.n() == 0) { return; }
        ecgismp i0rem = nd.ifeed() - cnahead - ecgismp(0.7 * 250);
        s_long ind = -1;
        ecg_noise.find(i0rem, &ind);

        if (ind <= 0) { return; }

        const ordhs_t<ecgismp, nwinfo>::entry *e, *e0;
        if (ind < ecg_noise.n()) {
            e = ecg_noise(ind);
            e0 = ecg_noise(ind - 1);
            nwinfo w = e0->v;
            bool b_prev = w.pttype > 0 && i0rem < e->k;
            for (s_long j = 0; j < ind; ++j) { ecg_noise.remove(ecg_noise(0)->k); }
            if (b_prev) {
                w.pttype = 2;
                ecg_noise[i0rem] = w;
            }
            return;
        } else {
            e0 = ecg_noise(ind - 1);
            nwinfo w = e0->v;
            if (w.pttype > 0) {
                ecg_noise.ordhs_clear();
                w.pttype = 2;
                ecg_noise[i0rem] = w;
                return;
            }
            ecg_noise.ordhs_clear();
            return;
        }
    }

    alg_sqrs_nc::_nc_control::range alg_sqrs_nc::_nc_control::get_nw(ecgismp gidest) const {
        range r;
        if (ecg_noise.n() == 0) { return r; }
        s_long ind = -1;
        ecg_noise.find(gidest, &ind);
        if (ind < 0) { return r; }

        const ordhs_t<ecgismp, nwinfo>::entry *e, *e0, *e2;
        if (ind == ecg_noise.n()) {
            e = ecg_noise(ind - 1);
            if (e->v.pttype == 0) { return r; }
        } else {
            e = ecg_noise(ind);
            if (gidest < e->k) {
                if (ind == 0) { return r; }
                e0 = ecg_noise(ind - 1);
                if (e0->v.pttype == 0) { return r; }
                r.i1 = e0->k;
                r.i2 = e->k;
                r.f1 = e0->v.f;
                r.f2 = e->v.f;
                return r;
            }
            if (e->v.pttype == 0) { return r; } // normal signal
            if (ind + 1 < ecg_noise.n()) // noise window is known in full already
            {
                e2 = ecg_noise(ind + 1);
                r.i1 = e->k;
                r.i2 = e2->k;
                r.f1 = e->v.f;
                r.f2 = e2->v.f;
                return r;
            }
        }
        // NOTE approximate() / range::get_approx() won't use r.i2 and r.f2 if the window is not completed.
        r.i1 = e->k;
        r.i2 = nd.ifeed();
        r.f1 = e->v.f;
        r.f2 = e->v.f;
        return r;
    }

    ecgismp alg_sqrs_nc::min_gisrc() const {
        ecgismp gi = gisrc() - r1.gisrc_le_f(0.7) - 1;
        if (gi < r1.gi0src) { gi = r1.gi0src; }
        return gi;
    }

    int alg_sqrs_nc::run_m(bool b_final) {
        if (_state != 1) { return -3; }
        bool b1 = false;
        while (true) {
            // Get data sample from sequence.
            ecgfvsmp f; // the current sample value.
            int res = r1.v_interp(_gidest, &f);

            if (res == -1) {
                if (b_final) { _state = 2; }
                return -1;
            }
            if (res == 0 || res == -2) {
                if (b_final) { _state = 2; }
                return b1 ? 1 : 0;
            }
            if (res < 0) { throw exc_ecg1(); } // integrity
            b1 = true;

            // SQRS.
            _sqrsretval pk = _d.sqrs_step(_gidest, f);

            // Save peak pos. and value.
            if (pk.b_R()) {
                ecgismp gi = r1.gisrc_le(pk.gi);
                ecgismp isrc = r1.v_max_abs_ge(gi, gi + r1.gisrc_ge_f(0.03), gi) - r1.gi0src;
                pbeats->insert(isrc);
            }

            ++_gidest;
        }
    }

    ecgismp g_beforeRPeak = -1L;

    // run() has all features 1..4. See near struct alg_sqrs_nc.
    int alg_sqrs_nc::run(bool b_final) {
        if (_state != 1) { return -3; }
        if (_gidest != _nc.nd.ifeed()) { return -3; }
//std::ostringstream st; long iprev = 0;
        bool b1 = false;
        while (true) {
            // Get data sample from sequence.
            ecgfvsmp f; // the current sample value.
            int res = r1.v_interp(_gidest, &f);
//if (res <= 0) { file_io::save_bytes("__trace.txt", st.str(), false); }
            if (res == -1) {
                if (b_final) { _state = 2; }
                return -1;
            }
            if (res == 0 || res == -2) {
                if (b_final) { _state = 2; }
                return b1 ? 1 : 0;
            }
            if (res < 0) { throw exc_ecg1(); } // integrity
            b1 = true;

            // Feed noise detector.
            _nc.feed(_gidest, f);
            ++_gidest;

            // Remember the current value for later passing into SQRS.
            _ncq.push(f);

            // Process the queued value if already available.
            if (_ncq.size() > size_t(_nc.ndest_ahead())) {
                ecgismp gi_sqrs = _gidest - _nc.ndest_ahead() - 1;
                f = _ncq.front();
                _ncq.pop();
                ecgfvsmp f2 = _nc.approximate(gi_sqrs, f);

//if (f2 != f) { _nc_control::range r = _nc.get_nw(gi_sqrs); st << gi_sqrs << "\t" << (gi_sqrs - iprev) << "\t" << r.i1 << "\t" << r.i2 << "\t"  << r.f1 << "\t" << r.f2 << "\t" << f << "\t" << f2 << endl; iprev = gi_sqrs; }
                // SQRS.
                _sqrsretval pk = _d.sqrs_step(gi_sqrs, f2);
                // Save peak pos. and value.
                if (pk.b_R()) {
                    if (g_beforeRPeak == -1) {
                        g_beforeRPeak = pk.gi;
                    }else{}

                    double rriDistance = (pk.gi - g_beforeRPeak) / r1.fdest;
                    g_beforeRPeak = pk.gi;
                    if(rriDistance > 0)
                        arr_RRI.push_back(rriDistance);

                    ecgismp gi = r1.gisrc_le(pk.gi);
                    ecgismp isrc = r1.v_max_abs_ge(gi, gi + r1.gisrc_ge_f(0.03), gi) - r1.gi0src;
                    pbeats->insert(isrc);
                }
            }
        }//whie
        g_beforeRPeak = -1; // init global index(before packet)
    }

    int alg_pqrst::estimate1(beat_info *pret, const ordhs_t<ecgismp, beat_info> *pbeats,
                             bool b_calc_pqst, bool b_streaming) {
        if (pret) { pret->clear(); }

        // === === 1. FILTERING === ===

        ecg_data &re = *_pecg;
        const ordhs_t<ecgismp, beat_info> &rbb = pbeats ? *pbeats : re.beats;
        _w.clear();
        _w2.clear();
        _wgi0 = _gi;
        _ir = -1;
        _indb = -1;

        if (true) // calc. _w, _wgi0 (pos. of window beginning), _ir (rel. R pos), next _gi
        {
            ecgismp i = _gi - r1.gi0src;
            s_long ind = -1;
            rbb.find(i, &ind);
            if (ind < 0) { return -1; }
            if (ind >= rbb.n()) { return 0; }
            if (b_streaming && ind >= rbb.n() - 1) { return 0; }
            ecgismp ir1 = rbb(ind)->k;
            if (ir1 < 0) { return -1; }
            ecgismp ie2;
            if (ind < rbb.n() - 1) {
                ie2 = rbb(ind + 1)->k;
                if (ie2 - ir1 <= 2 * cnwpat_r()) { ie2 = (ir1 + ie2) / 2; }
                else {
                    ie2 = ir1 + cnwpat_r();
                }
            }
            else { ie2 = ir1 + cnwpat_r(); }
            if (size_t(ie2) > re.ecg.size()) { return 0; }
            ecgismp ie0;
            if (_gir_prev < 0) {
                ie0 = ir1 - cirpat();
                if (ie0 < 0) { ie0 = 0; }
            }
            else {
                ie0 = _gir_prev - r1.gi0src;
                if (ie0 < 0) { return -1; }
                if (ir1 - ie0 <= 2 * cirpat()) { ie0 = ir1 - (ir1 - ie0) / 2; }
                else {
                    ie0 = ir1 - cirpat();
                }
            }

            if (!pret) {
                pret = &rbb(ind)->v;
                pret->clear();
            }

            _w = re.ecg.substr(size_t(ie0), size_t(ie2 - ie0));
            _wgi0 = ie0 + r1.gi0src;
            _ir = ir1 - ie0;
            _indb = ind;

            _gi = ie2 + r1.gi0src;
        }

        beat_info &rb = *pret;
        // NOTE At this point, the current beat is reagrded as valid (even if noisy).
        //    So, far below, the earliest possible return from this function
        //    sets at least rb flags |= (1 << QRS_names::R),
        //      + appropriate element of inds and f (practically always != 0),
        //      + Lw, Lt (normally to 0),
        //      + hr (may be 0).
        //    rb.empty() will return false at least because of rb.flags != 0.
        // NOTE ~!!! bad_alloc case between here and the earliest return
        //    is currently not implemented, so in such situation this algorihtm, and everything
        //    on top of it, should be reset and restarted to work properly.

        // Center data.
        _qrs_centering(_w, _ir, cdc_k(), cdc_t_ms(), re.fd);

        // Push _w into median accumulator, recalc. min. max. indices where at least 3 values are averaged.
        if (ecgismp(_pat.size()) == cnwpat()) {
            for (size_t i = 0; i < _w.length(); ++i) {
                ecgismp j = cirpat() + ecgismp(i) - _ir;
                median_avg_t<ecgfvsmp> &m = _pat[j];
                m.push(_w[i]);
                if (m.n() >= 3) {
                    if (_pat31 < 0 || j < _pat31) { _pat31 = j; }
                    if (_pat39 < 0 || j > _pat39) { _pat39 = j; }
                }
            }
        }

        ecgismp njoincorr = ecgismp(re.fd * cpat_join_t_ms() / 1000);
        ecgismp n35 = ecgismp(re.fd * 25 / 1000);
        ecgismp n40 = ecgismp(re.fd * 40 / 1000);
        ecgismp n50 = ecgismp(re.fd * 50 / 1000);
        ecgismp n100 = ecgismp(re.fd * 100 / 1000);
        ecgismp i1, i2, nw;
        vector<ecgismp> ranges;
        nw = ecgismp(_w.size());

        // Where median accumulator contains enough data, correct peak interference
        //  based on accu. as template.
        if (ecgismp(_pat.size()) == cnwpat() && _pat31 >= 0 && _pat39 >= 0) {
            for (int z = 1; z <= 2; ++z) {
                if (z == 1) {
                    i1 = _pat31 - cirpat() + _ir;
                    if (i1 < 0) { i1 = 0; }
                    i2 = _ir - n35 + 1;
                } else {
                    i1 = _ir + n40;
                    i2 = _pat39 + 1 - cirpat() + _ir;
                    if (i2 > nw) { i2 = nw; }
                }
                if (i2 - i1 > 0) {
                    ecgismp i, icorr0 = -1, icorr2 = -1;
                    ecgismp nscorr = 0;
                    for (i = i1; i < i2; ++i) {
                        ecgismp j = i - _ir + cirpat();
                        ecgfvsmp fm = _pat[j].median();
                        ecgfvsmp f = _w[i];
                        if (icorr0 >= 0) // now at corrected area
                        {
                            if (fabs(f - fm) >= cthr_sigptn()) // continue the corrected area
                            { icorr2 = i + 1; }
                            else // normal signal
                            {
                                if (i - icorr2 > njoincorr) {
                                    ranges.push_back(icorr0);
                                    ranges.push_back(icorr2);
                                    nscorr += icorr2 - icorr0;
                                    icorr0 = -1;
                                    icorr2 = -1;
                                }
                            }
                        } else // now at normal area
                        {
                            if (fabs(f - fm) >= cthr_sigptn()) {
                                icorr0 = i;
                                icorr2 = i + 1;
                            }
                        }
                    }
                    if (icorr0 >= 0) {
                        ranges.push_back(icorr0);
                        ranges.push_back(icorr2);
                        nscorr += icorr2 - icorr0;
                    }
                    if (nscorr > n100) {
                        if (ranges.front() < _ir) { ranges.front() = 0; }
                        if (ranges.back() > _ir) { ranges.back() = nw; }
                    }
                }
            }

            _w2 = _w;

            // All resulting small ranges of errors are corrected using median as pattern.
            // f_left, f_right, fm_left, fm_right; j = i - _ir + cirpat(); linear approx. la(i), lam(j); f(i) = la(i) + (fm(j) - lam(j))
            ecgfvsmp f_left, f_right, fm_left, fm_right, kf, kfm;
            for (size_t ia = 0; ia < ranges.size(); ia += 2) {
                i1 = ranges[ia];
                i2 = ranges[ia + 1];
                ecgismp j1 = i1 - _ir + cirpat(), j2 = i2 - _ir + cirpat();
                if (j1 > 0) { j1 -= 1; }
                if (j2 >= cnwpat()) { j2 = cnwpat() - 1; }
                f_left = i1 > 0 ? _w2[i1 - 1] : (_w2[0] + _w2[1]) / 2;
                f_right = i2 < nw ? _w2[i2] : (_w2[nw - 1] + _w2[nw - 2]) / 2;
                kf = (f_right - f_left) / (i2 - (i1 - 1));
                fm_left = _pat[j1].median();
                fm_right = _pat[j2].median();
                kfm = (fm_right - fm_left) / (j2 - j1);
                for (ecgismp i = i1; i < i2; ++i) {
                    ecgismp j = i - _ir + cirpat();
                    _w2[i] = (f_left + kf * (i - (i1 - 1))) +
                             (_pat[j].median() - (fm_left + kfm * (j - j1)));
                }
            }
        } else { _w2 = _w; }

        double m1 = pow(chf_k(), 1000 / re.fd / chf_t_ms());
        _avg_bidi(_w2, m1, 1 - m1);

        // Calc. noise levels.
        double Lw[2] = {0, 0};
        for (int z = 0; z <= 1; ++z) {
            ecgismp nhalf;
            if (z == 0) {
                i1 = 0;
                i2 = _ir - n50;
                nhalf = cirpat();
            }
            else {
                i1 = _ir + n50;
                i2 = nw;
                nhalf = cnwpat_r();
            }
            if (i2 - i1 > 0) {
                double N = 0;
                double q0 = 0;
                for (ecgismp i = i1; i < i2; ++i) {
                    double q = floor(fabs(_w[i] - _w2[i]) / cthr_noise());
                    if (q) {
                        if (!q0) { q0 = q; }
                        N += q / (1 + fabs(q - q0)) * (3 - 2.5 * fabs(double(i - _ir)) / nhalf);
                    }
                    q0 = q;
                } // the last multiplier applies triangular window
                N /= i2 - i1;
                if (N > 1) { N = 1; }
                Lw[z] = N;
            }
        }


        // === === 2. END OF PART 1 + BEAT PARA. EST. === ===
        using namespace QRS_names;

        // Subsample R position and estimated max. level.
        while (1) // once
        {
            ecgfvsmp f;
            double ts;
            ecgismp i;
            double fxw;
            resampler r2(re.fd, re.fd, &_w,
                         _wgi0); // R peak is estimated for data with only DC removed (this saves peak height)

            int res = r2.v_interp(_wgi0 + _ir, &f, &ts,
                                  true); // src. and dest. sample indices are the same (no resampling)
            if (res > 0) {
                fxw = ts * re.fd - _wgi0;
                ecgfvsmp fa = fabs(f), f0 = _w[_ir], fa0 = fabs(f0);
                if (fa < fa0 || (f >= 0) != (f0 >= f0)) { f = f0; }
                else if (fa0 >= 1 && fa > 0.1 * fa0) { f = 1.1 * f0; }
                else if (fa0 < 1 && fa - fa0 >= 0.2) { f = 1.25 * f0; }
            } else {
                ts = (_wgi0 + _ir) / re.fd;
                fxw = _ir;
                f = _w[_ir];
            }

            double hr = 0;
            s_long fLw2 = (int(Lw[0] >= 0.2) << 1) | int(Lw[1] >= 0.2);
            if (_gir_prev >= 0) {
                double dt = ts - _tsr_prev;
                if (dt < 0.25 || Lw[0] + Lw[1] == 2 ||
                    (f < 0 && (Lw[0] == 1 || Lw[1] == 1))) {} // probably false positive
                else {
                    bool b_skip = false;
                    double hrm = _avg_hr.median();
                    _fLw <<= 2;
                    _fLw |= fLw2;
                    hr = 60 / dt;
                    if (hr < 20) { hr = 0; } // qualify as no rhythm
                    else if (hr < 40) // in [20..40), may be rhythm under certain conditions
                    {
                        s_long mask = 0x3f;
                        if (_fLw & mask) { hr = hrm; } // use avg. value
                        else if (hrm && hr >= 0.66 * hrm && hr < 1.5 * hrm) { _avg_hr.push(hr); }
                        else {
                            _avg_hr.push(hr);
                            hr = hrm;
                        } // use avg. value
                    } else if (hrm && !(hr >= 0.7 * hrm &&
                                        hr < 1.4 * hrm)) // possibly missed beat or extrasystole
                    {
                        s_long mask = 0xf;
                        if (hr >= 1.4 * hrm && (Lw[0] == 1 || Lw[1] == 1 || Lw[0] + Lw[1] > 1.5)) {
                            hr = 0;
                            b_skip = true;
                        }
                        else if (_fLw & mask) {
                            hr = 0.5 * (hr + hrm);
                        } // if noise might have been influenced, smooth the estimate
                        if (hr) { _avg_hr.push(hr); }
                    } else // normal beat, or primary averaging
                    { _avg_hr.push(hr); }

                    if (!b_skip) {
                        _gir_prev = _wgi0 + _ir;
                        _tsr_prev = ts;
                    }
                }
            } else {
                _fLw <<= 2;
                _fLw |= fLw2;
                _gir_prev = _wgi0 + _ir;
                _tsr_prev = ts;
            }

            // Set noise levels.
            rb.Lp = Lw[0];
            rb.Lt = Lw[1];

            i = R;
            rb.flags |= (1 << i);
            rb.inds[i] = float(fxw - _ir);
            rb.f[i] = f;
            rb.hr = hr;

            if (f <
                0) { return 1; } // estimation for reversed beat data is not implemented, exiting with R pos. and value only

            break;
        }

        if (!b_calc_pqst) { return 1; }

        // P side key points:
        //  1) xi0 = R - 25ms -- initial point to look for P at the left
        //    2) xi1 (< xi0) -- first intersection upwards with fhalf = avg(f) + (max(f) - avg(f)) * 0.4
        //      3) xi2 (< xi1) -- first intersection downwards with fhalf
        //        4) xmaxaw -- (between xi1 and xi2) P wave weighted avg. max. position
        //        5) xi3 (< xi2) -- first intersection upwards with f01 = avg(f) + (max(f) - avg(f)) * 0.1, or 0 (window beginning) if not found
        //          6) xbend3 (< xmaxaw) -- arg. min. (f - lin. approx(f, [xi3 .. xi2]) -- beginning of P left slope
        //      7) (if R > f[xi1]) xpk1 -- arg. min. (f - lin. approx(f, [xi1 .. R])), in [xi1+1 .. xi0] -- Q peak pos.
        //        8) xbend1 -- arg. min. (f - lin. approx(f, [xmaxaw .. xpk1])), in [xi1+1 .. xpk1-1], only if != 0 -- PR segment beginning (end of P right slope)
        //        9) xbend2 -- arg. max. (f - lin. approx(f, [xbend1 .. xpk1])), in [max(xi1, xbend1)+1 .. xpk1-1], only if != 0 -- PR segment end (Q peak beginning)
        while (1) // once
        {
            ecgismp i, xi0, xi1, xi2, xmaxaw, xi3, xbend3, xpk1, xbend1, xbend2;
            ecgismp ilim = 0; // boundary sample index, inclusive (see below), i < ilim won't be analyzed
            xi0 = _ir - n40;
            if (xi0 < 5 || xi0 < n40) { break; } // too little data
            double fmax = _w2[xi0], favg = _w2[xi0];
            if (1) {
                const ecgismp nlim = n40;
                ecgismp nseg = 0;
                ilim = xi0;
                for (i = xi0 - 1; i >= 0; --i) {
                    if (_w2[i] > fmax) { fmax = _w2[i]; }
                    favg += _w2[i];
                    if ((_w2[i] >= 0) != (_w2[i + 1] >= 0)) // new segment
                    {
                        if (ilim - i > nlim) {
                            ++nseg;
                            ilim = i;
                            if (nseg >= 3) { break; }
                        }
                    }
                }
                if (nseg < 3) { ilim = 0; }
                favg /= (xi0 + 1 - ilim);
            }
            double fhalf = favg + (fmax - favg) * 0.4;
            xi1 = -1;
            for (i = xi0 - 1; i >= ilim; --i) {
                if (_w2[i] > fhalf && _w2[i + 1] <= fhalf) {
                    xi1 = i;
                    break;
                }
            }
            if (xi1 < 0) { break; }
            xi2 = -1;
            for (i = xi1 - 1; i >= ilim; --i) {
                if (_w2[i] < fhalf) {
                    xi2 = i;
                    break;
                }
            }
            xmaxaw = xi3 = xbend3 = -1;
            double zmin, zmax, f_left, f_right, kf;
            if (xi2 >= 0) {
                double f01 = favg + (fmax - favg) * 0.1;
                double sum1 = 0, sum2 = 0;
                for (i = xi2 + 1; i <= xi1; ++i) {
                    sum1 += i * (_w2[i] - favg);
                    sum2 += _w2[i] - favg;
                }
                xmaxaw = ecgismp(sum1 / sum2);
                if (xmaxaw < xi2 + 1) { xmaxaw = xi2 + 1; }
                if (xmaxaw > xi1) { xmaxaw = xi1; }
                for (i = xi2 - 1; i >= ilim; --i) {
                    if (_w2[i] > f01 && _w2[i + 1] <= f01) {
                        xi3 = i;
                        break;
                    }
                }
                if (xi3 < 0) { xi3 = ilim; }
                if (xi3 >= xi2) { xi3 = -1; }
                if (xi3 >= 0) {
                    f_left = _w2[xi3], f_right = _w2[xi2 + 1], kf =
                            (f_right - f_left) / (xi2 + 1 - xi3);
                    zmin = 0;
                    xbend3 = -1;
                    for (i = xi3 + 1; i <= xi2; ++i) {
                        double z = _w2[i] - (f_left + kf * (i - xi3));
                        double k = double(i - xi3) / (xi2 + 1 - xi3);
                        z += z * k * k;
                        if (xbend3 < 0 || z < zmin) {
                            xbend3 = i;
                            zmin = z;
                        }
                    }
                }
            }
            ecgismp i0, i9;
            f_left = _w[xi1], f_right = _w[_ir]; // Q peak is estimated for data with only DC removed (this saves peak height)
            xpk1 = xbend1 = xbend2 = -1;
            if (f_right > f_left) {
                kf = (f_right - f_left) / (_ir - xi1);
                zmin = 0;
                i0 = xi1 + 1;
                i9 = _ir - 1;
                for (i = i0; i <= i9; ++i) {
                    double z = _w[i] - (f_left + kf * (i - xi1));
                    if (z < zmin) {
                        xpk1 = i;
                        zmin = z;
                    }
                } // taken from _w
                if (xpk1 >= 0) {
                    f_left = _w2[xmaxaw]; // left point pos. is taken from P top
                    f_right = _w[xpk1]; // right point is taken from _w, because this value is estimated Q (min.)
                    kf = (f_right - f_left) / (xpk1 - xmaxaw);
                    zmin = 0;
                    i0 = xi1 + 1;
                    i9 = xpk1 - 1;
                    for (i = i0; i <= i9; ++i) {
                        double z = _w2[i] - (f_left + kf * (i - xmaxaw));
                        if (z < zmin) {
                            xbend1 = i;
                            zmin = z;
                        }
                    }
//            if (xbend1 < 0)
//            {
//              f_left = _w2[xi1];
//              f_right = _w[xpk1]; // taken from _w
//              kf = (f_right - f_left) / (xpk1 - xi1);
//              zmax = 0; i0 = xi1 + 1; i9 = xpk1 - 1;
//                for (i = i0; i <= i9; ++i) { double z = _w[i] - (f_left + kf * (i - xi1)); if (z > zmax) { xbend2 = i; zmax = z; } } // taken from _w
//            }
//            else
//            {
//              f_left = _w2[xbend1], f_right = _w2[xpk1], kf = (f_right - f_left) / (xpk1 - xbend1);
//              zmax = 0; i0 = xbend1 + 1; i9 = xpk1 - 1;
//                for (i = i0; i <= i9; ++i) { double z = _w[i] - (f_left + kf * (i - xbend1)); if (z > zmax) { xbend2 = i; zmax = z; } } // taken from _w
//            }
                    if (xbend1 < 0) {
                        i0 = xi1 + 1;
                        i9 = xpk1 - 1;
                    }
                    else {
                        i0 = xbend1 + 1;
                        i9 = xpk1 - 1;
                        if (i9 - i0 > n35) { i0 += (i9 - i0) / 2; }
                    }
                    f_left = _w2[i0 - 1];
                    f_right = _w[i9 + 1];
                    if (f_left > f_right) {
                        double f_avg = (f_left + f_right) / 2;
                        zmax = 0;
                        for (i = i0; i <= i9; ++i) {
                            if (_w2[i] > 0 && _w[i] < f_left && _w[i] > f_avg) {
                                double z = (i9 - i) / re.fd + _w2[i] - f_right;
                                if (z > zmax) {
                                    xbend2 = i;
                                    zmax = z;
                                }
                            }
                        }
                    }
                }
            }

            ecgismp xw;
            xw = xmaxaw;
            i = P;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }
            xw = xbend3;
            i = P0;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }
            xw = xpk1;
            i = Q;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = _w[xw];
            } // taken from _w
            xw = xbend1;
            i = P2;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }
            xw = xbend2;
            i = Q0;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = _w[xw];
            } // taken from _w

            break;
        }

        // T side key points:
        //  1) xi0 = R + 25ms -- initial point to look for T at the right
        //    2) xi1 (> xi0) -- first intersection upwards with fhalf = avg(f) + (max(f) - avg(f)) * 0.4
        //      3) xi2 (> xi1) -- first intersection downwards with fhalf
        //        4) xmaxaw -- (between xi1 and xi2) T wave weighted avg. max. position
        //        5) xi3 (> xi2) -- first intersection upwards with f01 = avg(f) + (max(f) - avg(f)) * 0.1, or nw - 1 (last window sample) if not found
        //          6) xbend3 (> xmaxaw) -- arg. min. (f - lin. approx(f, [xi2 .. xi3]) -- end of T right slope
        //      7) (if R > f[xi1]) xpk1 -- arg. min. (f - lin. approx(f, [R .. xi1])), in [xi0 .. xi1-1] -- S peak pos.
        //        8) xbend1 -- arg. max. (f - lin. approx(f, [xpk1 .. xi1])), in [xpk1+1 .. xi1-1], only if != 0 -- ST segment beginning (S peak end)
        //        9) xbend2 -- arg. min. (f - lin. approx(f, [xpk1 .. xmaxaw])), in [max(xpk1, xbend1) + 1 .. xi1-1], only if != 0 -- ST segment end (beginning of T left slope)
        while (1) // once
        {

            ecgismp i, xi0, xi1, xi2, xmaxaw, xi3, xbend3, xpk1, xbend1, xbend2;
            ecgismp ilim = 0; // boundary sample index, inclusive (see below), i > ilim won't be analyzed
            xi0 = _ir + n40;
            double fmax = _w2[xi0], favg = _w2[xi0];
            if (1) {
                const ecgismp nlim = n40;
                ecgismp nseg = 0;
                ilim = xi0;
                for (i = xi0 + 1; i < nw; ++i) {
                    if (_w2[i] > fmax) { fmax = _w2[i]; }
                    favg += _w2[i];
                    if ((_w2[i] >= 0) != (_w2[i - 1] >= 0)) // new segment
                    {
                        if (i - ilim > nlim) {
                            ++nseg;
                            ilim = i;
                            if (nseg >= 3) { break; }
                        }
                    }
                }
                if (nseg < 3) { ilim = nw - 1; }
                favg /= (ilim + 1 - xi0);
            }
            double fhalf = favg + (fmax - favg) * 0.4;
            xi1 = -1;
            for (i = xi0 + 1; i <= ilim; ++i) {
                if (_w2[i] > fhalf && _w2[i - 1] <= fhalf) {
                    xi1 = i;
                    break;
                }
            }
            if (xi1 < 0) { break; }
            xi2 = -1;
            for (i = xi1 + 1; i <= ilim; ++i) {
                if (_w2[i] < fhalf) {
                    xi2 = i;
                    break;
                }
            }
            xmaxaw = xi3 = xbend3 = -1;
            double zmin, zmax, f_left, f_right, kf;
            if (xi2 >= 0) {
                double f01 = favg + (fmax - favg) * 0.1;
                double sum1 = 0, sum2 = 0;
                for (i = xi1; i < xi2; ++i) {
                    sum1 += i * (_w2[i] - favg);
                    sum2 += _w2[i] - favg;
                }
                xmaxaw = ecgismp(sum1 / sum2);
                if (xmaxaw < xi1) { xmaxaw = xi1; }
                if (xmaxaw >= xi2) { xmaxaw = xi2 - 1; }
                for (i = xi2 + 1; i <= ilim; ++i) {
                    if (_w2[i] > f01 && _w2[i - 1] <= f01) {
                        xi3 = i;
                        break;
                    }
                }
                if (xi3 < 0) { xi3 = ilim; }
                if (xi3 <= xi2) { xi3 = -1; }
                if (xi3 >= 0) {
                    f_left = _w2[xi2 - 1], f_right = _w2[xi3], kf =
                            (f_right - f_left) / (xi3 - (xi2 - 1));
                    zmin = 0;
                    xbend3 = -1;
                    for (i = xi2; i < xi3; ++i) {
                        double z = _w2[i] - (f_left + kf * (i - (xi2 - 1)));
                        double k = 1 - double(i - (xi2 - 1)) / (xi3 - (xi2 - 1));
                        z += z * k * k;
                        if (xbend3 < 0 || z < zmin) {
                            xbend3 = i;
                            zmin = z;
                        }
                    }
                }
            }
            ecgismp i0, i9;
            f_left = _w[_ir], f_right = _w[xi1]; // S peak is estimated for data with only DC removed (this saves peak height)
            xpk1 = xbend1 = xbend2 = -1;
            if (f_left > f_right) {
                kf = (f_right - f_left) / (xi1 - _ir);
                zmin = 0;
                i0 = _ir + 1;
                i9 = xi1 - 1;
                for (i = i0; i <= i9; ++i) {
                    double z = _w[i] - (f_left + kf * (i - _ir));
                    if (z < zmin) {
                        xpk1 = i;
                        zmin = z;
                    }
                }
                if (xpk1 >= 0) {
                    f_left = _w2[xpk1], f_right = _w2[xi1], kf = (f_right - f_left) / (xi1 - xpk1);
                    zmax = 0;
                    i0 = xpk1 + 1;
                    i9 = xi1 - 1;
                    for (i = i0; i <= i9; ++i) {
                        double z = _w2[i] - (f_left + kf * (i - xpk1));
                        if (z > zmax) {
                            xbend1 = i;
                            zmax = z;
                        }
                    }
                    if (xbend1 < 0) {
                        f_left = _w[xpk1], f_right = _w2[xi1], kf = (f_right - f_left) / (xi1 -
                                                                                          xpk1); // left point is taken from _w, because this value is estimated S (min.)
                        zmin = 0;
                        i0 = xpk1 + 1;
                        i9 = xi1 - 1;
                        for (i = i0; i <= i9; ++i) {
                            double z = _w2[i] - (f_left + kf * (i - xpk1));
                            if (z < zmin) {
                                xbend2 = i;
                                zmin = z;
                            }
                        }
                    } else {
                        f_left = _w2[xbend1];
                        f_right = _w2[xmaxaw], kf = (f_right - f_left) / (xmaxaw - xbend1);
                        zmin = 0;
                        i0 = xbend1 + 1;
                        i9 = xi1 - 1;
                        for (i = i0; i <= i9; ++i) {
                            double z = _w2[i] - (f_left + kf * (i - xbend1));
                            if (z < zmin) {
                                xbend2 = i;
                                zmin = z;
                            }
                        }
                    }
                }
            }

            ecgismp xw;
            xw = xmaxaw;
            i = T;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }
            xw = xbend3;
            i = T2;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }
            xw = xpk1;
            i = S;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = _w[xw];
            }
            xw = xbend1;
            i = S2;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = _w2[xw];
            }
            xw = xbend2;
            i = T0;
            if (xw >= 0) {
                rb.flags |= (1 << i);
                rb.inds[i] = xw - _ir;
                rb.f[i] = chf_ptcorr() * _w2[xw];
            }

            break;
        }

        return 1;
    }

    void alg_pqrst::_avg_bidi(basic_string<ecgfvsmp> &w, double m1, double m2) {
        const ecgismp n = ecgismp(w.size());
        if (!n) { return; }
        for (ecgismp i = 1; i < n; ++i) { w[i] = w[i - 1] * m1 + w[i] * m2; }
        for (ecgismp i = n - 2; i >= 0; --i) { w[i] = w[i + 1] * m1 + w[i] * m2; }
    }

    void alg_pqrst::_qrs_centering(basic_string<ecgfvsmp> &w, ecgismp ir, double k, double t_ms,
                                   double fd, basic_string<ecgfvsmp> *pavg) {
        if (w.size() == 0) { return; }
        basic_string<ecgfvsmp> avg0, &avg = pavg ? *pavg : avg0;
        avg = w;
        ecgismp n40 = ecgismp(fd * 25 / 1000);
        ecgismp n50 = ecgismp(fd * 40 / 1000);
        ecgismp i1 = ir - n40, i2 = ir + n50;
        msla_data_t<ecgfvsmp, ecgismp> la;
        la.points[i1] = w[i1];
        la.points[i2] = w[i2];
        for (ecgismp i = i1; i < i2; ++i) { avg[i] = la.f(i); }
        double m1 = pow(k, 1000 / fd / t_ms);
        _avg_bidi(avg, m1, 1 - m1);
        for (size_t i = 0; i < w.size(); ++i) { w[i] -= avg[i]; }
    }

    void alg_pqrst::_pat_reset() {
        _pat.clear();
        _pat.resize(cnwpat(), median_avg_t<ecgfvsmp>(cnmed));
        _pat31 = _pat39 = -1;
        ecgismp n1 = ms1000 * 5 / 10;
        ecgismp n2 = ms1000 * 8 / 10;
        for (ecgismp i = 0; i < cnwpat(); ++i) {
            ecgismp j = i - cirpat();
            if (j < -n2 || j >= n2) {
                _pat[i].push(-0.1);
                _pat[i].push(0);
                _pat[i].push(0.1);
            }
            else if (j < -n1 || j >= n1) {
                _pat[i].push(-0.1);
                _pat[i].push(0.1);
            }
        }
    }

    int alg_slpst_L1::plan_cig::eval(ecg_data &dat, const basic_string<ecgismp> &gibeats,
                                     ecgismp gi0src) {
        const ecgismp ncig = ecgismp(gibeats.length());
        if (ncig < nbeatsmincig) { return 0; }

        basic_string<double> dts;
        median_avg_t<double> dstat(s_long(gibeats.length()));
        for (size_t j = 0; j < gibeats.length(); ++j) {
            const ordhs_t<ecgismp, beat_info>::entry *e1 = dat.beats.find(gibeats[j] - gi0src);
            if (!e1) { return 0; }
            double x = e1->v.hr;
            if (x < hrmincig) { return 0; }
            x = 60 / x;
            dts.push_back(x);
            dstat.push(x);
        }
        double ddt = dstat.max_i(3) - dstat.min_i(3);
        if (ddt < 1.e-6) { return -1; }

        stat_mode mo;
        mo.mode_f(&dts[0], s_long(dts.length()), dtmscig / 1000.);
        double amo = double(mo.ret_n) / ncig;
        if (amo < 0.01) { return -1; }
        xdts = ddt;
        xvb = 1 / mo.ret_mo / amo;
        xin = 50 * amo / mo.ret_mo / ddt;

        return 1;
    }

    bool
    alg_slpst_L1::plan_slpst::test_phd(int ph1, int ph2, int ph3, int ph4, int ph5, int ph6) const {
        if (phd == ph1) { return true; }
        if (phd == ph2) { return true; }
        if (ph2 == -10) { return false; }
        if (phd == ph3) { return true; }
        if (ph3 == -10) { return false; }
        if (phd == ph4) { return true; }
        if (ph4 == -10) { return false; }
        if (phd == ph5) { return true; }
        if (ph5 == -10) { return false; }
        if (phd == ph6) { return true; }
        return false;
    }

    // Sets new phase values and updates time point.
    void alg_slpst_L1::plan_slpst::set_ph(int ph) {
        if (phd == ph) {
            phe = ph;
            return;
        }
        tphd0s = ts;
        if (naf == 1) {
            if (ph == 0 && test_phd(3, 4)) {
                naf = 2;
                phe = phd = ph;
                return;
            }
            if (phd == -1 && (ph == 1 || ph == 2)) {
                phe = phd = ph;
                return;
            }
            if (phd == 1 && (ph == -1 || ph == 2 || ph == 3)) {
                phe = phd = ph;
                return;
            }
            if (phd == 2 && (ph == 1 || ph == 3 || ph == 4)) {
                phe = phd = ph;
                return;
            }
            if (phd == 3 && (ph == 2 || ph == 4)) {
                phe = phd = ph;
                return;
            }
            if (phd == 4 && (ph == 3)) {
                phe = phd = ph;
                return;
            }
            naf = 0;
            phe = phd = ph;
            return;
        } else if (naf == 2) {
            if (ph == 0 && test_phd(3, 4)) {
                naf = 2;
                phe = phd = ph;
                return;
            }
            if (phd == 0 && (ph == 1 || ph == 2)) {
                phe = phd = ph;
                return;
            }
            if (phd == 1 && (ph == 0 || ph == 2 || ph == 3)) {
                phe = phd = ph;
                return;
            }
            if (phd == 2 && (ph == 1 || ph == 3 || ph == 4)) {
                phe = phd = ph;
                return;
            }
            if (phd == 3 && (ph == 2 || ph == 4)) {
                phe = phd = ph;
                return;
            }
            if (phd == 4 && (ph == 3)) {
                phe = phd = ph;
                return;
            }
            naf = 0;
            phe = phd = ph;
            return;
        } else {
            phe = phd = ph;
            return;
        }
    }

    const alg_slpst_L1::plan_cig &alg_slpst_L1::plan_slpst::cig_nearest() const {
        int i0 = -1;
        int dt = 0;
        for (int i = nhcig - 1; i >= 0; --i) {
            const plan_cig &p = hcig[i];
            if (p.ts2 > p.ts0) {
                if (ts >= p.ts0 && ts < p.ts2) {
                    i0 = i;
                    dt = 0;
                    break;
                }
                double dt2 = myfmin(fabs(ts - p.ts0), fabs(ts - p.ts2));
                if (i0 < 0 || dt2 < dt) {
                    i0 = i;
                    dt = dt2;
                }
            }
        }
        if (i0 >= 0) { return hcig[i0]; }
        return cig_last();
    }

    const alg_slpst_L1::plan_cig *alg_slpst_L1::plan_slpst::pcig_fact() const {
        for (int i = nhcig - 1; i >= 0; --i) {
            const plan_cig &p = hcig[i];
            if (ts >= p.ts0 && ts < p.ts2) { return &p; }
        }
        return 0;
    }


    void alg_slpst_L1::state_L1::_eval_next_stat(double hr, double ts_now, bool b_filtered_sample) {
        ts = ts_now;

        const double eps = 1.e-6;
        double x1, x2, x3;

        if (b_filtered_sample) {
            _q1sd10.push(hr);
            x2 = 0;
            x1 = _q1sd10.avg_f();
            if (x1 > eps) { x2 = 10 * _q1sd10.stddev_f() / x1; }
            sd10 = x2;

            _q2sdn6.push(hr);
            x2 = 0;
            x1 = _q2sdn6.avg_f();
            x3 = _q2sdn6.stddev_f();
            if (x1 > eps) { x2 = 60 * x3 * x3 / x1; }
            _q2min.push(x2);
            _q2amssd.push(myfmin(10, _q2min.min1()));
            amssd = _q2amssd.avg_f();
        }

        if (true) {
            // Continuous values.
            double _sd10, _amssd;

            _q31sd10.push(hr);
            x2 = 0;
            x1 = _q31sd10.avg_f();
            if (x1 > eps) { x2 = 10 * _q31sd10.stddev_f() / x1; }
            _sd10 = x2;

            _q31sdn6.push(hr);
            x2 = 0;
            x1 = _q31sdn6.avg_f();
            x3 = _q31sdn6.stddev_f();
            if (x1 > eps) { x2 = 60 * x3 * x3 / x1; }
            _q31min.push(x2);
            _q31amssd.push(myfmin(10, _q31min.min1()));
            _amssd = _q31amssd.avg_f();

            x1 = hr >= 20 ? 60 / hr : 3;
            _q3wi1.push(x1);
            x2 = _q3wi1.stddev_f();
            x3 = _q3wi1.avg_f();
            wi1 = 0;
            if (_q3wi1.n() >= 10 && _sd10 && _amssd) {
                wi1 = std::fabs(12 * x2 - x3) * std::fabs(_sd10 - 0.2) / myfmax(0.1, _amssd);
            }
        }

        if (b_filtered_sample) {
            _hr_last = hr;
        }
    }

    void alg_slpst_L1::state_L1::_estimate_slpst(double ts_now) {
        ts = ts_now;
        tu0s = ts_now;

        state_L1 &stp = *this;
        const ruleset_coefs &c = stp._coefs;
        const plan_cig *cig = stp.pcig_fact();
        rulestat &rls = stp._rls;
        bool b = false;

        // Common rules.
        b = stp.test_phd(-1, 1, 2, 3) && stp.wi1 >= c.j12 && stp.sd10 >= c.j13 &&
            stp.sd10 < c.j14 && stp.amssd < c.j15;
        rls("Ra1", b);
        if (b) {
            stp.set_ph(-1);
            return;
        }
        b = stp.test_phd(-1, 1, 2, 3, 4, 0) && stp.wi1 >= c.j11 && stp.amssd >= c.j15;
        rls("Ra2", b);
        if (b) {
            stp.set_ph(2);
            return;
        }
        b = stp.test_phd(1, 2, 3, 4, 0) && stp.wi1 >= c.j11;
        rls("Ra3", b);
        if (b) {
            stp.set_ph(0);
            return;
        }

        if (cig) // normally
        {
            b = cig->xin >= c.k6 && stp.amssd < c.k7;
            rls("R1", b);
            if (b) {
                stp.set_ph(-1);
                return;
            }
            b = stp.test_phd(-1) && cig->xin >= c.k8 && stp.amssd < c.k9;
            rls("R10", b);
            if (b) {
                stp.set_ph(-1);
                return;
            }

            b = stp.test_phd(1, 2) && cig->xvb >= c.k21 && stp.sd10 >= c.k22 &&
                stp.amssd >= c.k24 && stp.amssd < c.k25;
            rls("R2a", b);
            if (b) {
                stp.set_ph(0);
                return;
            }
            b = stp.test_phd(3, 4, 0) && cig->xvb >= c.k21 && stp.sd10 >= c.k23 &&
                stp.amssd >= c.k24;
            rls("R2b", b);
            if (b) {
                stp.set_ph(0);
                return;
            }

            b = cig->xvb >= c.k26 && stp.sd10 >= c.k23 && stp.amssd >= c.k24;
            rls("R2c", b);
            if (b) {
                stp.set_ph(2);
                return;
            }

            b = stp.test_phd(-2) && cig->xvb >= c.k27 && stp.sd10 < c.k28 && stp.amssd < c.j31;
            rls("R2d", b);
            if (b) {
                stp.set_ph(3);
                return;
            }

            b = cig->xvb < c.k1 && stp.sd10 < c.k5;
            rls("R5", b);
            if (b) {
                b = stp.test_phd(-1) || (stp.naf == 1 && stp.test_phd(-1, 1, 2, 3, 4));
                rls("R5a", b);
                if (b) {
                    stp.naf = 1;
                    if (stp.test_phd(-1)) {
                        if (cig->xin >= c.k10) {
                            stp.tphd0s = stp.ts;
                            return;
                        }
                        if (stp.ts - stp.tphd0s >= 4 * 60) { stp.set_ph(1); }
                        return;
                    }
                    if (stp.test_phd(1)) {
                        if (stp.ts - stp.tphd0s >= 3 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(2) && stp.sd10 + stp.amssd < c.j32) {
                        if (stp.ts - stp.tphd0s >= 8 * 60) { stp.set_ph(3); }
                        return;
                    }
                    if (stp.test_phd(3) && stp.amssd < c.j4) {
                        if (stp.ts - stp.tphd0s >= 8 * 60) { stp.set_ph(4); }
                        return;
                    }
                    stp.set_ph(stp.phd); // may influence phe
                    return;
                }
                b = stp.test_phd(0) || (stp.naf == 2 && stp.test_phd(0, 1, 2, 3, 4));
                rls("R5b", b);
                if (b) {
                    stp.naf = 2;
                    if (stp.test_phd(0)) {
                        if (cig->xin >= c.k10) {
                            stp.tphd0s = stp.ts;
                            return;
                        }
                        if (stp.ts - stp.tphd0s >= 3 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(1)) {
                        if (stp.ts - stp.tphd0s >= 1 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(2) && stp.sd10 + stp.amssd < c.j32) {
                        if (stp.ts - stp.tphd0s >= 5 * 60) { stp.set_ph(3); }
                        return;
                    }
                    if (stp.test_phd(3) && stp.amssd < c.j4) {
                        if (stp.ts - stp.tphd0s >= 5 * 60) { stp.set_ph(4); }
                        return;
                    }
                    stp.set_ph(stp.phd); // may influence phe
                    return;
                }
                naf = 0;
                rls("R5c", true);
            }

            b = stp.test_phd(4) && (cig->xvb >= c.k1 || stp.amssd >= c.j41);
            rls("R6a", b);
            if (b) {
                stp.set_ph(3);
                return;
            }

            b = stp.test_phd(3) && cig->xvb >= c.k1;
            rls("R6b", b);
            if (b) {
                stp.set_ph(2);
                return;
            }

            b = stp.test_phd(0) && stp.amssd >= c.k31 && stp.amssd < c.k32;
            rls("R6c", b);
            if (b) {
                stp.set_ph(1);
                return;
            }
        } else // CIG data not available, estimate based only on SD10, AMSSD
        {
            b = stp.sd10 >= c.q11 && stp.amssd >= c.q12;
            rls("Rnc12", b);
            if (b) {
                stp.set_ph(-1);
                return;
            }
            b = stp.sd10 >= c.q52 && stp.amssd >= c.q53;
            rls("Rnc4", b);
            if (b) {
                stp.set_ph(2);
                return;
            }
            b = stp.sd10 < c.q13 && stp.amssd >= c.q14 && stp.wi1 < c.q16;
            rls("Rnc13", b);
            if (b) {
                stp.set_ph(2);
                return;
            }
            b = !stp.test_phd(-1) && stp.sd10 >= c.q10 && stp.sd10 < c.q15 && stp.amssd < c.q9;
            rls("Rnc14", b);
            if (b) {
                stp.set_ph(-1);
                return;
            }
            b = !stp.test_phd(-1) && stp.sd10 >= c.q15 && stp.amssd < c.q9;
            rls("Rnc15", b);
            if (b) {
                stp.set_ph(2);
                return;
            }

            b = stp.sd10 < c.q5;
            rls("Rnc5", b);
            if (b) {
                b = stp.test_phd(-1) || (stp.naf == 1 && stp.test_phd(-1, 1, 2, 3, 4));
                rls("Rnc5a", b);
                if (b) {
                    stp.naf = 1;
                    if (stp.test_phd(-1) && stp.amssd >= c.q17) {
                        if (stp.ts - stp.tphd0s >= 3 * 60) { stp.set_ph(1); }
                        return;
                    }
                    if (stp.test_phd(1) && stp.amssd >= c.q18) {
                        if (stp.ts - stp.tphd0s >= 3 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(2) && stp.sd10 + stp.amssd < c.j32) {
                        if (stp.ts - stp.tphd0s >= 8 * 60) { stp.set_ph(3); }
                        return;
                    }
                    if (stp.test_phd(3) && stp.amssd < c.j4) {
                        if (stp.ts - stp.tphd0s >= 8 * 60) { stp.set_ph(4); }
                        return;
                    }
                    stp.set_ph(stp.phd); // may influence phe
                    return;
                }
                b = stp.test_phd(0) || (stp.naf == 2 && stp.test_phd(0, 1, 2, 3, 4));
                rls("Rnc5b", b);
                if (b) {
                    stp.naf = 2;
                    if (stp.test_phd(0)) {
                        if (stp.ts - stp.tphd0s >= 3 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(1)) {
                        if (stp.ts - stp.tphd0s >= 1 * 60) { stp.set_ph(2); }
                        return;
                    }
                    if (stp.test_phd(2) && stp.sd10 + stp.amssd < c.j32) {
                        if (stp.ts - stp.tphd0s >= 5 * 60) { stp.set_ph(3); }
                        return;
                    }
                    if (stp.test_phd(3) && stp.amssd < c.j4) {
                        if (stp.ts - stp.tphd0s >= 5 * 60) { stp.set_ph(4); }
                        return;
                    }
                    stp.set_ph(stp.phd); // may influence phe
                    return;
                }
                naf = 0;
                rls("Rnc5c", true);
            }

            b = !stp.test_phd(-1) && stp.amssd < c.q31;
            rls("Rnc16", b);
            if (b) {
                stp.set_ph(-1);
                return;
            }
            b = stp.test_phd(-1, 0, 1, 2, 3, 4) && stp.amssd >= c.q31 && stp.amssd < c.q32;
            rls("Rnc8", b);
            if (b) {
                if (stp.test_phd(-1, 0, 1)) {
                    stp.set_ph(1);
                    return;
                }
                if (stp.test_phd(4)) {
                    stp.set_ph(3);
                    return;
                }
                stp.set_ph(2); // stp.test_phd(2, 3) == true
                return;
            }
        }

        // The last rule
        b = stp.test_phd(-2);
        rls("Ra9", b);
        if (b) {
            stp.set_ph(-1);
            return;
        }
        return;
    }

    void alg_slpst_L1::state_L1::_notify_slpst_disabled(double ts_now) {
        ts = ts_now;
        if (phd != -2 && ts - tu0s >= dtms_slpst_lost / 1000.) {
            naf = 0;
            phd = -2;
            tphd0s = ts;
            tu0s = ts;
        }
    }

    int alg_slpst_L1::run(ecgismp gi9, bool b_est_slpst, bool b_streaming) {
        if (gi9 < stp.r1.gi0src) { return -1; }
        if (gi9 <= stp._gi - 1 || !stp.pecg) { return -2; }

        const ordhs_t<ecgismp, beat_info> &rbb = stp.pecg->beats;

        // Ensure the last CIG data window to be up to and including the current place.
        //    If the data is noisy, the last CIG window will end before the current position.
        // At the same time, valid beat indexes for main algorithm are queued.
        //    At loop exit, the queue contains enough beats for whole algorithm advance to gi9.
        if (stp._gicignew < gi9 + 1) {
            basic_string<ecgismp> gib_cig, gib_stat2;
            s_long ind_b = -1;
            rbb.find(stp._gicignew - stp.r1.gi0src, &ind_b);
            if (ind_b < 0) { return -2; }
            if (ind_b + nbeatscig > rbb.n()) {
                // To evaluate the current pos. (gi9), at least nbeatscig beats must be available,
                //    so run() should have exited.
                //    But if data is noisy, CIG might have been cancelled at 0.25*nbeatscig
                //    on prev. call. to run(), so several beats may be already available,
                //    and no-exiting makes an attempt to process further.
                if (b_streaming && gib_stat2.empty()) { return 0; }
            }

            median_avg_t<double> noise(3);
            sliding_sum_t<int> nbad(nbeatscig /
                                    4); // CIG will be evaluated at this area only if >=25% of 0.25*nbeatscig last beats are valid
            double fR_prev = 0;
            while (true) {
                using namespace QRS_names;
                if (ind_b >= rbb.n()) {
                    if (b_streaming) { return 0; }
                    break;
                }
                if (stp._gicignew >= gi9 + 1) { break; }

                const beat_info &bi = rbb(ind_b)->v;
                if (bi.empty()) { return 0; }

                double L = myfmax(bi.Lp, bi.Lt);
                double fR = bi.f[R];
                double kv = (fR > 0.25 * fR_prev) * (1 - noise.max1() * 0.5) *
                            (1 - L); // [0..1] validity estimate for HR value
                noise.push(L);

                ecgismp gi = stp.r1.gi0src + rbb(ind_b)->k;

                if (kv >= 0.8 && bi.hr >= hrmincig) // ref. 0.7
                {
                    gib_stat2.push_back(gi);
                } // gi --> gib_stat2 --> _gib2; see _gib2 decl. in the header file

                bool b_good = false;
                if (kv >= 0.9) // ref. 0.9
                {
                    fR_prev = fR;
                    b_good = bi.hr >= hrmincig;
                    if (b_good) {
                        gib_cig.push_back(gi);
                    } // gi --> gib_cig --> _gib; see _gib decl. in the header file
                }
                nbad.push(b_good ? 0 : 1);

                bool b_eval = false;
                bool b_gap = gib_cig.size() >= 2 &&
                             (stp.r1.ts_src(gi) - stp.r1.ts_src(gib_cig[gib_cig.size() - 2]) >
                              dtms_gap_max / 1000);
                bool b_noisy = nbad.avg_f() >= 0.25;

                if (b_streaming) {
                    if (gib_cig.length() >= nbeatscig) { b_eval = true; }
                } else {
                    if (ind_b == rbb.n() - 1) { b_eval = true; }
                    else {
                        if (gib_cig.length() >= nbeatscig &&
                            rbb.n() - ind_b >= (nbeatscig + nbeatsmincig) / 2) { b_eval = true; }
                    }
                }

                if (b_eval || b_gap || b_noisy) {
                    // 1. Update pending (for statistics AMSSD etc.) beats queues.
                    // 2. Evaluate the current CIG window if data is enough, and contains no large gaps.
                    // 3. Clear temp. indices, set CIG window starting point to the next window.
                    for (size_t i = 0; i < gib_cig.length(); ++i) {
                        stp._gib.push_back(gib_cig[i]);
                    }
                    for (size_t i = 0; i < gib_stat2.length(); ++i) {
                        stp._gib2.push_back(gib_stat2[i]);
                    }

                    if (b_eval && !b_gap) {
                        plan_cig cp;
                        if (cp.eval(*stp.pecg, gib_cig, stp.r1.gi0src) > 0) {
                            cp.ts0 = stp.r1.ts_src(stp._gicignew);
                            cp.ts2 = stp.r1.ts_src(gi + 1);
                            stp.shift_hcig();
                            stp.cig_last() = cp;
                        }
                    }

// NOTE If resetting nbad etc. is uncommented,
//    the algorithm produces identical results when run() called
//    without low pre-feeding (down to 1 sample)
//    in comparison to normal pre-feeding (90..120 s).
//  This is not usable, because of 10% accuracy loss in this mode.
//  Without identicity, accuracy remains constant independently on pre-feeding amount.
//          if (!b_eval) { fR_prev = 0; nbad.clear(); noise.clear(); }

                    gib_cig.clear();
                    gib_stat2.clear();
                    stp._gicignew = gi +
                                    1; // next attempt to define CIG window will start right after the current point
                }

                ++ind_b;
            }
        }

        while (stp._gi < gi9 + 1) {
            if (!stp._gib2.empty()) {
                ecgismp gi = stp._gib2.front();
                bool b_filtered = !stp._gib.empty() && gi == stp._gib.front();
                if (gi >= gi9 + 1) { break; }
                const ordhs_t<ecgismp, beat_info>::entry *e1 = rbb.find(gi - stp.r1.gi0src);
                if (!e1) { return -1; }

                bool b_gap = (stp.r1.ts_src(gi) - stp.r1.ts_src(stp._gi) > dtms_gap_max / 1000);
                if (b_gap) {
                    stp._q1sd10.clear();
                    stp._q2sdn6.clear();
                    stp._q2min.clear();
                    stp._q2amssd.clear();
                    stp._q3wi1.clear();
                    stp._q31sd10.clear();
                    stp._q31sdn6.clear();
                    stp._q31min.clear();
                    stp._q31amssd.clear();
                    stp._hr_last = 0;
                }

                stp._gib2.pop_front();
                if (b_filtered) { stp._gib.pop_front(); }
                stp._gi = gi;

                const beat_info &bi = e1->v;

                double ts_now = stp.r1.ts_src(gi);
                stp._eval_next_stat(bi.hr, ts_now, b_filtered);
            }

            if (stp._gib2.empty()) { stp._gi = gi9 + 1; }
        }

        double ts_now = stp.r1.ts_src(gi9);
        if (b_est_slpst) { stp._estimate_slpst(ts_now); }
        else { stp._notify_slpst_disabled(ts_now); }

        return 1;
    }
}

