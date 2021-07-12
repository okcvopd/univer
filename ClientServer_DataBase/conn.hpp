#ifndef __CONN_HPP__
#define __CONN_HPP__

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cinttypes>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include <vector>
#include "database.h"

class Conn {
public:
    Conn(int fd);
   ~Conn();

    bool canRcv() const;
    bool canSnd() const;
    bool isActv() const;

    void rcv(database &DataB);
    void snd();

private:
    int               fd_;
    bool              actv_;
    size_t            icur_,
                      iall_,
                      ocur_,
                      oall_,
                      sizenote;
    std::vector<char> ibuf_,
                      obuf_;
    Session session;
    Result result;
    void close_();
    friend class ConnDb;
};

class ConnDb {
public:
    ConnDb(int ld, size_t nc);
   ~ConnDb();

    void perform(database &DataB);

private:
    int                 ld_;
    size_t              nc_;
    std::vector<Conn *> cs_;
    std::vector<pollfd> ps_;

    void accept_();
};

int toUInt16(const char *str, uint16_t *p);

#endif
