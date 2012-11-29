#ifndef _CORO_FRAME_BASE_H_
#define _CORO_FRAME_BASE_H_

#include "common_types.h"

#include "coroutine.h"
#include "scheduler.h"

#include "net_event.h"
#include "net_package.h"

#include "protocol.h"

#ifdef WIN32
#ifndef typeof
#define typeof decltype
#endif
#endif

void init_daemon( int nochdir, int noclose );

uint32_t create_network(const net_conf_t& conf, net_event_handler_callback_t handler);
void release_tunnel(uint32_t net_id);

int init_environment(uint32_t uin);
void run_environment();
void run_environment_once();
void stop_environment();

// return -1表示发送失败，-2表示异步返回超时
// 0表示操作成功，ne返回NULL表示socket close or socket exception
int send_async_package(uint32_t net_id, net_package*& np, int timeout, net_event*& ne);

void coro_sleep(int32_t timeout);


#define SPAWN(F, ...)                                                   \
    coro_scheduler::push_coro(new CoroutineT<typeof(F)>(F, ##__VA_ARGS__))

#define SPAWN_WITH_STACK(N, F, ...)                                     \
    coro_scheduler::push_coro(new CoroutineT<typeof(F)>(N, F, ##__VA_ARGS__))

#endif

