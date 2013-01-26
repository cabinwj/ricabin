#ifndef _WEBAGENT_FRAME_H_
#define _WEBAGENT_FRAME_H_

#include "hc_bitmap.h"

// 配置文件
#ifdef WIN32
#define WEBAGENT_FRAMED_CONF "etc\\webagentd_config.xml"
#else
#define WEBAGENT_FRAMED_CONF "etc/webagentd_config.xml"
#endif

void sigusr1_handle(int signo);
void sigusr2_handle(int signo);

class webagent_frame
{
public:
    webagent_frame();
    virtual ~webagent_frame();

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

#define CREATE_WEBAGENT_FRAME_INSTANCE  object_singleton<webagent_frame>::Instance
#define GET_WEBAGENT_FRAME_INSTANCE     object_singleton<webagent_frame>::Instance
#define DESTROY_WEBAGENT_FRAME_INSTANCE object_singleton<webagent_frame>::Instance

#endif

