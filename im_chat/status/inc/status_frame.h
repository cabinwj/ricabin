#ifndef _STATUS_FRAME_H_
#define _STATUS_FRAME_H_

#include "common_bitmap.h"


// 配置文件
#ifdef WIN32
#define ROUTERD_CONF "etc\\statusd_config.xml"
#else
#define ROUTERD_CONF "etc/statusd_config.xml"
#endif

void sigusr1_handle(int signo);
void sigusr2_handle(int signo);

class status_frame
{
public:
    status_frame();
    virtual ~status_frame();

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

#define CREATE_STATUS_INSTANCE  object_singleton<status_frame>::Instance
#define GET_STATUS_INSTANCE     object_singleton<status_frame>::Instance
#define DESTROY_STATUS_INSTANCE object_singleton<status_frame>::Instance

#endif

