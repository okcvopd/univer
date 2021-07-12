#ifndef _LIBRARY_H_
#define _LIBRARY_H_
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

class Note
{
    public:
        void setRand();
        int readBin( int fd );
        int writeBin( int fd );
        int printNote(FILE *fo) const;
        void writeStr( char *str) const;
        int sizeofStr() const;
        int getYear() const
        {
            return year_;
        }
        int getNumber() const
        {
            return number_;
        }
        char  compareAuthor(const char *author) const
        {
            return (strcmp(author_,author)==0);
        }
        Note(char journal[64], int year, int number, char author[32], char article[128]=NULL):year_(year),number_(number)
        {
            strncpy(journal_,journal,64);
            strncpy(author_,author,32);
            strncpy(article_,article,128);
        }
        Note():year_(1869),number_(1)
        {}
        ~Note()
        {}
    private:
        char journal_ [64];
        int year_, number_;
        char author_ [32] ;
        char article_ [128];
};



#endif
