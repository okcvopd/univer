#include "conn.hpp"

Conn::Conn(int fd): fd_(fd), actv_(true), icur_(0), iall_(4), ocur_(0), oall_(0), ibuf_(4, 0), obuf_(4, 0) {}

Conn::~Conn() {
    close_();
}

bool Conn::canRcv() const {
    return actv_ && iall_ > icur_;
}

bool Conn::canSnd() const {
    return actv_ && oall_ > ocur_;
}

bool Conn::isActv() const {
    return actv_;
}



void Conn::rcv(database& DataB) {
    uint32_t h;
    ssize_t  s;

    if(!canRcv()) return;
    
    s = read(fd_, &ibuf_[icur_], iall_ - icur_);
    if(s > 0)
        icur_ += s;
    else
        if(!s) {
            close_();
            return;
        }
        else
            if(s == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "%4d - Read error.\n", fd_);
                close_();
                return;
            }

    if(iall_ == icur_) {
        if(iall_ == sizeof(h)) {
            memcpy(&h, &ibuf_[0], sizeof(h));
            ibuf_.resize(h);
            iall_ = h;
            icur_=0;
        }
        else {
            obuf_.resize(sizeof(int)+sizeof(uint32_t));
            result=DataB.perform(&ibuf_[0],session);
            int status=(int)result.Status();

            memcpy(&obuf_[0],&status, sizeof(status));
            if (result.Status()==SUCCESS)
                h=result.resultNotes().size();
            else
                h=0;
            oall_=sizeof(h)+sizeof(int);
            memcpy(&obuf_[0]+sizeof(int),&h, sizeof(h));
        }
    }
}

void Conn::snd() {
    if(!canSnd()) return;

    ssize_t s = write (fd_, &obuf_[0] + ocur_, oall_ - ocur_);

    if(s > 0)
        ocur_ += s;
    else
        if(!s) {
            close_();
            return;
        }
        else
            if(s == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "%4d - Write error.\n", fd_);
                close_();
                return;
            }

    if(oall_ == ocur_) {
        if (result.Status()==SUCCESS)
        {
            if (!result.resultNotes().empty())
            {
                ocur_=0;
                Note note=result.resultNotes().front();
                oall_=note.sizeofStr();
                obuf_.resize(oall_);
                note.writeStr(&obuf_[0]);
                sizenote=oall_;
                result.popFront();
            }
            else
            {
                if (oall_==sizeof(uint32_t)+sizeof(int) || sizenote==oall_ )
                {
                    uint32_t total=(uint32_t)result.Total();
                    oall_=sizeof(total);
                    ocur_=0;
                    obuf_.resize(oall_);
                    memcpy(&obuf_[0],&total,sizeof(total));
                }
                else
                {
                    icur_ = 0;
                    iall_ = 4;
                    ocur_ = 0;
                    oall_ = 0;  
                }
            }   
        }
        else
        {
            icur_ = 0;
            iall_ = 4;
            ocur_ = 0;
            oall_ = 0;
        }
    }
}

void Conn::close_() {
    if(actv_) {
        printf("%4d - Close.\n", fd_);

        if(shutdown(fd_, 2) == -1)
            fprintf(stderr, "%4d - Shutdown error.\n", fd_);

        if(close(fd_))
            fprintf(stderr, "%4d - Close error.\n", fd_);

        actv_ = false;
    }
}


ConnDb::ConnDb(int ld, size_t nc): ld_(ld), nc_(nc), cs_(nc, 0), ps_(nc + 1) {
    ps_[0].fd = ld;
    ps_[0].events = POLLIN;
}

ConnDb::~ConnDb() {
    for(size_t d = 0; d < nc_; d ++)
        delete cs_[d];
}

void ConnDb::perform( database &DataB) {
    nfds_t np;    
    size_t d;

    for(np = 1, d = 0; d < nc_; d ++)
        if(cs_[d] && cs_[d]->isActv()) {
            short e = 0;

            if(cs_[d]->canRcv())
                e = POLLIN;


            if(cs_[d]->canSnd())
                e = e ? e | POLLOUT : POLLOUT;

            if(e) { 
                ps_[np].fd     = d;
                ps_[np].events = e;
                np ++;
            }
        }

    switch(poll(&ps_[0], np, 1 * 1000)) { 
        case 0:
            break;

        case -1:
            if(errno != EINTR)
                fprintf(stderr, "     - Poll error.\n"); 
            break;

        default:
            if(ps_[0].revents & POLLIN) 
                accept_();

            for(d = 1; d < np; d ++) {
                if(ps_[d].revents & POLLIN)
                    cs_[ps_[d].fd]->rcv(DataB);

                if(ps_[d].revents & POLLOUT)
                    cs_[ps_[d].fd]->snd();

            }
    }
}

static void printConn(int fd, struct sockaddr_in *addr);

void ConnDb::accept_() {
    struct sockaddr_in addr;
    socklen_t          addrlen;
    int                fd;
    int                fl;

    memset(&addr, 0, sizeof addr);
    addrlen = sizeof addr;

    fd = accept(ld_, (struct sockaddr *)&addr, &addrlen);
    if(fd == -1) {
        if(fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
            fprintf(stderr, "     - Accept error.\n");
        return;
    }

    printConn(fd, &addr);

    if((size_t)fd >= nc_) {
        fprintf(stderr, "%4d - Storage limit error.\n", fd);
        Conn c(fd);
    }
    else {
            if((fl = fcntl(fd, F_GETFL)) == -1) {
                fprintf(stderr, "%4d - Fcntl (F_GETFL) error.\n", fd);
                Conn c(fd);
                return;
            }

            if(fcntl(fd, F_SETFL, fl | O_NONBLOCK) == -1) {
                fprintf(stderr, "%4d - Fcntl (F_SETFL) error.\n", fd);
                Conn c(fd);
                return;
            }

            delete cs_[fd];
            cs_[fd] = new Conn(fd);
        }
}

static void printConn(int fd, struct sockaddr_in *addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof ip);
    printf("%4d -New (%s %d).\n", fd, ip, addr->sin_port);
}

int toUInt16(const char *str, uint16_t *p) {
    long  l;
    char *e;

    errno = 0;
    l = strtol(str, &e, 10);

    if(!errno && *e == '\0' && 0 <= l && l <= 65535) {
        *p = (uint16_t)l;
        return 0;
    }
    else
        return -1;
}

