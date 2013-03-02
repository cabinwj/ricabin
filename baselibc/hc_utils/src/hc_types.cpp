#include "hc_types.h"

#include <time.h>

#ifndef WIN32
#include <signal.h>
#else
#include <windows.h>
#endif

void init_daemon(int8_t nochdir, int8_t noclose)
{
#ifndef WIN32
    daemon(nochdir, noclose);

    //ignore signals
    signal(SIGINT,  SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}


void sleep_ms(unsigned long _ms)
{
#ifndef WIN32
    struct timespec req;
    req.tv_sec = (int)_ms/1000;
    req.tv_nsec = (_ms - req.tv_sec*1000)*1000000L;
    nanosleep(&req, NULL);
#else
    Sleep(_ms);
#endif
}

