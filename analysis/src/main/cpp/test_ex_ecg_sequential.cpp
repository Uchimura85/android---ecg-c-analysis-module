#include <iostream>

using namespace std;

#include "thread_ALG.h"
#include "thread_DATASRC.h"
#include "thread_CLIENT.h"

cref_t<t_QFEED> q1_feed;
cref_t<t_QOUT> q2_out;
bool isNowAnalysis = false;

int test_ex_ecg_sequential(int argc, char **argv, bool isFileMode = false) {
    isNowAnalysis = true;

    threadctl ALG, DATASRC, CLIENT;
    try {
        if (argc < 3) {
            cerr << "ERR Supply text DB filename as 1st argument." << endl;
            return 1;
        }

        q1_feed.create0(false);
        q2_out.create0(false);

        ALG.start_auto<thread_ALG, unity>(paramline().list_m(
                "QFEED", q1_feed,
                "QOUT", q2_out
        ));
        if (1) {
            t_lock_QFEED __lock;
            (void) __lock;
            t_QFEED &QFEED = (t_QFEED &) q1_feed.ref();
            QFEED.push_back(feed_value(250, 1)); // send sampling frequency
        }

        if (isFileMode) {
            DATASRC.start_auto<thread_DATASRC, unity>(paramline().list_m(
                    "DB filename", argv[1],
                    "QFEED", q1_feed,
                    "Speed ratio", str2f(argc >= 3 ? argv[3] : "1", 1)
            ));
        }

        CLIENT.start_auto<thread_CLIENT, unity>(paramline().list_m(
                "DB resultfilename", argv[2],
                "QOUT", q2_out
        ));
        if (isFileMode) {
            int stage = 1;
            while (true) {
                if (stage == 1 && DATASRC.state() != 2) {
                    stage = 2;
                    ALG.signal_stop();
                    CLIENT.signal_stop();
                } // fed all data
                if (stage == 2 && ALG.state() != 2 &&
                    CLIENT.state() != 2) { break; } // all remaining threads exited
                sleep_mcs(10000);
            }
        } else {
            // realtime mode
            while (true) {
                if (!isNowAnalysis) {
                    ALG.signal_stop();
                    CLIENT.signal_stop();
                    break;
                }
                sleep_mcs(1000000); // check 1s(for stop)
            }
        }
        return 0;
    }
    catch (std::exception &e) { cerr << endl << "ERR " << e.what() << endl; }
    catch (...) { cerr << endl << "C++ exception" << endl; }

    ALG.signal_stop();
    DATASRC.signal_stop();
    CLIENT.signal_stop();

    return 2;
}

void addEcgSleep(MyArray arr, bool stop = false) {

    t_lock_QFEED __lock;
    (void) __lock;
    t_QFEED &QFEED = (t_QFEED &) q1_feed.ref();
    int len = arr.size();
    for (int i = 0; i < len; i++) {
        QFEED.push_back(feed_value(arr.at(i), 0));
    }
    if (stop)isNowAnalysis = true;
//    LOGD("testtesttest %d,    isNowAnalysis = %d,  stop = %d", QFEED.size(), isNowAnalysis, stop);
}