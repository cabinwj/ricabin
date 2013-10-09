#include "net_id_holder.h"
#include "hc_stack_trace.h"


// class net_id_guard
net_id_holder::net_id_holder()
{
    STACK_TRACE_LOG();

    m_tcp_connector_id_queue_ = new net_id_queue( TCP_CONNECTOR_END - TCP_CONNECTOR_BEGIN + 1 );
    for (int32_t id = TCP_CONNECTOR_BEGIN; id <= TCP_CONNECTOR_END; id++) {
        m_tcp_connector_id_queue_->push(id);
    }

    m_tcp_acceptor_id_queue_ = new net_id_queue( TCP_ACCEPTOR_END - TCP_ACCEPTOR_BEGIN + 1 );
    for (int32_t id = TCP_ACCEPTOR_BEGIN; id <= TCP_ACCEPTOR_END; id++) {
        m_tcp_acceptor_id_queue_->push(id);
    }

    m_tcp_stream_id_queue_ = new net_id_queue( TCP_STREAM_END - TCP_STREAM_BEGIN + 1 );
    for (int32_t id = TCP_STREAM_BEGIN; id <= TCP_STREAM_END; id++) {
        m_tcp_stream_id_queue_->push(id);
    }
}

net_id_holder::~net_id_holder()
{
    STACK_TRACE_LOG();

    if (NULL != m_tcp_connector_id_queue_)
    {
        delete m_tcp_connector_id_queue_;
        m_tcp_connector_id_queue_ = NULL;
    }
    
    if (NULL != m_tcp_acceptor_id_queue_)
    {
        delete m_tcp_acceptor_id_queue_;
        m_tcp_acceptor_id_queue_ = NULL;
    }
    
    if (NULL != m_tcp_stream_id_queue_)
    {
        delete m_tcp_stream_id_queue_;
        m_tcp_stream_id_queue_ = NULL;
    }    
}

int32_t net_id_holder::acquire(net_id_holder::net_id_t type)
{
    STACK_TRACE_LOG();

    int rc = 0;
    int32_t id = 0;

    switch (type)
    {
    case net_id_holder::ID_TCP_CONNECTOR:
        rc = m_tcp_connector_id_queue_->pop(id, true);
        break;
    case net_id_holder::ID_TCP_ACCEPTOR:
        rc = m_tcp_acceptor_id_queue_->pop(id, true);
        break;
    case net_id_holder::ID_TCP_STREAM:
        rc = m_tcp_stream_id_queue_->pop(id, true);
        break;
    default:
        break;
    }

    if (0 != rc) {
        // 此处id不可能用尽
        return 0;
    }

    return id;
}

void net_id_holder::release(int32_t id)
{
    STACK_TRACE_LOG();

    if ((id >= TCP_CONNECTOR_BEGIN) && (id <= TCP_CONNECTOR_END)) {
        m_tcp_connector_id_queue_->push(id);
    }
    else if ((id >= TCP_ACCEPTOR_BEGIN) && (id <= TCP_ACCEPTOR_END)) {
        m_tcp_acceptor_id_queue_->push(id);
    }
    else if ((id >= TCP_STREAM_BEGIN) && (id <= TCP_STREAM_END)) {
        m_tcp_stream_id_queue_->push(id);
    }
}

