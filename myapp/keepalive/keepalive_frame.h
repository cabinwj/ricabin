#ifndef _KEEPALIVE_FRAME_H_
#define _KEEPALIVE_FRAME_H_

// ≈‰÷√Œƒº˛
#ifdef WIN32
#define KEEPALIVE_CONF "etc\\keeplive.json"
#else
#define KEEPALIVE_CONF "etc/keeplive.json"
#endif

class keepalive_frame
{
public:
    keepalive_frame();
    virtual ~keepalive_frame();

public:
    int start(bool is_daemon);
    void run();
};

#endif

