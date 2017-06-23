#include "common.h"

struct thread_DATA_ADD_ONE : threadctl::ctx_base
{
  void _thread_proc()
  {
    unity& args = *pdata<unity>();
    t_QFEED& QFEED = args["QFEED"].ref<t_QFEED>();
    float fn_db = +args / "EcgData" / 1.;
    double r = +args / "Speed ratio" / 1.;
//      if (r <= 0.1) { r = 0.1; }
//
//    ecg_data d(250); ;
//    cout <<"DATASRC Added " << fn_db << ": ";
//         // cout<< d.load("", ecg_data::db_ECG) << endl;              add data to d( ecg_data)
//
//    if (d.ecg.empty()) { return; }
//
//    if (1)
//    {
//      t_lock_QFEED __lock; (void)__lock;
//      QFEED.push_back(feed_value(d.fd, 1)); // send sampling frequency
//    }
//
//    const size_t m = size_t(d.fd / 100 * r); // data will be sent each 10 ms (number of samples == sampling frequency / 100 * speed)
//    for (size_t i = 0; i < d.ecg.size(); i += m)
//    {
//      if (b_stop()) { break; }
//      if (1)
//      {
//        t_lock_QFEED __lock; (void)__lock;
//        for (size_t j = 0; j < m && i + j < d.ecg.size(); ++j) { QFEED.push_back(feed_value(d.ecg[i + j], 0)); } // send data sample
//      }
//      sleep_mcs(10000);
//    }

    cout <<"DATASRC Exiting." << endl;
  }
};
