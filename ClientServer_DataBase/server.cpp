#include "conn.hpp"


static int signalIgnoring(void);

#define NC 100
static void loop(int ld, size_t nc, const char* filename);

int main(int argc, char *argv[]) {
    const char        *host = 0;
    uint16_t           port = 8000;
    struct sockaddr_in addr;
    int                ld   = -1;
    int                on;
    int                st   = 0;
    int                fl;

    try {
        if(argc != 2) {
            fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
            throw -1;
        }

        if(signalIgnoring() != 0)
            throw -1;

        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if(host) {
            if(inet_pton(AF_INET, host, &addr.sin_addr) < 1) {
                fprintf(stderr, "     - Host error.\n");
                throw -1;
            }
        }
        else
            addr.sin_addr.s_addr = INADDR_ANY;

        ld = socket(AF_INET, SOCK_STREAM, 0);
        if(ld == -1) {
            fprintf(stderr, "     - Socket error.\n");
            throw -1;
        }

        on = 1;
        if(setsockopt(ld, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == -1) {
            fprintf(stderr, "%4d - Setsockopt error.\n", ld);
            throw -1;
        }

        if((fl = fcntl(ld, F_GETFL)) == -1) {
            fprintf(stderr, "%4d - Fcntl (F_GETFL) error.\n", ld);
            throw -1;
        }

        if(fcntl(ld, F_SETFL, fl | O_NONBLOCK) == -1) {
            fprintf(stderr, "%4d - Fcntl (F_SETFL) error.\n", ld);
            throw -1;
        }

        if(bind(ld, (struct sockaddr *)&addr, sizeof addr) == -1) {
            fprintf(stderr, "%4d - Bind error.\n", ld);
            throw -1;
        }

        if(listen(ld, 5) == -1) {
            fprintf(stderr, "%4d - Listen error.\n", ld);
            throw -1;
        }

        
        loop(ld, NC, argv[1]);
    }
    catch(std::bad_alloc &e)
    {
        fprintf(stderr,"Allocation error\n");
        st=-1;
    }
    catch(const char *message)
    {
        fprintf(stderr,"%s\n",message);
        st=-1;
    }
    catch(int e) {
        st = e;
    }

    if(ld != -1 && close(ld)) {
        fprintf(stderr, "%4d - Close error.\n", ld);
        st = -1;
    }

    puts("Done.");

    return st;
}

static void handler(int signo);

static int signalIgnoring(void) {
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);

    if(sigaction(SIGINT, &act, 0) == -1) {
        fprintf(stderr, "     - Sigaction(SIGINT) error.\n");
        return -1;
    }

    if(sigaction(SIGPIPE, &act, 0) == -1) {
        fprintf(stderr, "     - Sigaction(SIGPIPE) error.\n");
        return -1;
    }

    return 0;
}

static volatile sig_atomic_t quit;

static void handler(int signo) {
    if(signo == SIGINT) 
        quit = 1;
}

static void loop(int ld, size_t nc, const char* filename) {
    try
    {
        ConnDb db(ld, nc);
        database DataB;
        DataB.readDataBase(filename);
        while(!quit) 
            db.perform(DataB);
    }
    catch(const char *message)
    {
        throw message;
    }
}

