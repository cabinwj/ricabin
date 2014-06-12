#ifndef _BASE_FRAME_H_
#define _BASE_FRAME_H_

#include "hc_types.h"
#include "hc_bitmap.h"

#include "protocol.h"

extern bitmap32 g_run_status;

// 守护进程
void init_daemon( int nochdir, int noclose );

// 安装信号处理函数
void signal_handler();

void release_tunnel(int32_t net_id);

int init_environment();
void run_environment_once();
void stop_environment();

#endif
