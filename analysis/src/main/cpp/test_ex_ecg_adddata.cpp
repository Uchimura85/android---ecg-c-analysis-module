#include <iostream>

using namespace std;

#include "thread_ALG.h"
#include "thread_DATA_ADD_ONE.h"
#include "thread_CLIENT.h"

int test_ex_ecg_adddata(float f_EcgData, float float_fd) {
    threadctl ALG, DATASRC, CLIENT;
    try {
        cref_t<t_QFEED> q1;
        q1.create0(false);
        cref_t<t_QOUT> q2;
        q2.create0(false);

        ALG.start_auto<thread_ALG, unity>(paramline().list_m(
                "QFEED", q1,
                "QOUT", q2
        ));

        DATASRC.start_auto<thread_DATA_ADD_ONE, unity>(paramline().list_m(
                "EcgData", f_EcgData,
                "QFEED", q1,
                "Speed ratio", float_fd > 0 ? float_fd : 1, 1
        ));

        CLIENT.start_auto<thread_CLIENT, unity>(paramline().list_m(
                "QOUT", q2
        ));

        int stage = 1;
        console_io cons;
        while (true) {
            if (cons.ugetch() == 0x1b) {
                ALG.signal_stop();
                DATASRC.signal_stop();
                CLIENT.signal_stop();
                break;
            } // ESC
            if (stage == 1 && DATASRC.state() != 2) {
                stage = 2;
                ALG.signal_stop();
                CLIENT.signal_stop();
            } // fed all data
            if (stage == 2 && ALG.state() != 2 &&
                CLIENT.state() != 2) { break; } // all remaining threads exited
            sleep_mcs(10000);
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
