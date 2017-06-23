#include <iostream>

using namespace std;

#include "thread_ALG.h"
#include "thread_DATASRC.h"
#include "thread_CLIENT.h"

int test_ex_ecg_sequential(int argc, char **argv) {
    LOGE("testtest test_ex_ecg_sequential");
    threadctl ALG, DATASRC, CLIENT;
    try {
        if (argc < 2) {
            cerr << "ERR Supply text DB filename as 1st argument." << endl;
            return 1;
        }

        cref_t<t_QFEED> q1;
        q1.create0(false);
        cref_t<t_QOUT> q2;
        q2.create0(false);

        ALG.start_auto<thread_ALG, unity>(paramline().list_m(
                "QFEED", q1,
                "QOUT", q2
        ));

        DATASRC.start_auto<thread_DATASRC, unity>(paramline().list_m(
                "DB filename", argv[1],
                "QFEED", q1,
                "Speed ratio", str2f(argc >= 3 ? argv[2] : "1", 1)
        ));

        CLIENT.start_auto<thread_CLIENT, unity>(paramline().list_m(
                "QOUT", q2
        ));

        int stage = 1;
        console_io cons;
        while (true) {
//            if (cons.ugetch() == 0x1b) {
//                ALG.signal_stop();
//                DATASRC.signal_stop();
//                CLIENT.signal_stop();
//                break;
//            } // ESC
            if (stage == 1 && DATASRC.state() != 2) {
                stage = 2;
                ALG.signal_stop();
                CLIENT.signal_stop();
            } // fed all data
            if (stage == 2 && ALG.state() != 2 &&
                CLIENT.state() != 2) { break; } // all remaining threads exited
            sleep_mcs(10000);
        }
//*/
        return 0;
    }
    catch (std::exception &e) { cerr << endl << "ERR " << e.what() << endl; }
    catch (...) { cerr << endl << "C++ exception" << endl; }

    ALG.signal_stop();
    DATASRC.signal_stop();
    CLIENT.signal_stop();

    return 2;
}
