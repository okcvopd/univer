#include"database.h"

int printHead(FILE *fo);
void Result::print(FILE *fo) const
{
    if (status_!=SUCCESS)
    {
        printResult_Status(fo,status_);
        return;
    }
    if (resNotes_.size()!=0)
        printHead(fo);
    for(std::list<Note>::const_iterator i=resNotes_.begin(); i!=resNotes_.end(); i++) 
        i->printNote(fo);
    fprintf(fo,"Total: %lu\n",total_);
}

Result database::perform (const Query& query)
{
    Result result;
    if(query.Type() == SELECT) {
        Select(query, result);
        return result;
    }

    if(query.Type() == SELECT_FROM) {
        SelectFrom(query, result);
        return result;
    }

    if(query.Type() == DELETE) {
        Delete(query, result);
        return result;
    }

    if(query.Type() == INSERT) {
        Notes_.push_back(query.InsertNote());
        result.TotalChange(Notes_.size());
        return result;
    }

    if(query.Type() == CLEAR) {
        result.TotalChange( session_.Results().size());
        session_.Clear();
        return result;
    }

    if(query.Type()==SAVE) {
        result.StatusChange(Save());
        result.TotalChange(Notes_.size());
        return result;
    }

    result.StatusChange(UNKNOWN_QUERY_TYPE);

    return result;
}

Result database::perform (const char *str)
{
    Query q;
    Result_Status parse=q.parse(str);
    if (parse!=SUCCESS)
    {
        Result result;
        result.StatusChange(parse);
        return result;
    }
    return perform(q);
}

Result database::perform (const char *str, Session &session)
{
    Query q;
    Result_Status parse=q.parse(str);
    if (parse!=SUCCESS)
    {
        Result result;
        result.StatusChange(parse);
        return result;
    }
    return perform(q,session);
}

Result database::perform (const Query& query, Session &session)
{
    Result result;
    if(query.Type() == SELECT) {
        Select(query, result, session);
        return result;
    }

    if(query.Type() == SELECT_FROM) {
        SelectFrom(query, result, session);
        return result;
    }

    if(query.Type() == DELETE) {
        Delete(query, result);
        return result;
    }

    if(query.Type() == INSERT) {
        Notes_.push_back(query.InsertNote());
        result.TotalChange(Notes_.size());
        return result;
    }

    if(query.Type() == CLEAR) {
        result.TotalChange( session.Results().size());
        session.Clear();
        return result;
    }

    if(query.Type()==SAVE) {
        result.StatusChange(Save());
        result.TotalChange(Notes_.size());
        return result;
    }

    result.StatusChange(UNKNOWN_QUERY_TYPE);

    return result;
}

void database::Select(const Query& query,Result& result)
{
    bool checkyear=1, checknum=1, checkauth=1;
    if (query.GetFilter().Years().size()==0)
        checkyear=0;
    if (query.GetFilter().Numbers().size()==0)
        checknum=0;
    if (query.GetFilter().Authors().size()==0)
        checkauth=0;
    for(std::list<Note>::iterator i=Notes_.begin(); i!=Notes_.end(); i++)
    {
        bool year=!checkyear, number=!checknum, author=!checkauth;
        if (checkyear)
            for (std::list<Year_Interval>::const_iterator j=query.GetFilter().Years().cbegin(); j != query.GetFilter().Years().cend() ; j++)
            {
                if ((i->getYear()<=j->yearmax_) && (i->getYear()>=j->yearmin_))
                {
                    year=1;
                    break;
                }
            }
        
        if (checknum)
            for (std::list<Number_Interval>::const_iterator j=query.GetFilter().Numbers().cbegin(); j != query.GetFilter().Numbers().cend() ; j++)
            {
                if ((i->getNumber()<=j->nummax_) && (i->getNumber()>=j->nummin_))
                {
                    number=1;
                    break;
                }
            }

        if (checkauth)
            for (std::list<Author>::const_iterator j=query.GetFilter().Authors().cbegin(); j != query.GetFilter().Authors().cend() ; j++)
            {
                if (i->compareAuthor(j->author_))
                {
                    author=1;
                    break;
                }
            }

        if (year&&number&&author)
        {
            result.Add(*i);
            result.IncreaseTotal();
        }
    }
    session_.NewResult(result);
    session_.NextIncrease();
}

void database::Select(const Query& query,Result& result, Session &session)
{
    bool checkyear=1, checknum=1, checkauth=1;
    if (query.GetFilter().Years().size()==0)
        checkyear=0;
    if (query.GetFilter().Numbers().size()==0)
        checknum=0;
    if (query.GetFilter().Authors().size()==0)
        checkauth=0;
    for(std::list<Note>::iterator i=Notes_.begin(); i!=Notes_.end(); i++)
    {
        bool year=!checkyear, number=!checknum, author=!checkauth;
        if (checkyear)
            for (std::list<Year_Interval>::const_iterator j=query.GetFilter().Years().cbegin(); j != query.GetFilter().Years().cend() ; j++)
            {
                if ((i->getYear()<=j->yearmax_) && (i->getYear()>=j->yearmin_))
                {
                    year=1;
                    break;
                }
            }
        
        if (checknum)
            for (std::list<Number_Interval>::const_iterator j=query.GetFilter().Numbers().cbegin(); j != query.GetFilter().Numbers().cend() ; j++)
            {
                if ((i->getNumber()<=j->nummax_) && (i->getNumber()>=j->nummin_))
                {
                    number=1;
                    break;
                }
            }

        if (checkauth)
            for (std::list<Author>::const_iterator j=query.GetFilter().Authors().cbegin(); j != query.GetFilter().Authors().cend() ; j++)
            {
                if (i->compareAuthor(j->author_))
                {
                    author=1;
                    break;
                }
            }

        if (year&&number&&author)
        {
            result.Add(*i);
            result.IncreaseTotal();
        }
    }
    session.NewResult(result);
    session.NextIncrease();
}

void database::SelectFrom(const Query& query,Result& result)
{
    if (query.From()>=session_.Next())
    {
        result.StatusChange(INCORRECT_NUMBER_OF_FROM);
        return;
    }
    try
    {
        session_.Results().at(query.From());
        bool checkyear=1, checknum=1, checkauth=1;
        if (query.GetFilter().Years().size()==0)
            checkyear=0;
        if (query.GetFilter().Numbers().size()==0)
            checknum=0;
        if (query.GetFilter().Authors().size()==0)
            checkauth=0;
        for(std::list<Note>::const_iterator i=session_.GetResult((int)query.From()).resultNotes().begin(); i!=session_.GetResult((int)query.From()).resultNotes().end(); i++)
        {
            bool year=!checkyear, number=!checknum, author=!checkauth;
            if (checkyear)
                for (std::list<Year_Interval>::const_iterator j=query.GetFilter().Years().begin(); j != query.GetFilter().Years().end() ; j++)
                {
                    if ((i->getYear()<=j->yearmax_) && (i->getYear()>=j->yearmin_))
                    {
                        year=1;
                        break;
                    }
                }
        
            if (checknum)
                for (std::list<Number_Interval>::const_iterator j=query.GetFilter().Numbers().begin(); j != query.GetFilter().Numbers().end() ; j++)
                {
                    if ((i->getNumber()<=j->nummax_) && (i->getNumber()>=j->nummin_))
                    {
                        number=1;
                        break;
                    }
                }

            if (checkauth)
                for (std::list<Author>::const_iterator j=query.GetFilter().Authors().begin(); j != query.GetFilter().Authors().end() ; j++)
                {
                    if (i->compareAuthor(j->author_))
                    {
                        author=1;
                        break;
                    }
                }

            if (year&&number&&author)
            {
                result.Add(*i);
                result.IncreaseTotal();
            }
        }
        session_.NewResult(result);
        session_.NextIncrease();
    }
    catch(const std::out_of_range &ex)
    {
        result.StatusChange(INCORRECT_NUMBER_OF_FROM);
    }
}

void database::SelectFrom(const Query& query,Result& result, Session &session)
{
    if (query.From()>=session.Next())
    {
        result.StatusChange(INCORRECT_NUMBER_OF_FROM);
        return;
    }
    try
    {
        session.Results().at(query.From());
        bool checkyear=1, checknum=1, checkauth=1;
        if (query.GetFilter().Years().size()==0)
            checkyear=0;
        if (query.GetFilter().Numbers().size()==0)
            checknum=0;
        if (query.GetFilter().Authors().size()==0)
            checkauth=0;
        for(std::list<Note>::const_iterator i=session.GetResult((int)query.From()).resultNotes().begin(); i!=session.GetResult((int)query.From()).resultNotes().end(); i++)
        {
            bool year=!checkyear, number=!checknum, author=!checkauth;
            if (checkyear)
                for (std::list<Year_Interval>::const_iterator j=query.GetFilter().Years().begin(); j != query.GetFilter().Years().end() ; j++)
                {
                    if ((i->getYear()<=j->yearmax_) && (i->getYear()>=j->yearmin_))
                    {
                        year=1;
                        break;
                    }
                }
        
            if (checknum)
                for (std::list<Number_Interval>::const_iterator j=query.GetFilter().Numbers().begin(); j != query.GetFilter().Numbers().end() ; j++)
                {
                    if ((i->getNumber()<=j->nummax_) && (i->getNumber()>=j->nummin_))
                    {
                        number=1;
                        break;
                    }
                }

            if (checkauth)
                for (std::list<Author>::const_iterator j=query.GetFilter().Authors().begin(); j != query.GetFilter().Authors().end() ; j++)
                {
                    if (i->compareAuthor(j->author_))
                    {
                        author=1;
                        break;
                    }
                }

            if (year&&number&&author)
            {
                result.Add(*i);
                result.IncreaseTotal();
            }
        }
        session.NewResult(result);
        session.NextIncrease();
    }
    catch(const std::out_of_range &ex)
    {
        result.StatusChange(INCORRECT_NUMBER_OF_FROM);
    }
}

Result_Status database::Save()
{
    int fd;
    fd=open(file_,O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return CANT_OPEN;
    int s=0;
    for (std::list<Note>::iterator i=Notes_.begin(); i!=Notes_.end(); i++)
    {
        s=i->writeBin(fd);
        if (s == -1)
        {
            close(fd);
            return WRITING_ERROR;
        }
    }
    close(fd);
    return SUCCESS;
} 

void database::Delete(const Query& query,Result& result)
{
    bool checkyear=1, checknum=1, checkauth=1;
    result.TotalChange(0);
    if (query.GetFilter().Years().size()==0)
        checkyear=0;
    if (query.GetFilter().Numbers().size()==0)
        checknum=0;
    if (query.GetFilter().Authors().size()==0)
        checkauth=0;
    for(std::list<Note>::iterator i=Notes_.begin(); i!=Notes_.end();)
    {
        bool year=!checkyear, number=!checknum, author=!checkauth;
        if (checkyear)
            for (std::list<Year_Interval>::const_iterator j=query.GetFilter().Years().begin(); j != query.GetFilter().Years().end() ; j++)
            {
                if ((i->getYear()<=j->yearmax_) && (i->getYear()>=j->yearmin_))
                {
                    year=1;
                    break;
                }
            }
        
        if (checknum)
            for (std::list<Number_Interval>::const_iterator j=query.GetFilter().Numbers().begin(); j != query.GetFilter().Numbers().end() ; j++)
            {
                if ((i->getNumber()<=j->nummax_) && (i->getNumber()>=j->nummin_))
                {
                    number=1;
                    break;
                }
            }

        if (checkauth)
            for (std::list<Author>::const_iterator j=query.GetFilter().Authors().begin(); j != query.GetFilter().Authors().end() ; j++)
            {
                if (i->compareAuthor(j->author_))
                {
                    author=1;
                    break;
                }
            }

        if (year&&number&&author)
        {
            i = Notes_.erase(i);
            result.IncreaseTotal();
        }
        else
            i++;
    }
}

void database::readDataBase(const char *fi)
{
    try
    {
        Note temp;
        int fd, status;
        fd=open(fi,O_RDONLY);
        if (fd==-1)
            throw -1;
        while(true)
        {
            status=temp.readBin(fd);
            if (status==0)
                break;
            else
                if (status==-1)
                {
                    Notes_.clear();
                    close(fd);
                    throw -2;
                }
                else
                    Notes_.push_back(temp);
        }
        size_t len=strlen(fi)+1;
        file_=(char*)malloc(len*sizeof(char));       
        if (file_==0)
        {
            throw -3;
        }
        memcpy(file_,fi,len);
        close(fd);
        return;
    }
    catch(int c)
    {
        if (c==-1)
            throw "Cannot open file";
        else
            if (c==-2)
                throw "Cannot read";
            else
                throw "Allocation problem";
    }
}