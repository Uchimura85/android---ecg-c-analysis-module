#ifndef thread_client_H
#define thread_client_H

#include <iostream>
#include <jni.h>
#include "common.h"
#include "thread_CALMNESS.h"

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
//            printTest();
            cout << "{time:" << unity(d_now()).vcstr()
                 << ",timebeat:" << ftocs(beat.time_beat, 6, 2)
                 << ",hr:" << (beat.hr ? ftocs(beat.hr, 4, 1) : "N/A")
                 << ",lp:" << ftocs(beat.Lp, 4)
                 << ",lt:" << ftocs(beat.Lt, 4)
                 << ",time_slpst:" << (beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "")
                 << ",slpst:" << beat.slpst
                 << "}"
                 << endl;

        }
    }
};


#endif