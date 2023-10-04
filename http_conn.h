#ifndef HTTPSERVER_HTTP_CONN_H
#define HTTPSERVER_HTTP_CONN_H

#include <unistd.h>
#include <csignal>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include "http_conn.h"

class http_conn {
public:
    static const int FILENAME_LEN = 200;

    static const int READ_BUFFER_SIZE = 2048;

    static const int WRITE_BUFFER_SIZE = 1024;

    enum METHOD {
        GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH
    };

    enum CHECK_STATE {
        CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER, CHECK_STATE_CONTENT
    };

    enum HTTP_CODE {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSE_CONNECTION
    };

    enum LINE_STATUS {
        LINE_OK = 0, LINE_BAD, LINE_OPEN
    };

public:
    http_conn() = default;

    ~http_conn() = default;

public:
    void init(int sockfd, const sockaddr_in &addr);

    void close_conn(bool real_close = true);

    /* 处理客户请求 */
    void process();

    /* 非阻塞读操作 */
    bool read();

    /* 非阻塞写操作 */
    bool write();

private:
    void init();

    /* 解析HTTP请求 */
    HTTP_CODE process_read();

    bool process_write(HTTP_CODE ret);

    HTTP_CODE parse_request_line(char *text);

    HTTP_CODE parse_headers(char *text);

    HTTP_CODE parse_content(char *text) const;

    HTTP_CODE do_request();

    char *get_line() { return m_read_buf + m_start_line; }

    LINE_STATUS parse_line();

    void unmap();

    bool add_response(const char *format, ...);

    bool add_content(const char *content);

    bool add_status_line(int status, const char *title);

    void add_headers(size_t content_length);

    bool add_content_length(size_t content_length);

    bool add_linger();

    bool add_blank_line();

public:
    static int m_epollfd;
    static int m_user_count;

private:
    int m_sockfd;
    sockaddr_in m_address;

    /* 读缓冲区 */
    char m_read_buf[READ_BUFFER_SIZE];
    /* 标识读缓冲区中已经读入的客户端的最后一个字节的下一个位置 */
    int m_read_idx;
    /* 当前正在分析的字符在读缓冲区的位置 */
    int m_checked_idx;
    /* 当前正在解析的行的起始位置 */
    int m_start_line;
    /* 写缓冲区 */
    char m_write_buf[WRITE_BUFFER_SIZE];
    /* 写缓冲区中待发送的字节数 */
    int m_write_idx;

    CHECK_STATE m_check_state;
    METHOD m_method;

    char m_real_file[FILENAME_LEN];
    char *m_url;
    char *m_version;
    char *m_host;
    int m_content_length;
    /* HTTP请求是否要求保持连接 */
    bool m_linger;

    char *m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;

};


#endif //HTTPSERVER_HTTP_CONN_H
