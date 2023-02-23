#pragma once
#include<time.h>
#include<string.h>
#include<string>
std::string getDateNow(){
    char s[80];
    time_t rawtime;
    tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime (s,80,">%d/%m/%Y %H:%M:%S",timeinfo);
    
    return s;
}
int is_str_date_valid(std::string date);
std::string MMYYYY(std::string date){
    const bool MMYYYY = date.size()==7&&date[5]!='/';
    if(MMYYYY){
        date=("01/"+date);
    }
    return date;
}
//format DD/MM/YYYY or MM/YYYY
void str_to_tm(std::string date,tm* _tm){
    *_tm = {0}; // reset
    date = MMYYYY(date);
    if(date.size()==10){
        _tm->tm_mday = atoi(date.substr(0,2).c_str())-1;
        _tm->tm_mon = atoi(date.substr(3,5).c_str())-1;
        _tm->tm_year = atoi(date.substr(6,10).c_str())-1900; 
    }
}
std::string time_to_str(time_t t){
    char buf[20];
    time_t rawtime;
    time(&rawtime);
    tm *date = localtime(&rawtime);
    strftime(buf,20,"%d/%m/%Y",date);
    return std::string(buf);
}
// format: DD/MM/YYYY or MM/YYYY
// retuns:
// -1 on invalid day
// -2 on invalid month
// -3 when mktim() returns -1 (error) 
// -4 on incomplete string
// -5 on expired date
// 0 when no problems occur
int is_str_date_valid(std::string date){
    date = MMYYYY(date);
    if(date.size()==10){
        tm _tm;
        str_to_tm(date,&_tm);
        if(_tm.tm_mday>30&&_tm.tm_mday<0)
            return -1;
        else if(_tm.tm_mon>11&&_tm.tm_mon<0)
            return -2;
        else if(mktime(&_tm)==-1)
            return -3;
        else if(mktime(&_tm)<time(0))
            return -5;
        return 0;
    }
    return -4;
}
