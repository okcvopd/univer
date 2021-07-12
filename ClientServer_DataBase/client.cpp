#include "conn.hpp"

int printHead(FILE *fo);

static void repl(int fd);
static int signalIgnoring(void);

int main(int argc, char *argv[]) {
    const char        *host = "127.0.0.1";
    uint16_t           port = 8000;
    struct sockaddr_in addr;
    int                fd;

    if(argc != 1 && argc != 3) {
        fprintf(stderr, "Usage: %s [<ip> <port>]\n", argv[0]);
        return -1;
    }

    if(argc == 3) {
        host = argv[1];
        if(toUInt16(argv[2], &port) == -1) {
            fprintf(stderr, "Wrong parametr <port>\n");
            return -1;
        }
    }

    if(signalIgnoring() != 0)
        return -1;

    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if(inet_pton(AF_INET, host, &addr.sin_addr) < 1) {
        fprintf(stderr, "     - Host error.\n");
        return -1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        fprintf(stderr, "     - Socket error.\n");
        return -1;
    }

    if(connect(fd, (struct sockaddr *)&addr, sizeof addr) == -1) {
        fprintf(stderr, "%4d - Connect error.\n", fd);
        if(close(fd))
            fprintf(stderr, "%4d - Close error.\n", fd);
        return -1;
    }

    repl(fd);

    if(shutdown(fd, 2) == -1) {
        fprintf(stderr, "%4d - Shutdown error.\n", fd);
        if(close(fd))
            fprintf(stderr, "%4d - Close error.\n", fd);
        return -1;
    }

    if(close(fd)) {
        fprintf(stderr, "%4d - Close error.\n", fd);
        return -1;
    }

    return 0;
}

static void handler(int signo);

static int signalIgnoring(void) {
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    
    if(sigaction(SIGPIPE, &act, 0) == -1) {
        fprintf(stderr, "     - Sigaction(SIGPIPE) error.\n");
        return -1;
    }

    return 0;
}

static void handler(int signo) {
    (void)signo;
}

#define BUFLEN 50

static bool readLine(std::vector<char> &l) {
    char   buf[BUFLEN];    
    size_t len;

    l.clear();    

    for(;;) {
        if(!fgets(buf, sizeof buf, stdin))
            return false;

        len = strlen(buf);
       
        l.insert(l.end(), buf, buf + len); 

        if(buf[len - 1] == '\n') {
            l.push_back('\0');
            break;
        } 
    }

    return true;
}

static bool writeRead(int fd, const char *txt);
static bool readLine(std::vector<char> &l);

static void repl(int fd) {
    for(;;) {
        printf("> ");
   
        std::vector<char> l;
        if(!readLine(l))
        {
            if (feof(stdin))
                printf("End of process\n");
            return;
        }
        if(!writeRead(fd, &l[0]))
            return;
    }
}

#define LINESIZE 70

static bool writeAll(int fd, const char *buf, size_t len);
static bool readAll(int fd, char *buf, size_t len);

static bool writeRead(int fd, const char *txt) {
    try {
        uint32_t len = strlen(txt) + 1;
        int status=0;
        uint32_t total=0;

        if(!writeAll(fd, (const char *)&len,sizeof (len)))
            throw "     - Write(length) error.";

        if(!writeAll(fd, txt, len))
            throw "     - Write(text) error.";

        if (!readAll(fd,(char *)&status,sizeof(status)))
            throw "     - Read(status) error.";

        if(!readAll(fd, (char *)&len, sizeof(len)))
            throw "     - Read(length) error.";
        
        Note note;
        if (len!=0)
        {
            printHead(stdout);
            for (uint32_t i=0;i<len;i++)
            {
                note.readBin(fd);
                note.printNote(stdout);
            }
        }
        if ((Result_Status)status==SUCCESS)
        {
            if (!readAll(fd,(char *)&total,sizeof(total)))
                throw "     - Read(total error.";
            fprintf(stdout,"\nTotal: %d \n",total);
        }
        else
        {
            printResult_Status(stdout,(Result_Status)status);
        }
        return true;
    }
    catch(const char *err) {
        fprintf(stderr, "%s\n", err);
    }
    catch(std::bad_alloc &e) {
        fprintf(stderr, "Memory allocation error\n");
    }

    return false;
}

static bool writeAll(int fd, const char *buf, size_t len) {
    for(size_t cur = 0; len - cur;) {
        ssize_t s = write(fd, buf + cur, len - cur);
        if(s > 0) {
            cur += s;
        }
        else
            return false;
    }
    return true;
}

static bool readAll(int fd, char *buf, size_t len) {
    
    for(size_t cur = 0; len - cur;) {
        ssize_t s = read(fd, buf + cur, len - cur);
        if(s > 0) {
            cur += s;
        } 
        else
            return false;
    }
    return true;
}

