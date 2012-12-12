#ifndef _CONNECTOR_FRAME_H_
#define _CONNECTOR_FRAME_H_

#include "common_bitmap.h"

// 配置文件
#ifdef WIN32
#define CONNECTORD_CONF "etc\\connectord_config.xml"
#else
#define CONNECTORD_CONF "etc/connectord_config.xml"
#endif

void sigusr1_handle(int signo);
void sigusr2_handle(int signo);

class connector_frame
{
public:
    connector_frame();
    virtual ~connector_frame();

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

#define CREATE_CONNECTOR_INSTANCE  object_singleton<connector_frame>::Instance
#define GET_CONNECTOR_INSTANCE     object_singleton<connector_frame>::Instance
#define DESTROY_CONNECTOR_INSTANCE object_singleton<connector_frame>::Instance

#endif

