#include "hc_base.h"

#ifndef WIN32
#include <signal.h>
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

