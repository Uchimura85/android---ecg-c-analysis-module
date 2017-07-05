#include <iostream>
#include "common.h"

int gTimes(0);

struct thread_CLIENT : threadctl::ctx_base {
    void _thread_proc() {
        unity &args = *pdata<unity>();
        t_QOUT &QOUT = args["QOUT"].ref<t_QOUT>();

        string strSaveFileName = +args / "DB resultfilename" / "";

        cout << "Now; beat time (s); HR; P-, T-side noise; slp.st. time (s); slp.st." << endl;
        string beforeState = "";
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
            if (beat.slpst.length() > 0) { // sleep stage
                LOGE("sleepresult: %d,  %s, %s, %s, %s, %s, time_slpst:%s, sleepStage: %s", gTimes,
                     unity(d_now()).vcstr().c_str(),
                     ftocs(beat.time_beat, 6, 2).c_str(),
                     (beat.hr ? ftocs(beat.hr, 4, 1) : "N/A").c_str(), ftocs(beat.Lp, 4).c_str(),
                     ftocs(beat.Lt, 4).c_str(),
                     (beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "").c_str(),
                     beat.slpst.c_str());
//            string strForFile = "" + ftocs(gTimes) + ": " + beat.slpst.c_str() + "\n\r";

                char cpLine[1000];
//                sprintf(cpLine, "sleepresult: %d,  %s, %s, %s, %s, %s,%s, sleepStage: %s \n",
//                        gTimes,
//                        unity(d_now()).vcstr().c_str(),
//                        ftocs(beat.time_beat, 6, 2).c_str(),
//                        (beat.hr ? ftocs(beat.hr, 4, 1) : "N/A").c_str(), ftocs(beat.Lp, 4).c_str(),
//                        ftocs(beat.Lt, 4).c_str(),
//                        (beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "").c_str(),
//                        beat.slpst.c_str());
//                string befor
                if (beforeState != beat.slpst.c_str()) {
                    string time_slpst = beat.time_slpst >= 0 ? ftocs(beat.time_slpst, 6, 2) : "";
                    sprintf(cpLine, "%s, %s \n", time_slpst.c_str(), beat.slpst.c_str());
                    beforeState = beat.slpst.c_str();

                    int fres = file_io::save_bytes(strSaveFileName.c_str(), cpLine, true);
                    LOGD("savecsvsleep: file: %s, content length: %d, filesave: %d",
                         strSaveFileName.c_str(), strlen(cpLine), fres);
                }
                gTimes = 0;

            }
        }
    }
};

