#include "connector_frame.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <signal.h>
#include <sys/file.h>
#endif

#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[])
{
    bool is_daemon = false;
#ifndef WIN32
    if (1 < argc && !strcasecmp(argv[1], "-d" ) )
    {
        is_daemon = true;
    }
    else
    {
        is_daemon = false;
    }

    int lock_fd = open((const char*)argv[0], O_RDONLY);
    if (lock_fd < 0 )
    {
        cout << "can not open: " << argv[0] << endl;
        exit(1);
    }

    if (::flock(lock_fd, LOCK_EX | LOCK_NB) < 0 )
    {
        cout << argv[0] << " was launched!" << endl;
        exit(1);
    }
#endif

    connector_frame aframe;
    int rc = aframe.start(is_daemon);
    if (0 != rc)
    {
        return -1;
    }

    aframe.run();

    return 0;
}

