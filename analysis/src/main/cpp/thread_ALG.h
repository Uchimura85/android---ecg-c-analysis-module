
#include "common.h"
#include <android/log.h>
#define  LOG_TAG    "someTag"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

struct thread_ALG : threadctl::ctx_base
{
  void _thread_proc()
  {
    unity& args = *pdata<unity>();
    t_QFEED& QFEED = args["QFEED"].ref<t_QFEED>();
    t_QOUT& QOUT = args["QOUT"].ref<t_QOUT>();

    ecgismp gi9(0); // global index of the point till which alg_slpst_L1 has run
    ecg_data d(250);
    LOGD( "This is a number from JNI: %f %d", d.fd, d.ecg.length());
    alg_sqrs_nc a1(d.fd, d.ecg, d.beats);
    alg_pqrst a2(d);
    alg_slpst_L1 a3(d);

    unity stages; stages.array("?", "W", "R", "1", "2", "3", "4").arrlb_(-2);

    while (true)
    {
      if (b_stop()) { break; }

      if (1)
      {
        t_lock_QFEED __lock; (void)__lock;
        while (!QFEED.empty())
        {
          feed_value v = QFEED.front(); QFEED.pop_front();

          if (v.type == 0) // data sample
          {
            d.ecg.push_back(v.x);
          }
          else if (v.type == 1) // sampling frequency change (new dataset)
          {
            gi9 = -1;
            d = ecg_data(v.x);
            a1.~alg_sqrs_nc(); new (&a1) alg_sqrs_nc(d.fd, d.ecg, d.beats);
            a2.~alg_pqrst(); new (&a2) alg_pqrst(d);
            a3.~alg_slpst_L1(); new (&a3) alg_slpst_L1(d);
          }
        }
      }

      int res1 = 0, res2 = 0, res3 = 0;

      res1 = a1.run(false);

        if (res1 < 0) { break; }

      if (res1 >= 1)
      {
        res2 = a2.estimate1(0, &d.beats, false, true);
          if (res2 < 0) { break; }

        if (res2 >= 1) // at each beat 1) try estimate sleep stage 2) push HR info into output queue
        {
          timed_beat beat; beat_info& bi = beat; bi = d.beats(a2.ind_beat())->v;

          const int t_slp_wnd = 30 * d.fd;

          ecgismp gi = a2.wgi0() + a2.ir(); // the current beat position
          if (gi - gi9 >= 120 * d.fd) // pre-fed seconds
          {
            res3 = a3.run(gi9 + t_slp_wnd, true, true);
            if (res3 < 0) { break; }
            else if (res3 == 0) { } // still not enough data
            else  // success
            {
              gi9 += t_slp_wnd; // new time window end pos

                // Remove unnecessary old data.
              gi = mylmin(a1.min_gisrc(), a2.min_gisrc(), a3.min_gisrc());
              d.trim(gi - a1.gi0src());
              a1.notify_trimmed(gi);
              a2.notify_trimmed(gi);
              a3.notify_trimmed(gi);
            }
          }

          beat.time_beat = a2.gts(a2.ir());

          if (res3 <= 0) { beat.time_slpst = -1; beat.slpst = ""; }
          else
          {
            beat.time_slpst = a3.stp.r1.ts_src(gi9 + 1 - t_slp_wnd);
            beat.slpst = stages[a3.stp.phe].vcstr();
          }

          t_lock_QOUT __lock;
            (void)__lock;
          QOUT.push_back(beat);
        }
      }

      sleep_mcs(10000);
    }//end while

  }//func
};
