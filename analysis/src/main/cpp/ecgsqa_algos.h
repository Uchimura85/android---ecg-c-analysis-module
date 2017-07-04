#ifndef yk_ecgsqa_algos_H
#define yk_ecgsqa_algos_H


#include <string>
#include <vector>
#include <queue>
#include <cctype>
#include <deque>

using namespace std;

#include "3p_bmdx/src_code/bmdx_main.h"

#include "math_utils/median.h"
#include "math_utils/polynome3.h"

#include "ecgsqa_utils.h"

// This def. enables history and stats. calc. in sleep stage estimation algorithm (struct alg_slpst_L1).
// Requires src_code/tools/tool_research/__rulestat_L1.h
//#define USE_RULESTAT

namespace ecgsqa {
    using std::basic_string;
    using std::string;
    using std::vector;

    using namespace bmdx;
    using namespace bmdx_str::conv;

    using namespace algs_median;
    using namespace algs_polynome;
    using namespace ecgsqa_utils;

#ifndef T_ECGFVSMP
    typedef float ecgfvsmp; // sample value type; NOTE for all time measurements, double is used (no typedef)
#else
    typedef T_ECGFVSMP ecgfvsmp;
#endif

    typedef long ecgismp;

    struct exc_ecg1 {
    };

    namespace QRS_names {
        enum {
            P0 = 0, P = 1, P2 = 2,
            Q0 = 3,
            Q = 4, R = 5, S = 6,
            S2 = 7,
            T0 = 8, T = 9, T2 = 10,
            sizeof_QRS_names
        };
    }

    // Positions, in samples, of QRS complex and artifacts, relative to R peak (0 means position is not calculated).
    struct beat_info {
        enum {
            nart = QRS_names::sizeof_QRS_names
        };

        // === === BEAT DATA === ===

        float inds[nart]; // sample index relative to R peak max. sample; for peaks (including R), float index may be fractional, if the peak is estimated using interpolation
        int flags; // bits [0..nart-1] tell if approp. paramter is estimated. flags == 0: no estimation made, otherwise at least R is present
        ecgfvsmp f[nart]; // signal value estimations (after filtering)

        double hr; // HR in bpm; if 0: not estimated or could not be determined (e.g. no rhythm); otherwise value >= 20

        // Noise level at the left (P-side) and right (T-side) of R peak.
        //  [0..1].
        //  0 means no noise.
        //  <0.1 -- small amount of noise, which does not influence parameters estimate.
        //  0.1..<1 -- heavy noise, beat parameters estimate may contain errors.
        //  1 -- wrong R peak (false positive) or too high interference for any estimate.
        double Lp, Lt;

        // === === === === === ===

        beat_info() { clear(); }

        // true if all parameters are set to defaults.
        bool empty() const { return flags == 0 && !hr && !Lp && !Lt; }

        void clear() {
            flags = 0;
            for (int i = 0; i < nart; ++i) {
                inds[i] = 0;
                f[i] = 0;
            }
            hr = 0;
            Lp = 0;
            Lt = 0;
        }

        // ind: one of QRS_names [0..nart)
        bool b_flag(int ind) const { return !!((flags >> ind) & 1); }

        string art_name(int ind) const { return art_name_s(ind); }

        static string art_name_s(int ind) {
            static const char *x[nart] = {"P0", "P", "P2", "Q0", "Q", "R", "S", "S2", "T0", "T",
                                          "T2"};
            return ind >= 0 && ind < nart ? x[ind] : "?";
        }

        string plstr() const;

        int pl_set(const unity &anns);
    };

    // Time window information
    struct sleep_info {
        ecgismp nw; // number os samples in window
        int istage; // numeric sleep stage index: W, 1, 2, 3, 4, R, ? (undefined) <---> -1, 1, 2, 3, 4, 0, -2
        bool mt; // MT state
        vector<string> addann; // additional annotations (strings)

        sleep_info() : nw(0), istage(-2), mt(false) {}

        bool empty() const { return nw == 0 && istage == -2 && !mt; }

        void clear() { *this = sleep_info(); }

        string str_stage() const {
            if (istage == -1) { return "W"; }
            if (istage >= 1 && istage <= 4) { return ntocs(istage); }
            if (istage == 0) { return "R"; }
            return "?";
        }

        string plstr() const;

        int pl_set(const unity &anns);
    };

    // Main ECG / respiration / time windows / annotations container.
    struct ecg_data {
        double fd; // sampling rate
        basic_string<ecgfvsmp> ecg; // single-lead ECG values, mV
        basic_string<ecgfvsmp> resp; // respiration values, litres
        basic_string<ecgfvsmp> eeg; // single-lead EEG values, mV
        basic_string<ecgfvsmp> bp; // blood pressure, mmHg
        ordhs_t<ecgismp, beat_info> beats; // { R peak position, QRS and artifacts info }
        ordhs_t<ecgismp, sleep_info> slpst; // sleep stage evaluations in time windows { starting pos, window info } - may overlap

        ecg_data(double fd_) : fd(fd_) {}

        enum {
            db_ECG = 0x1,
            db_Resp = 0x8,
            db_EEG = 0x2,
            db_BP = 0x4,
            dba_beats = 0x10,
            dba_beats_eval = 0x20, // dba_beats: R positions only; dba_beats_eval = R positions + HR,P,Q,S,T,noise etc. evaluations (beat_info)
            dba_slpst = 0x40,
            __db_last = 0x40000000,
            db_all = (((__db_last | 1) - 2) << 1) | 1,
            db_arrays = 0xf
        };

        void clear(int db_sections);

        bool save(const string &fnp, int db_sections, bool b_append);

        bool load(const string &fnp, int db_sections);

        bool loadFromEcgFile(const string &fnp, int sec);

        ecgismp nmax(int db_sections) const;

        static void append_text(string &s, const string &ann, const char *delim = "; ") {
            if (ann.empty()) { return; }
            if (s.empty()) { s = ann; }
            else {
                if (delim) { s += delim; }
                s += ann;
            }
        }

        void trim(ecgismp i_start,
                  ecgismp i_end = -1); // i_start will be limited to >=0; negative i_end == all up to end
        template<class T>
        void shift_keys(ordhs_t<ecgismp, T> &h, ecgismp delta) {
            if (delta == 0 || h.n() == 0) { return; }
            s_long i0 = delta > 0 ? h.n() - 1 : 0;
            s_long i2 = delta > 0 ? -1 : h.n();
            s_long di = delta > 0 ? -1 : 1;
            for (s_long i = i0; i != i2; i += di) {
                ecgismp k = h(i)->k;
                const T &v = h(i)->v;
                h[k + delta] = v;
                h.remove(k);
            }
        }
    };

    // Data resampling + finding exact time pos for peaks.
    struct resampler {
        ecgismp gi0src; // global number of the starting sample (all ecg_data :: ecg, resp, beats etc. are 0-based but implied to be gi0-based)
        const double fsrc; // same as ecg_data :: fd
        const double fdest; // required destination sampling frequency
        const bool b_resample; // fsrc == fdest
        const bool b_down; // fdest < fsrc
        const double rsd; // fsrc / fdest
        const double rds; // fdest / fsrc

        const basic_string<ecgfvsmp> *pecg;

        resampler() : gi0src(0), fsrc(100), fdest(100), b_resample(1), b_down(0), rsd(1), rds(1),
                      pecg(0) {}

        resampler &operator=(const resampler &x) {
            new(this) resampler(x);
            return *this;
        }

        resampler(double fsrc_, double fdest_, const basic_string<ecgfvsmp> *pecg_ = 0,
                  ecgismp gi0src_ = 0)
                : gi0src(gi0src_), fsrc(fsrc_), fdest(fdest_), b_resample(fsrc != fdest),
                  b_down(fdest < fsrc), rsd(fsrc / fdest), rds(fdest / fsrc), pecg(pecg_) {}

        // The nearest source sample index right before or exactly at idest.
        //  If the new value is not valid, the default value is returned.
        ecgismp gisrc_le(ecgismp gidest, ecgismp dflt = -1) const {
            double fi = floor(gidest * rsd);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        // The nearest source sample index right after or exactly at idest.
        //  If the new value is not valid, the default value is returned.
        ecgismp gisrc_ge(ecgismp gidest, ecgismp dflt = -1) const {
            double fi = ceil(gidest * rsd);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        // The nearest destination sample index right before or exactly at isrc.
        //  If the new value is not valid, the default value is returned.
        ecgismp gidest_le(ecgismp gisrc, ecgismp dflt = -1) const {
            double fi = floor(gisrc * rds);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        // The nearest destination sample index right after or exactly at isrc.
        //  If the new value is not valid, the default value is returned.
        ecgismp gidest_ge(ecgismp gisrc, ecgismp dflt = -1) const {
            double fi = ceil(gisrc * rds);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        // Conversion from time (s) into sample index.
        ecgismp gisrc_le_f(double ts, ecgismp dflt = -1) const {
            double fi = floor(ts * fsrc);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        ecgismp gisrc_ge_f(double ts, ecgismp dflt = -1) const {
            double fi = ceil(ts * fsrc);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        ecgismp gidest_le_f(double ts, ecgismp dflt = -1) const {
            double fi = floor(ts * fdest);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        ecgismp gidest_ge_f(double ts, ecgismp dflt = -1) const {
            double fi = ceil(ts * fdest);
            ecgismp i = ecgismp(fi);
            if (i == fi) { return i; }
            return dflt;
        }

        // Source sample time in seconds.
        double ts_src(ecgismp gisrc) const { return double(gisrc) / fsrc; }

        // Destination sample time in seconds.
        double ts_dest(ecgismp gidest) const { return double(gidest) / fdest; }

        // Min. destination sample index to Test with interpolation (v_interp),
        //  if data is starting from gi0src.
        // NOTE If resampling is not used (fsrc == fdest) and b_max == false,
        //  the function returns simply gi0src. v_interp will return exactly source samples.
        ecgismp interp_gidest_min(bool b_max, ecgismp dflt = -1) const;

        // Downsampled sample value (cubic interpolation or direct if no resampling).
        //    If b_max == true, finds maximum point within the range (-1..+1) sample
        //    from the specified (gidest).
        //  On b_strict == false, the nearest sample value is returned, for any b_max flag value.
        // Returns:
        //    2 - frequencies are equal, no max finding, ret_pv, ret_pts - sample value and time (s) at gidest.
        //    1 - resampled (or max. found if asked) successfully, ret_pv, ret_pts - resampled value and its exact time point (s).
        //    0 - pecg == 0 or string contains too little data (less than 4 samples around gidest).
        //    -1 - gi is beyond or at the left of the range, covered by *pecg.
        //    -2 - gi is beyond or at the right of the range, covered by *pecg.
        //    -3 - failed to interpolate (math. error). Only on b_strict == true.
        int v_interp(ecgismp gidest, ecgfvsmp *ret_pv = 0, double *ret_pts = 0, bool b_max = false,
                     bool b_strict = false) const;

        // In range [gi1src..gi2src), finds index of source sample withmax. absolute value.
        //  If no samples available, returns dflt.
        ecgismp v_max_abs_ge(ecgismp gi1src, ecgismp gi2src, ecgismp dflt);
    };


    // Original SQRS implementation. Prototype is taken from:
    //  http://physionet.org/physiotools/wfdb/examples/example10.c
    struct alg_sqrs {
        resampler r1;
        ordhs_t<ecgismp, beat_info> *const pbeats; // output container, will be cleared before use
    private:
        struct _sqrsretval {
            bool b_R() const { return !!(flags & 1); } // normal R peak
            bool b_A() const { return !!(flags & 2); } // unrecognized artifact
            bool b_any() const { return !!(flags & 3); } // R or A
            int flags;
            ecgismp gi; // peak pos.
            _sqrsretval() : flags(0), gi(0) {}

            _sqrsretval(bool bp_, bool ba_, ecgismp gi_) : flags(ba_ * 2 + bp_), gi(gi_) {}
        };

        struct _sqrsdata {
            ecgismp time, sign, nslope, qtime, maxtime, ms160, ms200, ms2000;
            ecgfvsmp maxslope, slopecrit, scmax, scmin, f[10];
            bool b_f; // maxslope: global max abs filter value

            // The algorithm works at fixed sampling rate.
            double fdest() const { return 250; }

            _sqrsdata();

            void reset() { *this = _sqrsdata(); }

            // Samples are processed sequentially.
            //  The current sample index (gi_) may Test from any value, but must increase by 1 on each step.
            _sqrsretval sqrs_step(ecgismp gi_, ecgfvsmp f_);
        };

        int _state;
        ecgismp _gidest;
        _sqrsdata _d;
    public:

        // 0 - not initialized,
        //  1 - running (the only working state, set by reset() and constructor),
        //  2 - the last run has been completed.
        int state() const { return _state; }

        // Destination index of the current algorithm data input.
        //  On resampling, it does not correspond to data array,
        //  only source data is interpolated around that time point.
        ecgismp gidest() const { return _gidest; }

        // Starting source index of the current algorithm data input.
        //    Min. data input (pre-feed) window size to continue is 20 ms
        //    (more exactly, 5 samples / min(fsrc, 250 smp/s)).
        ecgismp gisrc() const {
            return r1.b_down ? r1.gisrc_le(_gidest - 1) : r1.gisrc_le(_gidest) - 1;
        }

        // "Global" index of ecg sample 0, common to all algorithms run in stack.
        // The index is set on algorithm construction (typically to 0),
        //    and then updated (grows) each time the client deleted unnecessary (already processed) portion
        //    from ecg containers (see ecg_data::trim).
        // Updating index value: see notify_trimmed().
        // NOTE Normally, gi0src() <= min(min_gisrc()) of all algorithms run as stack.
        ecgismp gi0src() const { return r1.gi0src; }

        // 1. Returns min. index in source data, whose sample must be available.
        //    The index starts from gi0src() and grows with each algorithm run.
        //    Samples below min_gisrc() may be removed, and then notify_trimmed() called
        //    to set the new factually available sample index.
        //
        // 2. At the same time, min_gisrc() is the minimum sample index
        //    that may be returned by the algorithm as peak position,
        //    i.e. decision lag time is limited by this value.
        //
        //    WARNING The original SQRS algorithm lag (gisrc() - min_gisrc())
        //      is not limited when the input data is noisy.
        //
        // NOTE The client only is responsible for ensuring to keep samples at >= min_gisrc() available.
        ecgismp min_gisrc() const;

        alg_sqrs(double fsrc_, const basic_string<ecgfvsmp> &ecg_,
                 ordhs_t<ecgismp, beat_info> &beats_, ecgismp gi0src_ = 0)
                : r1(fsrc_, 250, &ecg_, gi0src_), pbeats(&beats_) { reset(gi0src_); }

        void reset(ecgismp gi0src_) {
            r1.gi0src = gi0src_;
            _gidest = r1.interp_gidest_min(false);
            _d.reset();
            _state = 1;
        }


        // b_final == false for multiple runs.
        //  b_final == true at the last (or single) run, to exhaust internal queues and complete pending calculations.
        //  After the last run, state() becomes 2, and reset() is necessary to Test new session.
        // Returns:
        //    1 - on_calm_result. In queued processing: if the run was not final,
        //        all samples up to and not including min_gisrc() may be removed from ECG beginning,
        //        and next portion of samples can be added.
        //    0 - OK, but no job done - not enough data. In queued processing: if the run was not final, add more ECG input data,
        //        to continue at the current position.
        //    -1 - the current position is invalid (before actual input data).
        //        May occur if too much data removed on queued processing. See min_gisrc().
        //    -3 - state() != 1 (the algorithm is not in running state).
        int run(bool b_final);

        // In sequential mode of operation, min_gisrc() - gi0src() or less samples may be removed from ecg.
        //    notify_trimmed updates the global index of 0-th sample in ecg containers.
        //  For stack of several algorithms, running on top of each other,
        //  the number of samples that may be removed after each top-level alg. run,
        //    == min(each alg. min_gisrc()) - any alg. gi0src().
        //  It is expected that notify_trimmed() is called at the same time
        //    and with same argument (e.g. min(min_gisrc())) for all algorithms in stack.
        void notify_trimmed(ecgismp gi0src_new) { r1.gi0src = gi0src_new; }
    };


    // Fast noise detection algorithm.
    //  Works on sequentially supplied data.
    //  Principle: at the end of each positive and negative slope of input signal,
    //    the detector calculates steepness S = abs(delta_f) / pow(nints(), 2).
    //    If S >= threshold, signal is regarded as containing HF noise
    //      or pulse interference peak at this position.
    //  Detection algorithm lag: "the current position" is exactly 1 sample back from the supplied data.
    //  Noise detection lag: client-specified nimax() value.
    //  (Total lag == 1 + nimax().)
    // 1) bse(), bN(), S(), ifeed(), islope(), nints().
    //    -- bse() true: slope end (or split point) detected at the current position.
    //            false: inside flat area or a slope (all other functions return default values).
    //    -- bN() true: slope end with S >= threshold detected at the current position,
    //            false: slope has not ended yet or S < threshold.
    //    -- S() >= 0: steepness. May be > 0 only where bse() == true.
    //        (For each detected peak, S() becomes positive only once - at peak end.)
    //    -- ifeed(): index of the next sample that must be fed into filter.
    //    -- islope(): index of the first sample in the currrent slope.
    //        islope() == ifeed() - 2 - nints(). This is >= ifeed() - nimax() - 2.
    //    -- nints(): number of sample intervals in the currrently detected slope or flat area.
    //        bse() true: nints() == last slope sample - first slope sample == [1 ..  nimax()].
    //        bse() false: nints() == 0.
    // 2) Lw():
    //    noise level in the sliding window,
    //    Lw = nN / nw == [0..1].
    //      nN -- number of noise samples currently in the window.
    //      nw -- window width, in samples.
    //    NOTE Lw == 0 may be treated specially, as "the current window contains clean signal",
    //      which is suitable for running noise-vulnerable algorithms like SQRS.
    //    NOTE For single interference peak, Lw > 0 during nw samples.
    //    NOTE If the window is not specified (nw == 0), Lw == S exactly.
    // 3) Configuration:
    //    -- S0() -- noise threshold.
    //    -- nimax() -- max. slope length, in samples.
    //        If the slope is longer, it is split in chunks nimax() long.
    //        Guideline to choose nimax: due to 1/n^2 dependence,
    //        slopes longer than sqrt(abs(delta_f) / S0)
    //        won't be considered as noise.
    //    -- nw() -- size (in samples) of the sliding window for noise level calculation.
    //        The window spans [ifeed() - 1 - nw(), ifeed() - 2] range.
    template<class Val = double, class Ind = long> // Val must be signed numeric type, Ind must be a signed integer type
    struct alg_fnd_t {
        typedef Val t_val;
        typedef Ind t_ind;

        alg_fnd_t(t_ind ifeed0_, double S0_, t_ind nimax_, t_ind nw_ = 0) {
            _bs = false;
            _s = 0;
            _i = ifeed0_;
            _nints = -2;
            _nN = 0;
            _s0 = S0_;
            _sum = 0;
            _x1 = 0;
            _x2 = 0;
            _nim = nimax_;
            if (_nim < 1) { _nim = 1; }
            _nw = nw_;
            if (_nw < 0) { _nw = 0; }
            if (_nw) { _rw.create0(false); }
        }

        void feed(t_val x) {
            ++_i;
            if (_nints <= -2) {
                _x1 = _x2 = x;
                _nints = -1;
                return;
            }
            if (_nints == -1) {
                _x1 = _x2;
                _x2 = x;
                _nints = 0;
                return;
            }
            if (_bs) {
                _bs = false;
                _nints = 0;
                _sum = 0;
            }
            ++_nints;
            _sum += std::fabs(_x2 - _x1);
            int sign1 = cmp(_x2, _x1);
            int sign2 = cmp(x, _x2);
            if (sign2 != sign1 || _nints >= _nim) {
                _bs = true;
                _s = _sum / _nints / _nints;
            }
            _x1 = _x2;
            _x2 = x;
            if (_nw) {
                try {
                    bool bn = _bs && _s >= _s0;
                    _w().push(bn);
                    if (bn) { ++_nN; }
                    while (t_ind(_w().size()) > _nw) {
                        bn = _w().front();
                        _w().pop();
                        if (bn) { --_nN; }
                    }
                } catch (...) {}
            }
        }


        bool bse() const { return _bs; } // slope end detected ?
        bool bN() const {
            return _bs && _s >= _s0;
        } // is the current slope a noise ? (may be true at slope end only)
        double S() const { return _bs ? _s : 0; } // steepness (may be > 0 at slope end only)

        t_ind ifeed() const { return _i; } // which sample to feed next
        t_ind islope() const { return _i - _nints - 2; } // starting sample of the current slope
        t_ind
        nints() const { return _nints; } // <= 0 until 3 samples fed; after: >= 1 (any slope has at least 1 interval)

        double Lw() const {
            if (_nw) { return double(_nN) / _nw; }
            return S();
        } // current noise level ? (continuous function)

        double S0() const { return _s0; }

        t_ind nimax() const { return _nim; }

        t_ind nw() const { return _nw; }


        // 1, 0, -1 -- >, = , <
        static int cmp(t_val a, t_val b) { return a > b ? 1 : (a == b ? 0 : -1); }

    private:

        bool _bs;
        double _s;
        t_ind _i, _nints, _nN; // _ns starts from -2. After 3 samples initially fed, it is always >= 1.
        double _s0;
        double _sum;
        t_val _x1, _x2; // _x2 - last fed, _x1 - right before _x2
        t_ind _nim, _nw;
        cref_t<std::queue<bool> > _rw; // no obj. if _nw <= 0
        std::queue<bool> &_w() { return *_rw._pnonc_u(); }
    };







    // Modified SQRS algorithm:
    //  1. Tuned coefficients for thresholds and limits + several changes in conditions.
    //  2. Exact peak pos. calculation.
    //    Original SQRS peak pos. estimate is 1 or more samples before real maximum.
    //    Also, on resampling, source sample index nearest to the peak maximum may be near,
    //      but not exactly the recalculated pos. of sample got from SQRS.
    //  3. Noise cancellation.
    //    Original SQRS is not perfectly stable to high-frequency noise and short interference peaks.
    //    This leads to few false positives (when single short pulse occurs),
    //      and (multiple) false negatives (when an amount of high-level noise screens ECG signal
    //      and SQRS corrects is thresholds to wrong large value).
    //    In the modified version, SQRS is stopped each time a noise window begins, and restarted after it has passed.
    //  4. Original SQRS has indefinitely large lag for returning QRS-like artifact position,
    //      since the artifact window is prolonged by 160 or 200 ms each time when signal slope occurs.
    //    The modified version limits the artifact window length even if it's not recognized as noise.

    struct alg_sqrs_nc {
        deque<double> arr_RRI;
        resampler r1;
        ordhs_t<ecgismp, beat_info> *const pbeats; // output container, will be cleared before use
    private:

        struct _sqrsretval {
            bool b_R() const { return !!(flags & 1); } // normal R peak
            bool b_A() const { return !!(flags & 2); } // unrecognized artifact
            bool b_any() const { return !!(flags & 3); } // R or A
            int flags;
            ecgismp gi; // peak pos.
            _sqrsretval() : flags(0), gi(0) {}

            _sqrsretval(bool bp_, bool ba_, ecgismp gi_) : flags(ba_ * 2 + bp_), gi(gi_) {}
        };

        struct _sqrsdata {
            ecgismp time, sign, nslope, qtime, maxtime, ms160, ms200, ms2000;
            ecgfvsmp maxslope, slopecrit, scmax, scmin, f[10];
            bool b_f, b_nslope_a; // maxslope: global max abs filter value

            // The algorithm works at fixed sampling rate.
            double fdest() const { return 250; }

            _sqrsdata();

            void reset() { *this = _sqrsdata(); }

            // Samples are processed sequentially.
            //  The current sample index (gi_) may Test from any value, but must increase by 1 on each step.
            _sqrsretval sqrs_step(ecgismp gi_, ecgfvsmp f_);
        };

        // Noise cancellation data and functions. Used by run().
        struct _nc_control {
// best for slp01a
//      enum { cnahead = 14, cnintsl = 3, cnnw = 2 };
            enum {
                cnahead = 14, cnintsl = 3, cnnw = 6
            }; // all back from ifeed, must cnintsl + 6 <= cnahead
            static double S0() { return 0.3; }

            typedef alg_fnd_t<ecgfvsmp, ecgismp> t_nd;

            static t_nd nd0(ecgismp gi0dest_) { return t_nd(gi0dest_, S0(), cnintsl, cnnw); }

            struct range {
                ecgismp i1, i2; // [i1..i2)
                ecgfvsmp f1, f2;

                bool is_empty() const { return i2 <= i1; }

                range() : i1(0), i2(0), f1(0), f2(0) {}

                range(ecgismp i1_, ecgismp i2_) : i1(i1_), i2(i2_), f1(0), f2(0) {}

                ecgfvsmp get_approx(ecgismp i, ecgfvsmp xfact) const;
            };

            struct nwinfo {
                int pttype; // 0 - signal beginning or noise window end, no noise up to the next pos., 1 - noise window beginning, 2 - noise window continuation
                ecgfvsmp f; // real signal value at this point
                nwinfo() : pttype(0), f(0) {}

                nwinfo(int pttype_, ecgfvsmp f_) : pttype(pttype_), f(f_) {}
            };

            ordhs_t<ecgismp, nwinfo> ecg_noise;
            t_nd nd; // noise detector
            basic_string<ecgfvsmp> qfeed; // prev. signal values to set for noise window slope beginning, when it's found
            bool b_nw; // the last feed sample state (noise or not)

            _nc_control(ecgismp gi0dest_) : nd(nd0(gi0dest_)), b_nw(false) {}

            ecgismp
            ndest_ahead() { return cnahead; } // number of samples at SQRS sampling frequency to queue before feeding SQRS

            bool feed(ecgismp gidest, ecgfvsmp x);

            // Deletes from ecg_noise all data earlier than (cnahead + 0.7 s SQRS lag).
            void _reduce_nwdata();

            // Returns a range, representing noise window around gidest,
            //    or empty range if gidest points to normal signal.
            range get_nw(ecgismp gidest) const;

            // Returns original sample, or an approximation if gidest falls into noise window.
            ecgfvsmp approximate(ecgismp gidest, ecgfvsmp x) {
                return get_nw(gidest).get_approx(gidest, x);
            }
        };

        int _state;
        ecgismp _gidest;
        _sqrsdata _d;
        _nc_control _nc;
        std::queue<ecgfvsmp> _ncq;
    public:

        // 0 - not initialized,
        //  1 - running (the only working state, set by reset() and constructor),
        //  2 - the last run has been completed.
        int state() const { return _state; }

        // Destination index of the current algorithm data input.
        //  On resampling, it does not correspond to data array,
        //  only source data is interpolated around that time point.
        ecgismp gidest() const { return _gidest; }

        // Starting source index of the current algorithm data input.
        //    Min. data input (pre-feed) window size to continue is 20 ms
        //    (more exactly, 5 samples / min(fsrc, 250 smp/s)).
        ecgismp gisrc() const {
            return r1.b_down ? r1.gisrc_le(_gidest - 1) : r1.gisrc_le(_gidest) - 1;
        }

        // "Global" index of ecg sample 0, common to all algorithms run in stack.
        // The index is set on algorithm construction (typically to 0),
        //    and then updated (grows) each time the client deleted unnecessary (already processed) portion
        //    from ecg containers (see ecg_data::trim).
        // Updating index value: see notify_trimmed().
        // NOTE Normally, gi0src() <= min(min_gisrc()) of all algorithms run as stack.
        ecgismp gi0src() const { return r1.gi0src; }

        // 1. Returns min. index in source data, whose sample must be available.
        //    The index starts from gi0src() and grows with each algorithm run.
        //    Samples below min_gisrc() may be removed, and then notify_trimmed() called
        //    to set the new starting index value.
        // 2. At the same time, min_gisrc() is the minimum sample index
        //    that may be returned by the algorithm as peak position,
        //    i.e. decision lag time is limited by this value.
        //
        //    In the modified SQRS algorithm, gisrc() - min_gisrc() < 700 ms (4 slopes * 160 ms).
        //
        // NOTE The client only is responsible for ensuring to keep samples at >= min_gisrc() available.
        ecgismp min_gisrc() const;

        alg_sqrs_nc(double fsrc_, const basic_string<ecgfvsmp> &ecg_,
                    ordhs_t<ecgismp, beat_info> &beats_, ecgismp gi0src_ = 0)
                : r1(fsrc_, 250, &ecg_, 0), pbeats(&beats_), _nc(0) { reset(gi0src_); }

        void reset(ecgismp gi0src_) {
            r1.gi0src = gi0src_;
            _gidest = r1.interp_gidest_min(false);
            _d.reset();
            _nc = _nc_control(_gidest);
            _ncq = std::queue<ecgfvsmp>();
            _state = 1;
        }

        // run_m() has features 1, 2, 4, noise cancellation is not used. See near struct alg_sqrs_nc.
        // b_final == false for multiple runs.
        //  b_final == true at the last (or single) run, to exhaust internal queues and complete pending calculations.
        //  After the last run, state() becomes 2, and reset() is necessary to Test new session.
        // Returns:
        //    1 - on_calm_result. In queued processing: if the run was not final,
        //        all samples up to and not including min_gisrc() may be removed from ECG beginning,
        //        and next portion of samples can be added.
        //    0 - OK, but no job done - not enough data. In queued processing: if the run was not final, add more ECG input data,
        //        to continue at the current position.
        //    -1 - the current position is invalid (before actual input data).
        //        May occur if too much data removed on queued processing. See min_gisrc().
        //    -3 - state() != 1 (the algorithm is not in running state).
        int run_m(bool b_final);

        // run() has all features 1..4. See near struct alg_sqrs_nc.
        int run(bool b_final);

        // In sequential mode of operation, min_gisrc() - gi0src() or less samples may be removed from ecg.
        //    notify_trimmed updates the global index of 0-th sample in ecg containers.
        //  For stack of several algorithms, running on top of each other,
        //  the number of samples that may be removed after each top-level alg. run,
        //    == min(each alg. min_gisrc()) - any alg. gi0src().
        //  It is expected that notify_trimmed() is called at the same time
        //    and with same argument (e.g. min(min_gisrc())) for all algorithms in stack.
        void notify_trimmed(ecgismp gi0src_new) { r1.gi0src = gi0src_new; }
    };


    // Data container for multi-segment linear approximation.
    //  NOTE Ind may be floating point if necessary.
    template<class Val = double, class Ind = long>
    struct msla_data_t {
        typedef Val t_val;
        typedef Ind t_ind;
        ordhs_t<t_ind, t_val> points; // to insert a point: points[f] = i
        t_val f0; // dflt. value returned if no points

        msla_data_t(t_val f0_ = t_val()) { f0 = f0_; }

        // Calc. approximated value at pos x.
        t_val f(t_ind x) {
            if (points.n() == 0) { return f0; }
            if (points.n() == 1) { return points(0)->v; }
            s_long ind = -1;
            points.find(x, &ind);
            if (ind < 0) { return f0; }
            if (ind == points.n()) { ind -= 1; }
            const typename ordhs_t<t_ind, t_val>::entry *e = points(ind), *e2 = 0;
            if (e->k == x) { return e->v; }
            if (ind == 0) { e2 = points(1); }
            else {
                e2 = e;
                e = points(ind - 1);
            }
            return t_val(e->v + double(x - e->k) / (e2->k - e->k) * (e2->v - e->v));
        }
    };


    // The algorithm works on original data, without resampling.
    struct alg_pqrst {
        enum {
            cnmed = 5, _cnL = 3
        };

        resampler r1; // no resampling, but used for subsample R position finding

        // "Global" index of ecg sample 0, common to all algorithms run in stack.
        // The index is set on algorithm construction (typically to 0),
        //    and then updated (grows) each time the client deleted unnecessary (already processed) portion
        //    from ecg containers (see ecg_data::trim).
        // Updating index value: see notify_trimmed().
        // NOTE Normally, gi0src() <= min(min_gisrc()) of all algorithms run as stack.
        ecgismp gi0src() const { return r1.gi0src; }

        // Returns min. index in source data, whose sample must be available.
        ecgismp min_gisrc() const {
            if (_gir_prev < 0) { return _gi; }
            return _gir_prev;
        }

        alg_pqrst(ecg_data &dat_, ecgismp gi0src_ = 0)
                : r1(dat_.fd, dat_.fd, &dat_.ecg, gi0src_), ms1000(r1.gisrc_le_f(1)),
                  _pecg(&dat_), _gir_prev(-1), _tsr_prev(0), _gi(gi0src_), _wgi0(gi0src_), _ir(0),
                  _indb(0),
                  _fLw(0), _avg_hr(_cnL) { _pat_reset(); }

        void reset(ecg_data &dat_, ecgismp gi0src_ = 0) {
            r1 = resampler(dat_.fd, dat_.fd, &dat_.ecg, gi0src_);
            ms1000 = r1.gisrc_le_f(1);
            _pecg = &dat_;
            _gir_prev = -1;
            _tsr_prev = 0;
            _gi = gi0src_;
            _w.clear();
            _w2.clear();
            _wgi0 = gi0src_;
            _ir = 0;
            _pat_reset();
            _fLw = 0;
            _avg_hr.clear();
        }

        // Returns a copy of alg_pqrst.
        //  By dflt., the full working state copy is made.
        //  On b_include_pat == false, the median pattern (_pat) used for filtering, won't be copied to save largest amount of memory.
        //    The resulting object should be used to access filtered data w(), w2() and all state variables,
        //    but estimate1() is not fully functional until reset().
        alg_pqrst _copy_data(bool b_include_pat = true) const {
            alg_pqrst x = *this;
            if (!b_include_pat) {
                x._pat.clear();
                x._pat31 = x._pat39 = -1;
            }
            return x;
        }

        const basic_string<ecgfvsmp> &
        rw() const { return _w; } // origial signal around single beat, with DC subtracted
        const basic_string<ecgfvsmp> &
        rw2() const { return _w2; } // w with smoothing and reconstruction
        ecgismp wgi0() const { return _wgi0; } // global index of the first sample in w, w2
        ecgismp ir() const { return _ir; } // window-related index of R peak max. sample in w and w2
        s_long
        ind_beat() const { return _indb; } // the last processed beat pos. in beats collection (NOTE becomes invalid when ECG is trimmed)
        double gts(ecgismp iw) const {
            return r1.ts_src(iw + _wgi0);
        } // global time for the given window-related sample index

        ecgismp cnwpat() const {
            return 2 * ms1000;
        } // total number of samples in the median pattern (max. data window size)
        ecgismp
        cirpat() const { return ms1000; } // number of samples in median pattern before R peak (or R peak sample index in the pattern)
        ecgismp cnwpat_r() const {
            return cnwpat() - cirpat();
        } // number of samples in median pattern, starting from R peak and after

        double
        cthr_sigptn() const { return 0.1; } // threshold abs(signal - pattern) to enable correction
        double cthr_noise() const { return 0.08; } // noise detector threshold
        double
        cdc_t_ms() const { return 80; } // averaging time constant to calculate DC offset for each signal point
        double
        cdc_k() const { return 0.3; } // (< 1) total signal reduction during cdc_t_ms(), when passing through DC filter

        double
        chf_t_ms() const { return 35; } // averaging time constant to remove HF component from signal before PQRST estimation
        double chf_k() const { return 0.1; } // (< 1) total signal reduction during chf_t_ms()
        double
        chf_ptcorr() const { return 1.35; } // for P, T amplitude to compensate HF filter action (depends on chf t, k and P, T width)

        double
        cpat_join_t_ms() const { return 20; } // max. duration of normal signal between two pattern-based corrections to regard them as one

        // 1. Find next beat, calculate its window, take data, filter out DC (--> _w) and HF and peaks (--> _w2).
        //  Source data must contain either 2 beats strating from gi, or at least 1 beat + 1 s.
        //  Also, source data must Test at min_gisrc() or less.
        // 2. Calculate P, Q, R, S, T peak positions, beginnings, endings, estimate signal values based on filtered data.
        //
        //    If pbeats is specified, _pecg->beats are not used.
        //
        //    b_calc_pqst == true: calculate all P..T parameters, noise levels, HR.
        //        == false: Calculate HR, noise levels, R amp. and frac. pos. Do not calculate anything related to P, Q, S, T parameters.
        //
        //    b_streaming == false (default): the algorithm processes any beat if available,
        //        including the very last one (assumes that no more ECG data will be added).
        //      == true: processes any beat up to but NOT including the currently last one (will return 0 -- not enough data).
        //        This must be set when algorithms are run as stack on constantly added data.
        //        Lower-level peak detector (e.g. alg_sqrs_nc) has certain lag, so raw ECG data may be already available up to certain point,
        //        but beats collection still miss a beat that is factually present near or at that point.
        //
        // 3. If pret == 0, write results of all calculations into appropriate element of pbeats (or _pecg->beats).
        //    If pret != 0, write results into *pret, leaving pbeats (or _pecg->beats) constant.
        // Returns:
        //    1 - on_calm_result,
        //    0 - not enough data,
        //    -1 - the current position is invalid.
        int estimate1(beat_info *pret = 0, const ordhs_t<ecgismp, beat_info> *pbeats = 0,
                      bool b_calc_pqst = true, bool b_streaming = false);

        // In sequential mode of operation, min_gisrc() - gi0src() or less samples may be removed from ecg.
        //    notify_trimmed updates the global index of 0-th sample in ecg containers.
        //  For stack of several algorithms, running on top of each other,
        //  the number of samples that may be removed after each top-level alg. run,
        //    == min(each alg. min_gisrc()) - any alg. gisrc().
        //  It is expected that notify_trimmed() is called at the same time
        //    and with same argument (e.g. min(min_gisrc())) for all algorithms in stack.
        void notify_trimmed(ecgismp gi0src_new) { r1.gi0src = gi0src_new; }

        // Averaging, passing forward and then backward through data window.
        //  w -= avg.
        //  avg[i] = avg[prev. i] * m1 + f[i] * m2, i == 0..N(w)..0.
        //  m1 == k ^ (1000 / fd / t_ms), where k < 1 -- desired reduction during t_ms.
        //  m2 -- initial reduction, may be 1 - m1.
        static void _avg_bidi(basic_string<ecgfvsmp> &w, double m1, double m2);

        // DC filter for QRS beat that uses bidirectional averaging.
        //  Averaging ignores -25/+40 ms around R peak, to center more correctly.
        //  ir -- R peak pos. (max. sample).
        //  k < 1 -- desired reduction during t_ms (milliseconds), fd - sampling freq. nsmp/s.
        //  See also _avg_bidi.
        static void
        _qrs_centering(basic_string<ecgfvsmp> &w, ecgismp ir, double k, double t_ms, double fd,
                       basic_string<ecgfvsmp> *pavg = 0);

    private:
        void _pat_reset();

        ecgismp ms1000;
        ecg_data *_pecg;

        ecgismp _gir_prev; // previous R-peak position, negative if no prev. beat
        double _tsr_prev; // previous R-peak position in seconds

        // Source data position, advanced by alg_pqrst automatically after each successive beat processing.
        ecgismp _gi;

        basic_string<ecgfvsmp> _w, _w2; // QRS data window (_w: with DC offset removed, _w2: with DC offset removed + pattern correction + HF smoothing
        ecgismp _wgi0; // global index of the first window sample
        ecgismp _ir; // window-related index of R peak max. sample
        s_long _indb; // the last processed beat pos. in beats collection

        vector<median_avg_t<ecgfvsmp> > _pat;
        ecgismp _pat31, _pat39;

        s_long _fLw; // 2 * _cnL bits, noise status of _cnL prev. beats
        median_avg_t<double> _avg_hr;
    };


    // Sleep stages estimation, based on rules.
    //  Numeric coefficients: nested struct alg_slpst_L1::state_L1::ruleset_coefs.
    //  Rules definition: see alg_slpst_L1::state_L1::_estimate_slpst().
    //  NOTE Rules are not only trivial logic (> < == != && || !).
    //    The algorithm proceeds beat by beat,
    //    and at each beat several parameters are evaluated and decisions are made.
    //  NOTE The algorithm may be taught to better match particular set of ECGs
    //    without rebuilding the program.
    //    Names and (by-ref.) values of numeric coefficients for rules
    //    are accessible by ordinal number (alg_slpst_L1::  stp._coefs.pn(int i), stp._coefs.coefs(int i)).
    //    The client program may run any kind of optimizer on the coefficients.
    //    Ranges for coefs. change should be determined once manually,
    //    based on 1) rules where they are used
    //    + 2) sleep stages statistics gathered when running with original coef. values.
    struct alg_slpst_L1 {
        enum {
            nhcig = 4, // number of CIG plans in history (at least 4) (valuable for learning algorithms)

            nbeatscig = 80, // working n beats for CIG (80..300 or more, optimal 100)
            dtmscig = 50, // optimal CIG mode interval, ms (do not change)
            nbeatsmincig = 30, // min min n beats for CIG (do not change)
            hrmincig = 20, // min min HR (do not change)

            dtms_slpst_lost = 120000, // period of state loss if sleep stage estimate is not updated (phase becomes undefined)
            dtms_gap_max = 40000 // max. period with no valid data, after which statistical algorithms state should be reset
        };

        // Data plan for cardiointervalography (method description in Russian: http://www.vestar.ru/article.jsp?id=1267).
        struct plan_cig {
            double ts0, ts2; // data time range in seconds, [ts0 .. ts2)
            double xin, xvb, xdts;

            plan_cig() { clear(); }

            bool empty() const { return !ts0 && !ts2 && !xin && !xvb && !xdts; }

            bool dts() const { return ts2 - ts0; }

            void clear() {
                ts0 = 0;
                ts2 = 0;
                xin = 0, xvb = 0, xdts = 0;
            }

            // 1 - on_calm_result.
            // 0 - not enough data (some of gibeats not found in ecg or HR < hrmincig). Min. gibeats.length() is 30, really working is >=100 or at least 80.
            // -1 - evaluation failed (bad input data).
            int eval(ecg_data &dat, const basic_string<ecgismp> &gibeats, ecgismp gi0src);
        };

        // Main data plan for sleep stages estimation.
        struct plan_slpst {
            plan_cig hcig[nhcig]; // statistical parameters
            double sd10, amssd, wi1; // statistical parameters
            // phe -- estimated sleep phase (stage):
            //    -2 -- undefined or not evaluated,
            //    -1 -- W (wake),
            //    0 -- R (REM sleep),
            //    1, 2 -- slow sleep stage 1, 2,
            //    3, 4 -- delta-sleep (sleep stage 3, 4).
            int phe;
            int phd; // dominant phase, re-fed into solver at each step; phe may differ from phd temporarily
            int naf; // "normal fall asleep" mode (1 - from W, 2 - from R, 0 - off)
            double ts, tu0s, tphd0s; // time points (s): last call of any evaluation, last phase update (for timeout checks), last change of dominant phase (for estimates)
            double dts_phd() const {
                return ts - tphd0s;
            } // duration (s) of the current dominant phase

            plan_slpst() { clear(); }

            void clear() {
                for (int i = 0; i < nhcig; ++i) { hcig[i].clear(); }
                sd10 = 0;
                amssd = 0;
                wi1 = 0;
                phe = -2;
                phd = -2;
                naf = 0;
                ts = 0;
                tphd0s = 0;
                tu0s = 0;
            }

            // true if phd equals to any of the given phases.
            bool test_phd(int ph1, int ph2 = -10, int ph3 = -10, int ph4 = -10, int ph5 = -10,
                          int ph6 = -10) const;

            // Sets new phase values and updates time point.
            void set_ph(int ph);

            // Shifts cig history back by 1 element.
            void shift_hcig() {
                for (int i = 1; i < nhcig; ++i) { hcig[i - 1] = hcig[i]; }
                hcig[nhcig - 1].clear();
            }

            // Last historical dataset of cardiointervalography.
            //  NOTE The current estimation point (ts) may be beyond its time range by number of reasons.
            plan_cig &cig_last() { return hcig[nhcig - 1]; }

            const plan_cig &cig_last() const { return hcig[nhcig - 1]; }

            // Returns CIG interval, nearest to ts point.
            //  If all intervals are empty, returns reference to the last in history.
            const plan_cig &cig_nearest() const;

            // Returns CIG interval, corresponding to ts point, or 0 if the point is beyond any of the intervals in history.
            //  If ret. 0 -- no cig data in history at this point
            const plan_cig *pcig_fact() const;
        };

        struct state_L1 : plan_slpst {
            struct _ruleset_coefs_base {
                double
                        k1, k21, k22, k23, k24, k25, k26, k27, k28, k31, k32, k33, k34, k4, k5, k52, k53, k6, k7, k8, k9, k10,
                        q31, q32, q5, q52, q53, q7, q9, q10, q11, q12, q13, q14, q15, q16, q17, q18,
                        j11, j12, j13, j14, j15, j31, j32, j4, j41;
            };

            struct ruleset_coefs : _ruleset_coefs_base {
                ruleset_coefs() {
                    k1 = 3.5;
                    k21 = 2.5;
                    k22 = 1.3;
                    k23 = 0.7;
                    k24 = 1;
                    k25 = 2.5;
                    k26 = 3;
                    k27 = 1.7;
                    k28 = 1;
                    k31 = 0.5;
                    k32 = 2;
                    k33 = 3;
                    k34 = 4;
                    k4 = 0.5;
                    k5 = 1;
                    k52 = 1.5;
                    k53 = 2;
                    k6 = 315;
                    k7 = 0.6;
                    k8 = 200;
                    k9 = 0.8;
                    k10 = 400;
                    q31 = 0.3;
                    q32 = 1;
                    q5 = 2;
                    q52 = 1.3;
                    q53 = 1;
                    q7 = 1.7;
                    q9 = 0.7;
                    q10 = 0.6;
                    q11 = 1.2;
                    q12 = 5.7;
                    q13 = 1;
                    q14 = 2;
                    q15 = 0.9;
                    q16 = 0.01;
                    q17 = 0.7;
                    q18 = 1;
                    j11 = 0.8;
                    j12 = 1.1;
                    j13 = 0.7;
                    j14 = 2.4;
                    j15 = 3;
                    j31 = 0.75;
                    j32 = 1.1;
                    j4 = 0.4;
                    j41 = 0.7;
                }

                static const char *pn(int i) {
                    static const char *x[] = {
                            "[k1]", "[k21]", "[k22]", "[k23]", "[k24]", "[k25]", "[k26]", "[k27]",
                            "[k28]", "[k31]", "[k32]", "[k33]", "[k34]", "[k4]", "[k5]", "[k52]",
                            "[k53]", "[k6]", "[k7]", "[k8]", "[k9]", "[k10]",
                            "[q31]", "[q32]", "[q5]", "[q52]", "[q53]", "[q7]", "[q9]", "[q10]",
                            "[q11]", "[q12]", "[q13]", "[q14]", "[q15]", "[q16]", "[q17]", "[q18]",
                            "[j11]", "[j12]", "[j13]", "[j14]", "[j15]", "[j31]", "[j32]", "[j4]",
                            "[j41]",
                            0};
                    return x[i];
                }

                double &coef(int i) const {
                    return *(double *) (((char *) this) + i * ((char *) &this->k21 -
                                                               (char *) &this->k1));
                }
            };

#ifdef USE_RULESTAT
#include "../tools/tool_research/__rulestat_L1.h"
#else

            struct rulestat {
                struct history_entry {
                    double ts;
                    string rule_name;
                    int eval_result;
                    string statement;
                    string params;

                    history_entry() : ts(0), eval_result(-1) {}
                };

                bool b_enabled;
                state_L1 *ppl;
                vector<history_entry> history;

                void operator()(const char *rule_name, bool b_result) {}

                string res_ruleset() const { return string(); }

                string res_stats() const { return string(); }

                string res_history(const string &add_heading = string()) const { return string(); }

                rulestat() : b_enabled(false), ppl(0) {}

                void clear() { history.clear(); }
            };

#endif

            ecg_data *pecg;
            resampler r1;
            ecgismp _gi; // first index for which sleep phase had not been evaluated yet
            ecgismp _gicignew; // first index for which CIG had not been evaluated yet
            std::deque<ecgismp> _gib; // pending beats for alg. processing on the next run(); for AMSSD, SD10
            std::deque<ecgismp> _gib2; // pending beats taken at lower noise level; superset of _gib; for WI1

            // q1* chain used for AMMSD
            // q2* chain - for SD10
            // q3* chain - for WI1
            sliding_sum_t<double> _q1sd10;
            sliding_sum_t<double> _q2sdn6;
            median_avg_t<double> _q2min;
            sliding_sum_t<double> _q2amssd;
            sliding_sum_t<double> _q3wi1;
            sliding_sum_t<double> _q31sd10;
            sliding_sum_t<double> _q31sdn6;
            median_avg_t<double> _q31min;
            sliding_sum_t<double> _q31amssd;
            double _hr_last;
            ruleset_coefs _coefs;
            rulestat _rls;


            // Should be called sequentially for each beat.
            //  (Beats may be skipped only if they contain invalid data.)
            //  Updates queues, sd10, amssd, wi1.
            void _eval_next_stat(double hr, double ts_now, bool b_filtered_sample);


            // At any particular point, should be called after CIG history update
            //    and _eval_next_stat() advanced up to the current beat.
            //    Updates phe, phd, naf and times.
            //  ts_now is the time point of evaluation, in seconds.
            void _estimate_slpst(double ts_now);


            // Possibly setting undefined sleep stage if sleep stage estimate had not been requested
            //    by the client during certain period.
            void _notify_slpst_disabled(double ts_now);


            state_L1()
                    : pecg(0),
                      _q1sd10(90, 60),
                      _q2sdn6(6, 0.1), _q2min(20, 0), _q2amssd(160, 0),
                      _q3wi1(100, 0),
                      _q31sd10(90, 60), _q31sdn6(6, 0.1), _q31min(20, 0),
                      _q31amssd(160, 0) { clear(); }

            void clear() {
                _gi = r1.gi0src;
                _gicignew = r1.gi0src;
                _q1sd10.clear();
                _q2sdn6.clear();
                _q2min.clear();
                _q2amssd.clear();
                _q3wi1.clear();
                _q31sd10.clear();
                _q31sdn6.clear();
                _q31min.clear();
                _q31amssd.clear();
                _hr_last = 0;
                _rls.clear();
                _rls.ppl = this;
                this->plan_slpst::clear();
            }
        };

        state_L1 stp;

        alg_slpst_L1(ecg_data &dat_, ecgismp gi0src_ = 0) { reset(dat_, gi0src_); }

        void reset(ecg_data &dat_, ecgismp gi0src_ = 0) {
            stp.clear();
            stp.pecg = &dat_;
            stp.r1 = resampler(dat_.fd, dat_.fd, &dat_.ecg, gi0src_);
            stp._gi = gi0src_;
            stp._gicignew = gi0src_;
        }

        // Run algorithm from the current position up to and including gi9.
        //    Note that ECG data must be pre-fed in excess, to surpass gi9 by >= 100 * 60 / bpmlow seconds,
        //    where bpmlow - lower HR threshold during sleep, e.g. 40.
        //    Factually, the algorithm may require pre-feeding more data than that value,
        //    if the data is noisy.
        //  b_est_slpst - estimate sleep stage at the point gi9, or only statistics.
        //    b_est_slpst == false: only statistical data is calculated, and stale historical values are reset.
        //        All valuable parameters, calculated at this time, may be passed into ML algorithm
        //          to perform sleep stage estimate without the rule-based solver.
        //        Namely, in stp:
        //          hcig[i] .xvb, .xin, .xdts;
        //          sd10, amssd.
        //    b_est_slpst == true: both statistics and sleep stage estimate for the current point are calculated.
        //        All valuable parameters, calculated at this time, may be passed into upper-level algorithm
        //          to 1) estimate sleep stage independently and 2) calculate all necessary data to correct rule-based solver state
        //          at this point.
        //        Namely, in stp:
        //          hcig[i] .xvb, .xin, .xdts;
        //          sd10, amssd;
        //          phe, naf, phd, dts_phd().
        //        What may be corrected with upper-level algorithm:
        //          phe, naf, phd, tphd0s (when phd changes).
        //        The following technical parameters should be kept as is:
        //          hcig[i] .wgi0, .wgi2; ts, tu0s.
        // NOTE plan_slpst part of stp is lightweight object,
        //    so it may be used as historical value or input into another algorithm.
        //  b_streaming == true:
        //      If no errors, each run() call a) estimates sleep stage for exactly one point (gi9).
        //      b) If not enough data, run() evaluates only part of statistics, and returns 0. Sleep stage is not estimated.
        //    b_streaming == false: even if gi9 point is close or after data end, do as much calculations
        //      as possible. Sleep stage is estimated anyway on b_est_slpst == true.
        //      run() returns 1 instead of 0 even if the last part of data was too short.
        //      Use b_streaming == false sequentially if whole dataset is already available.
        //      If no errors, each run() evaluates sleep stage for exactly one given point (gi9).
        // Returns:
        //  1 - on_calm_result, calculations done up to and including gi9.
        //  0 - OK, but no sleep stage estimated even if b_est_slpst == true, because more data must be fed in.
        //      NOTE One more possible reason of returning 0 is if any beat currently needed for processing,
        //      is not initialized (beat_info::empty() == true). The algorithm expects all beats
        //      from 0 to the last one required for estimate to be initialized.
        //      In streaming mode, such initialization (e.g. lower-level alg. run)
        //      regularly inserts new beats and fills, with certain small lag,
        //      with data so that empty() becomes false.
        //  -1 - the current position is invalid (before actual input data).
        //      May occur if too much data removed on queued processing. See min_gisrc().
        //  -2 - failure (gi9 is before the previous gi9 values, pecg == 0 etc.).
        int run(ecgismp gi9, bool b_est_slpst, bool b_streaming);


        // "Global" index of ecg sample 0, common to all algorithms run in stack.
        // The index is set on algorithm construction (typically to 0),
        //    and then updated (grows) each time the client deleted unnecessary (already processed) portion
        //    from ecg containers (see ecg_data::trim).
        // Updating index value: see notify_trimmed().
        // NOTE Normally, gi0src() <= min(min_gisrc()) of all algorithms run as stack.
        ecgismp gi0src() const { return stp.r1.gi0src; }

        // In sequential mode of operation, min_gisrc() - gi0src() or less samples may be removed from ecg (*this->pecg).
        //    notify_trimmed updates the global index of 0-th sample in ecg containers.
        //  For stack of several algorithms, running on top of each other,
        //  the number of samples that may be removed after each top-level alg. run,
        //    == min(each alg. min_gisrc()) - any alg. gi0src().
        //  It is expected that notify_trimmed() is called at the same time
        //    and with same argument (e.g. min(min_gisrc())) for all algorithms in stack.
        void notify_trimmed(ecgismp gi0src_new) { stp.r1.gi0src = gi0src_new; }

        // Returns min. index in source data, whose sample must be available.
        ecgismp min_gisrc() const { return mylmin(stp._gi, stp._gicignew); }
    };

}


#endif
