#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "common_os.h"
#include "common_log.h"
#include "address.h"

class Socket
{
public:
    /** Create a socket that is closed.
     */
    Socket();

    /** Create a socket with an already created file-descriptor.
     * This is used for descriptors that are returned by accept() which returns an opened socket.
     */
    explicit Socket(Descriptor sfd);

    /** Create a socket and opens it.
     * @param protocol The protocol to be used.
     * @exception Exception An exception is thrown when the socket is already open or
     * when it cannot be opened.
     */
    explicit Socket(Family family, Type type, Protocol protocol);

    /** Destroy the socket.
     * If the socket is open close it.
     * @sa close()
     */
    ~Socket();

public:
    /** Opens the Socket.
     * Is like a call to socket() of the C API.
     * @param protocol The protocol to be used.
     * @exception Exception An exception is thrown when the socket is already open or
     * when it cannot be opened.
     */
    int open(Family family, Type type, Protocol protocol);

    /** Close the socket.
     * @exception Exception An exception is thrown when the socket is already closed.
     */
    void close();

    /** The sockets can be used like a file-descriptor.
     */
    operator Descriptor() { return descriptor_; }

    Socket& operator=(Descriptor socket) { descriptor_ = socket; return *this; }

    //bool is_opened() { return descriptor_ >= 0; } //linux 文件描述符可以为0，切记!!!
    bool is_opened() { return (descriptor_ != INVALID_SOCKET) && (descriptor_ >= 0); } //linux 文件描述符可以为0，切记!!!

    /** Assign this socket an address. (classic)
     * @param addr Address to be assigned.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API bind() returns an error.
     */
    int bind(const sockaddr_in& addr);

    /** Assign this socket an address.
     * @param addr Address to be assigned.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API bind() returns an error.
     */
    int bind(const Address& addr);

    /** Connect this sockets to another socket. (classic)
     * @param addr The address of the other socket.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API connect() returns an error.
     */
    int connect(const sockaddr_in& addr);

    /** Connect this sockets to another socket.
     * @param addr The address of the other socket.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API connect() returns an error.
     */
    int connect(const Address& addr);

    /** Send data through the socket. (classic)
     * A connection must be established.
     * @param data Pointer to the beginning of the data.
     * @param size How many bytes should be sent.
     * @param flags Optional flags to tune the behaviour.
     * @return How many byte are really sent.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API send() returns an error.
     * @sa recv()
     */
    int send(const char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);

    /** Send data to @e addr (classic).
     * @param addr Address to which the data should be sent.
     * @param data Pointer to the beginning of the data.
     * @param size How many bytes should be sent.
     * @param flags Optional flags to tune the behaviour.
     * @return How many byte are really sent.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API sendto() returns an error.
     * @sa recvfrom()
     */
    int sendto(const sockaddr_in& addr, const char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);

    int sendto(const Address& addr, const char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);

    /** Read data from socket (classic).
     * A connection must be established.
     * @param data Pointer to the buffer that the read data should be put in.
     * @param size Maximum of bytes allowed to be read => size of buffer.
     * @param flags Optional flags to tune the behaviour.
     * @return How many bytes are really read.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API recv() returns an error.
     * @sa send()
     */
    int recv(char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);

    /** Read data from socket (classic).
     * The address of the sender is returned in @e addr.
     * @param addr Address of the sender.
     * @param data Pointer to the buffer that the read data should be put in.
     * @param size Maximum of bytes allowed to be read => size of buffer.
     * @param flags Optional flags to tune the behaviour.
     * @return How many bytes are really read.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API recvfrom() returns an error.
     * @sa sendto()
     */
    int recvfrom(sockaddr_in& addr, char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);
    int recvfrom(Address& addr, char* data, size_t size, int flags/* = MSG_NOSIGNAL*/);

    /** Wait for connections.
     * @param queuelen Maximum number of clients in wait-queue.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API listen() returns an error.
     */
    int listen(int queuelen);

    /** Accept connection (classic).
     * Waits until a connection in established.
     * @param addr Connected client address
     * @return Socket descriptor for new connection. (Data transfer-channel)
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API accept() returns an error.
     */
    Descriptor accept(sockaddr_in& addr);

    /** Accept connection.
     * Waits until a connection in established.
     * @param addr Connected client address
     * @return Socket descriptor for new connection. (Data transfer-channel)
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API accept() returns an error.
     */
    Descriptor accept(Address& addr);

    /** Returns the address of the socket.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API getsockname() returns an error.
     * @sa getpeername()
     */
    int getsockname(sockaddr_in& addr);
    int getsockname(Address& addr);

    /** Returns the address of the connected host.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API getpeername() returns an error.
     * @sa getsockname()
     */
    int getpeername(sockaddr_in& addr);
    int getpeername(Address& addr);

    /** Cancel active full-duplex connection.
     * See Shutdown for more info.
     * @param how How to shutdown the connection.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API shutdown() returns an error.
     */
    int shutdown(shutdown_t how);

    /** Wait for socket can be read, written or for an exception.
     * This is a select for exactly one socket.
     * The read, write and exception flags are set, when the appropriate event occurred.
     * E.g. when the method returns true and read is true, there had some data arrived on the socket.
     * @param read Wait until there is something available to read from the socket.
     * @param write Wait until a write would not block.
     * @param exception Test for exception on the socket.
     * @param seconds Timeout in seconds.
     * @param useconds Timeout in microseconds.
     * @return True, if no timeout occurred.
     */
    bool wait(bool& read, bool& write, bool& exception, int seconds = 0, int useconds = 0);

    bool can_read();
    bool can_write();

public:
    /** Read socket option.
     * The options are available as classes in the sub-namespaces of CPPSocket:
     * SocketOption, IPOption and TCPOption.
     * @param option Option to be read.
     * @return How many bytes has the read value.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API getsockopt() returns an error.
     * @sa setsockopt()
     */
    static int getsockopt(Descriptor descriptor, int level, int optname, void* optval, socklen_t* optlen);

    /** Set socket option.
     * The options are available as classes in the sub-namespaces of CPPSocket:
     * SocketOption, IPOption and TCPOption.
     * @param option Option to be set.
     * @exception Exception An exception is thrown when the socket is not opened or
     * the call to C API setsockopt() returns an error.
     * @sa getsockopt()
     */
    static void setsockopt(Descriptor descriptor, int level, int optname, const void* optval, socklen_t optlen);

    static void setnonblock(Descriptor socket); 
#ifndef WIN32
    static void fcntl(Descriptor descriptor,int flag);
#endif

private:
    /// filedescriptor; if >= 0 it is valid; else not opened
    Descriptor descriptor_;
    Socket(Socket& socket);
    Socket& operator=(Socket& socket);
};

#endif
