#include "common.h"

struct thread_DATASRC : threadctl::ctx_base {
    void _thread_proc() {
        unity &args = *pdata<unity>();
        t_QFEED &QFEED = args["QFEED"].ref<t_QFEED>();
        string fn_db = +args / "DB filename" / "";
        double r = +args / "Speed ratio" / 1.;
        if (r <= 0.1) { r = 0.1; }

        ecg_data d(250);;
        LOGD("DATASRC Loading ");
        cout << "DATASRC Loading " << fn_db << ": " << d.load(fn_db, ecg_data::db_ECG) << endl;
        LOGD("DATASRC load end ");
        if (d.ecg.empty()) { return; }
        LOGD("DATASRC and anaylsisi %d ", d.ecg.size());
        if (1) {
            t_lock_QFEED __lock;
            (void) __lock;
            QFEED.push_back(feed_value(d.fd, 1)); // send sampling frequency
        }

        const size_t m = size_t(d.fd / 100 *
                          r); // data will be sent each 10 ms (number of samples == sampling frequency / 100 * speed)
//        m = 7500;
        int ecgLen = d.ecg.size();
//        m = ecgLen / 100;
        LOGD("sept amount speed(r) = %f, %f", r, m);
        int j = 1;
        for (size_t i = 0; i < ecgLen; i += m, j++) {
            LOGD("sept amount speed(r) = %d, %d", r, m);
            LOGD("data input %d       %d", i, j);
            if (b_stop()) { break; }
            if (1) {
                t_lock_QFEED __lock;
                (void) __lock;
                for (size_t j = 0; j < m && i + j < d.ecg.size(); ++j) {
                    QFEED.push_back(feed_value(d.ecg[i + j], 0));
                } // send data sample
            }
            sleep_mcs(1000000); // 10000 = 10ms,1000,000 = 1s
        }
        LOGD("DATASRC Exiting ");
        cout << "DATASRC Exiting." << endl;
    }
};
