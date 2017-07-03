#include <iostream>
#include "common.h"

int gTimes(0);

struct thread_CLIENT : threadctl::ctx_base {
    void _thread_proc() {
        unity &args = *pdata<unity>();
        t_QOUT &QOUT = args["QOUT"].ref<t_QOUT>();

        cout << "Now; beat time (s); HR; P-, T-side noise; slp.st. time (s); slp.st." << endl;
        while (true) {
            if (b_stop()) { break; }
            timed_beat beat;
            if (1) {
                t_lock_QOUT __lock;
                (void) __lock;
                if (QOUT.size()) {
                    beat = QOUT.front();
                    QOUT.pop_front();
                }
            }
            if (beat.empty()) {
                sleep_mcs(10000);
                continue;
            }
            cout
                    << unity(d_now()).vcstr()
                    << "\t" << ftocs(beat.time_beat, 6, 2)
                    << "\t" << (beat.hr ? ftocs(beat.hr, 4, 1) : "N/A")
                    << "\t" << ftocs(beat.Lp, 4)
                    << "\t" << ftocs(beat.Lt, 4)
                    << "\t" << (beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "")
                    << "\t" << beat.slpst
                    << endl;
            gTimes++;
            LOGE("sleepresult: %d,  %s, %s, %s, %s, %s,%s, %s", gTimes,
                 unity(d_now()).vcstr().c_str(),
                 ftocs(beat.time_beat, 6, 2).c_str(),
                 (beat.hr ? ftocs(beat.hr, 4, 1) : "N/A").c_str(), ftocs(beat.Lp, 4).c_str(),
                 ftocs(beat.Lt, 4).c_str(),
                 (beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "").c_str(),
                 beat.slpst.c_str());
        }
    }
};

