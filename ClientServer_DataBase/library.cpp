#include "library.h"

void Note::setRand()
{
    int l,i,k;
    const char *b[]={"Nature", "Science", "Topology", "Journal of Algebra", "Genetica", "Computer", "Zoology", "History"};
    l=rand()%8;
    strcpy(journal_,b[l]);
    year_=1869+rand()%152;
    number_=1+rand()%500;
    const char *a[]={"Smith W.", "Ivanov I.A.","Miller F.", "Kuznetsov Y.A", "Bertrand A.W.", "Johnson R.", "Moore J.", "Fomenko A.T."};
    l=rand()%8;
    strcpy(author_,a[l]);
    l=2+rand()%126;
    article_[0]='A'+rand()%26;
    for ( i=1; i<l; i++)
    {
	k=rand()%27;
	if (k==26)
	    article_[i]=' ';
        else
	    article_[i]='a'+k;
    }
    article_[i]='\0';
}

int Note::writeBin( int fd)
{
    ssize_t c;
    c=write(fd,journal_,sizeof journal_);
    if (c!=(int)( sizeof journal_))
        return -1;

    c=write(fd,&year_,sizeof year_);
    if (c!=(int)( sizeof year_))
        return -1;

    c=write(fd,&number_,sizeof number_);
    if (c!=(int)( sizeof number_))
        return -1;

    c=write(fd,author_,sizeof author_);
    if (c!=(int)( sizeof author_))
        return -1;

    c=write(fd,article_,sizeof article_);
    if (c!=(int)( sizeof article_))
        return -1;
    return 1;
}

int Note::printNote(FILE *fo) const
{
    char *t;
    char a=' ';
    t=(char*)malloc(172*sizeof(char));
    if (!t)
        return -1;
    if (strlen(article_)<=64)
        sprintf(t,"%-64s|%-4d|%-3d|%-32s|%-64s",journal_,year_,number_,author_,article_);
    else
    {
        sprintf(t,"%-64s|%-4d|%-3d|%-32s|",journal_,year_,number_,author_);
	    memcpy(t + 107, article_, 64);
	    t[171] = '\0';
	    fprintf(fo,"%s\n", t);
        sprintf(t, "%64c|%4c|%3c|%32c|%-64s", a,a,a,a,article_+64);
    }
    fprintf(fo,"%s\n",t);
    free(t);
    return 0;
}

int Note::readBin( int fd )
{
    ssize_t c;
    c=read(fd,journal_,sizeof journal_);
    if (c!=(int)( sizeof journal_))
    {  
        if (c==0)
            return 0;
        else
            return -1;
    }
    c=read(fd,&year_,sizeof year_);
    if (c!=(int)( sizeof year_))
        return -1;

    c=read(fd,&number_,sizeof number_);
    if (c!=(int)( sizeof number_))
        return -1;
    
    c=read(fd,author_,sizeof author_);
    if (c!=(int)( sizeof author_))
        return -1;

    c=read(fd,article_,sizeof article_);
    if (c!=(int)( sizeof article_))
        return -1;
    return 1;
}

void Note::writeStr(char *str) const
{
    size_t len=0;
    memcpy(str,journal_,sizeof(journal_));
    len+=sizeof(journal_);
    memcpy(str+len,&year_,sizeof(year_));
    len+=sizeof(year_);
    memcpy(str+len,&number_,sizeof(number_));
    len+=sizeof(number_);
    memcpy(str+len,author_,sizeof(author_));
    len+=sizeof(author_);
    memcpy(str+len,article_,sizeof(article_));
}

int Note::sizeofStr() const
{
    return sizeof(journal_)+sizeof(year_)+sizeof(number_)+sizeof(author_)+sizeof(article_);
}

int printHead(FILE *fo);
int printHead(FILE *fo)
{
    char* t;
    char a=' ';
    t=(char *)malloc(174*sizeof(char));
    if (!t)
        return -1;
    sprintf(t,"Journal name%52c|Year|Num|Author%26c|Article%57c",a,a,a);
    fprintf(fo,"%s\n",t);
    memset(t , '-', 171);
    fprintf(fo,"%s\n",t);
    free(t);
    return 0;
}
