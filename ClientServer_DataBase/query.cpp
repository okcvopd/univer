#include"query.h"

int readUntilBracket(const char *cur_,std::vector<char> &string);
int readUntilBracket(const char *cur_,std::vector<char> &string)
{
    const char *cur=cur_;
    int i=0;
    bool space=0;
    cur+=skip(cur);
    while (cur[i]!='\n' && cur[i]!=EOF)
    {
        if (cur[i]==']')
        {
            if (space)
                string[i]='\0';
            else
                string.push_back('\0');
            return 1;
        }
        if (cur[i]=='[')
            return -1;
        if (cur[i]==' ')
        {
            if (space)
            {
                i++;
                continue;
            }
            else
                space=1;
        }
        else
            space=0;
        string.push_back(cur[i]);
        i++; 
    }
    return -1;
}

const char * Query::GetYearFilter(const char *cur_, Filter &filter, Result_Status &r)
{
    const char *cur=cur_;
    cur+=skip(cur);
    int x=0;
    sscanf(cur,"year=[%n",&x);
    if (x!=0)
    {
        int a1,a2;
        cur+=x;
        cur+=skip(cur);
        sscanf(cur,"%d%n",&a1,&x);
        if (x==0)
        {
            r=INCORRECT_FILTER;
            return cur;
        }
        cur+=x;
        cur+=skip(cur);
        if(cur[0]!=',')
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        cur++;
        cur+=skip(cur);
        sscanf(cur,"%d%n",&a2,&x);
        if (x==0)
        {
            r= INCORRECT_FILTER;
            return cur;
        }
        cur+=x;
        cur+=skip(cur);
        if (cur[0]!=']')
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        cur++;
        cur+=skip(cur);
        if (a1>a2)
        {
            r= INCORRECT_FILTER;
            return cur;
        }
        filter.AddYears(Year_Interval(a1,a2)); 
        { 
            r= SUCCESS;
            return cur;
        }
    }
    else
    {
        r= NO_RESULT;
        return cur;
    }
}

const char * Query::GetNumberFilter(const char *cur_, Filter &filter,Result_Status &r)
{
    const char *cur=cur_;
    cur+=skip(cur);
    int x=0;
    sscanf(cur,"number=[%n",&x);
    if (x!=0)
    {
        int a1,a2;
        cur+=x;
        cur+=skip(cur);
        sscanf(cur,"%d%n",&a1,&x);
        if (x==0)
        {
            r= INCORRECT_FILTER;
            return cur;
        }
        cur+=x;
        cur+=skip(cur);
        if(cur[0]!=',')
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        cur++;
        cur+=skip(cur);
        sscanf(cur,"%d%n",&a2,&x);
        if (x==0)
        {
            r=INCORRECT_FILTER;
            return cur;
        }
        cur+=x;
        cur+=skip(cur);
        if (cur[0]!=']')
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        cur++;
        cur+=skip(cur);
        if (a1>a2)
        {
            r= INCORRECT_FILTER;
            return cur;
        }
        filter.AddNumbers(Number_Interval(a1,a2));
        r= SUCCESS;
        return cur; 
    }
    else
    {
        r= NO_RESULT; 
        return cur;   
    }
}

const char * Query::GetAuthorFilter(const char *cur_, Filter &filter,Result_Status &r)
{
    const char *cur=cur_;
    cur+=skip(cur);
    int x=0;
    sscanf(cur,"author=[%n",&x);
    if(x!=0)
    {
        char a[32];
        cur+=x;
        cur+=skip(cur);
        std::vector<char> s;
        if (readUntilBracket(cur,s)!=1)
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        if (s.size()>32)
        {
            r= SYNTAX_ERROR;
            return cur;
        }
        for (size_t i=0;i<s.size();i++)
            a[i]=s[i];
        cur+=s.size();
        cur+=skip(cur);
        filter.AddAuthor(Author(a));
        r= SUCCESS;
        return cur;  
    }
    else 
    {
        r= NO_RESULT;
        return cur;
    }
}

Result_Status Query::Insert(const char *cur_)
{
    const char *cur=cur_;
    cur+=skip(cur);
    int x=0, year, number;
    bool i1=0, i2=0, i3=0, i4=0, i5=0; 
    char journal[64];
    char author[32];
    char article[128];
    while(!(i1&&i2&&i3&&i4&&i5))
    {
        cur+=skip(cur);
        x=0;
        sscanf(cur,"journal=[%n",&x);
        if (x!=0)
        {
            cur+=x;
            cur+=skip(cur);
            std::vector<char> s;
            if (readUntilBracket(cur,s)!=1)
                return SYNTAX_ERROR;
            if (i1||(s.size()>=64))
                return SYNTAX_ERROR;
            else
            {
                i1=1;
                for (size_t i=0;i<s.size();i++)
                journal[i]=s[i];
                cur+=s.size();
                cur+=skip(cur);
		        x=0;
                s.clear();
                continue;
            }
        }
        x=0;
        sscanf(cur,"year=[%n",&x);
        if (x!=0)
        {
            if (i2)
                return SYNTAX_ERROR;
            else
            {
                i2=1;
                cur+=x;
                cur+=skip(cur);
                x=0;
                sscanf(cur,"%d%n",&year,&x);
                if (x==0)
                    return SYNTAX_ERROR;
                cur+=x;
                cur+=skip(cur);
                if (cur[0]!=']')
                    return SYNTAX_ERROR;
                cur++;
                cur+=skip(cur);
                continue;
            }
        }
        x=0;
        sscanf(cur,"number=[%n",&x);
        if (x!=0)
        {
            if (i3)
                return SYNTAX_ERROR;
            else
            {
                i3=1;
                cur+=x;
                cur+=skip(cur);
                x=0;
                sscanf(cur,"%d%n",&number,&x);
                if (x==0)
                    return SYNTAX_ERROR;
                cur+=x;
                cur+=skip(cur);
                if (cur[0]!=']')
                    return SYNTAX_ERROR;
                cur++;
                cur+=skip(cur);
                continue;
            }
        }
        x=0;
        sscanf(cur,"author=[%n",&x);
        if (x!=0)
        {
            cur+=x;
            cur+=skip(cur);
            std::vector<char> s;
            if (readUntilBracket(cur,s)!=1)
                return SYNTAX_ERROR;
            if (i4||(strlen(author)>=32))
                return SYNTAX_ERROR;
            else
            {
                i4=1;
                for (size_t i=0;i<s.size();i++)
                author[i]=s[i];
                cur+=s.size();
                cur+=skip(cur);
                s.clear();
                continue;
            }
        }
        x=0;
        sscanf(cur,"article=[%n",&x);
        if (x!=0)
        {
            cur+=x;
            cur+=skip(cur);
            std::vector<char> s;
            if (readUntilBracket(cur,s)!=1)
                return SYNTAX_ERROR;
            if (i5||(strlen(article)>=128))
                return SYNTAX_ERROR;
            else
            {
                i5=1;
                for (size_t i=0;i<s.size();i++)
                article[i]=s[i];
                cur+=s.size();
                cur+=skip(cur);
                s.clear();
                continue;
            }
        }
        return SYNTAX_ERROR;
    }
    if (cur[0]!='\0' && cur[0]!='\n')
	    return SYNTAX_ERROR;
    insertNote_=Note(journal,year,number,author,article);
    type_=INSERT;
    return SUCCESS;
}

Result_Status Query::QFilter(const char *cur_)
{
    const char * cur=cur_;
    int x=0;
    int from;
    Filter filter;
    while(cur[0]!='\0' && cur[0]!='\n')
    {
        Result_Status r1=NO_RESULT, r2=NO_RESULT , r3=NO_RESULT;
        cur=GetYearFilter(cur,filter,r1);
        if (r1!=SUCCESS && r1!=NO_RESULT)
            return r1;
        cur=GetNumberFilter(cur,filter,r2);
        if (r2!=SUCCESS && r2!=NO_RESULT)
            return r2;
        cur=GetAuthorFilter(cur,filter,r3);
        if (r3!=SUCCESS && r3!=NO_RESULT)
            return r3;
        if (r1==NO_RESULT && r2==NO_RESULT && r3==NO_RESULT )
            break;
    }
    cur+=skip(cur);
    x = 0;
    sscanf(cur,"from%n",&x);
    if(x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        x=0;
        sscanf(cur,"%d%n",&from,&x);
        if (x!=0)
        {
            cur+=x;
            cur+=skip(cur);
            if (cur[0]!='\0' && cur[0]!='\n')
                return INCORRECT_NUMBER_OF_FROM;
            if (from==-1)
                return INCORRECT_NUMBER_OF_FROM;
            from_=from;
            type_=SELECT_FROM;
        }
        else
        {
            return INCORRECT_NUMBER_OF_FROM;
        }
    }
    cur+=skip(cur);
    if (cur[0]!='\0' && cur[0]!='\n')
    {
        return INCORRECT_FILTER;
    }
    filter_=filter;
    return SUCCESS;
}

Result_Status Query::parse(const char *str)
{
    const char *cur=str;
    cur+=skip(cur);
    int x=0;
    Result_Status k;
    sscanf(cur,"select%n",&x);
    if (x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        k=QFilter(cur);
        if (k==SUCCESS && type_!=SELECT_FROM)
            type_=SELECT;
        return k;
    }
    sscanf(cur,"clear%n",&x);
    if (x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        if(cur[0]!='\0' && cur[0]!='\n')
            return SYNTAX_ERROR;
        type_=CLEAR;
        return SUCCESS;
    }
    sscanf(cur,"delete%n",&x);
    if(x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        k=QFilter(cur);
        if (type_==SELECT_FROM)
            return SYNTAX_ERROR;
        if (k==SUCCESS)
            type_=DELETE;
        return k;
    }
    sscanf(cur,"insert%n",&x);
    if(x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        k=Insert(cur);
        return k;
    }
    sscanf(cur,"save%n",&x);
    if(x!=0)
    {
        cur+=x;
        cur+=skip(cur);
        if(cur[0]!='\0' && cur[0]!='\n')
            return SYNTAX_ERROR;
        type_=SAVE;
        return SUCCESS;
    }
    return UNKNOWN_QUERY_TYPE;       
}

int skip(const char* cur)
{
    int i=0;
    while(cur[i]==' ')
        i++;
    return i;
}

//enum QueryType{NO, SELECT, SELECT_FROM, DELETE, INSERT, SAVE, CLEAR};

void printQueryType(FILE *fo, QueryType type)
{
    if (type==NO)
    {
        fprintf(fo,"No type\n");
    }
    if (type==SELECT)
    {
        fprintf(fo,"select\n");
    }
    if (type==SELECT_FROM)
    {
        fprintf(fo,"select from\n");
    }
    if (type==DELETE)
    {
        fprintf(fo,"delete\n");
    }
    if (type==INSERT)
    {
        fprintf(fo,"insert\n");
    }
    if (type==SAVE)
    {
        fprintf(fo,"save\n");
    }
    if (type==CLEAR)
    {
        fprintf(fo,"clear\n");
    }
}

//enum Result_Status{NO_RESULT,SUCCESS, SYNTAX_ERROR, UNKNOWN_QUERY_TYPE, INCORRECT_FILTER, INCORRECT_NUMBER_OF_FROM, CANT_OPEN, WRITING_ERROR }; 

void printResult_Status(FILE *fo, Result_Status status)
{
    if (status==NO_RESULT)
    {
        fprintf(fo,"NO_RESULT\n");
    }
    if (status==SUCCESS)
    {
        fprintf(fo,"SUCCESS\n");
    }
    if (status==SYNTAX_ERROR)
    {
        fprintf(fo,"SYNTAX_ERROR\n");
    }
    if (status==INCORRECT_FILTER)
    {
        fprintf(fo,"INCORRECT_FILTER\n");
    }
    if (status==UNKNOWN_QUERY_TYPE)
    {
        fprintf(fo,"UNKNOWN_QUERY_TYPE\n");
    }
    if (status==INCORRECT_NUMBER_OF_FROM)
    {
        fprintf(fo,"INCORRECT_NUMBER_OF_FROM\n");
    }
    if (status==CANT_OPEN)
    {
        fprintf(fo,"CANT_OPEN\n");
    }
    if (status==WRITING_ERROR)
    {
        fprintf(fo,"WRITING_ERROR\n");
    }
}