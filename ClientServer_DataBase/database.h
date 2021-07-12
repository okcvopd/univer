#ifndef _DATABASE_H_
#define _DATABASE_H_

#include"query.h"

#include<map>

class Result
{
    private:
        Result_Status status_;
        size_t total_;
        std::list<Note> resNotes_;
    public:
        Result():status_(SUCCESS),total_(0)
        {}
        Result_Status Status() const
        {
            return status_;
        }
        const std::list<Note>& resultNotes() const
        {
            return resNotes_;
        }
        size_t Total() const
        {
            return total_;
        }
        void TotalChange(size_t new_total )
        {
            total_=new_total;
        }
        void IncreaseTotal()
        {
            total_++;
        }
        void StatusChange(Result_Status new_status)
        {
            status_=new_status;
        }
        void Add(Note new_note)
        {
            resNotes_.push_back(new_note);
        }
        void popFront()
        {
            resNotes_.pop_front();
        }
        void print(FILE *fo) const;
};

class Session
{
    private:
        std::map<int,Result> results_;
        int next;
    public:
        Session():next(1)
        {}
        void Clear()
        {
            results_.clear();
        }
        const std::map<int,Result>& Results() const
        {
            return results_;
        }
        const Result & GetResult(int number) 
        {
            return results_[number];
        }
        int Next() const
        {
            return next;
        }
        void NextIncrease()
        {
           next++; 
        }
        void NewResult(Result new_result)
        {
            results_[next]=new_result;
        }
};

class database
{
    private:
        std::list<Note> Notes_;
        Session session_;
        char *file_;
        void Select(const Query& query,Result& result);
        void SelectFrom(const Query& query,Result& result);
        void Select(const Query& query,Result& result,Session &session);
        void SelectFrom(const Query& query,Result& result,Session &session);
        void Delete(const Query& query,Result& result);
        void print() const;
    public:
        database():file_(NULL)
        {}
        const Session& session() const
        {
            return session_;
        }
        const std::list<Note> Notes() const
        {
            return Notes_;
        }
        const char* File() const
        {
            return file_;
        }
        Result perform (const Query& query);
        Result perform (const char *str);
        Result perform (const char *str, Session &session);
        Result perform (const Query& query, Session &session);
        Result_Status Save();
        void readDataBase(const char *fi);
};
#endif