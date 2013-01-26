#ifndef _ROUTER_FRAME_H_
#define _ROUTER_FRAME_H_

#include "hc_bitmap.h"


// 配置文件
#ifdef WIN32
#define ROUTERD_CONF "etc\\routerd_config.xml"
#else
#define ROUTERD_CONF "etc/routerd_config.xml"
#endif

void sigusr1_handle(int signo);
void sigusr2_handle(int signo);

class router_frame
{
public:
    router_frame();
    virtual ~router_frame();

public:
    int start(bool is_daemon);
    void run();

public:
    // 定时操作
    void on_tick();
    // 定时统计数据
    void on_timer();

public:
    static bitmap32 m_run_status_;
};

#define CREATE_ROUTER_INSTANCE  object_singleton<router_frame>::Instance
#define GET_ROUTER_INSTANCE     object_singleton<router_frame>::Instance
#define DESTROY_ROUTER_INSTANCE object_singleton<router_frame>::Instance

#endif

