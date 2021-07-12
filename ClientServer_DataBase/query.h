#ifndef _QUERY_H_
#define _QUERY_H_

#include"library.h"
#include<vector>
#include<list>

class Database;

int skip(const char* cur);

enum QueryType{NO, SELECT, SELECT_FROM, DELETE, INSERT, SAVE, CLEAR};
void printQueryType(FILE *fo, QueryType type);

enum Result_Status{NO_RESULT,SUCCESS, SYNTAX_ERROR, UNKNOWN_QUERY_TYPE, INCORRECT_FILTER, INCORRECT_NUMBER_OF_FROM, CANT_OPEN, WRITING_ERROR };
void printResult_Status(FILE *fo, Result_Status status);

struct Year_Interval
{
    int yearmin_, yearmax_;
    Year_Interval(int yearmin=1869, int yearmax=2020):yearmin_(yearmin),yearmax_(yearmax)
    {}
};

struct Number_Interval
{
    int nummin_, nummax_;
    Number_Interval(int nummin=1, int nummax=500):nummin_(nummin),nummax_(nummax)
    {}
};

struct Author
{
    char author_[32];
    Author(const char *authorv)
    {
        strncpy(author_,authorv,32);
    }
};

class Filter
{
    private:
        std::list<Author> authors_;
        std::list<Year_Interval> years_;
        std::list<Number_Interval> numbers_;
    public:
        const std::list<Author> & Authors() const
        {
            return authors_;
        } 
        const std::list<Year_Interval> & Years() const
        {
            return years_;
        }
        const std::list<Number_Interval> & Numbers() const
        {
            return numbers_;
        }
        void AddYears(Year_Interval new_year_interval)
        {
            years_.push_back(new_year_interval);
        }
        void AddNumbers(Number_Interval new_number_interval)
        {
            numbers_.push_back(new_number_interval);
        }
        void AddAuthor(Author new_author)
        {
            authors_.push_back(new_author);
        }
};


class Query
{
    private:
        QueryType type_;
        Filter filter_;
        Note insertNote_;
        int from_;
        Result_Status QFilter(const char* cur_);
        Result_Status Insert(const char* cur_);
        const char * GetYearFilter(const char *cur_, Filter &filter,Result_Status &r);
        const char * GetNumberFilter(const char *cur_, Filter &filter,Result_Status &r);
        const char * GetAuthorFilter(const char *cur_, Filter &filter,Result_Status &r);
    public:
        Query():type_(NO),from_(-1)
        {}
        Result_Status parse(const char* str);
        QueryType Type() const
        {
            return type_;
        }
        const Filter & GetFilter() const
        {
            return filter_;
        }
        Note InsertNote() const
        {
            return insertNote_;
        }
        int From() const
        {
            return from_;
        }
};

#endif