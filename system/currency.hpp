#pragma once
#include<string>
#include<algorithm>
#include<iostream>
#include<sstream>
int _str_digit_to_int_(std::string str){    
    str.erase(std::remove_if(str.begin(),str.end(),
    [](unsigned char c){return !std::isdigit(c);}
    ));
    size_t pos;
    return std::stoi(str.c_str(),&pos); 
}
void _str_to_currency(std::string& str){
    if(str[0]=='0')
        for(char &c:str){
            if(c=='0')
                str.erase(str.begin());
            else break;
        }
    size_t comma = str.find(',');
    if(comma==std::string::npos){
        str+=",00";
    }
    else{
        if(str.size()<=comma+1){
            str+="00";
        }
        else if(str.size()<=comma+2){
            str+="0";
        }
        if(str.size()>comma+3)
            str.erase(comma,comma+2);
    }
}
struct currency{
    int value=0;
    int real=0;
    int cent=0;
    currency(std::string initial_value){
        value = _str_digit_to_int_(initial_value);
        real = value/100;
        cent = value-real*100;
    };
    currency(int initial_value):value(initial_value){
        real = value/100;
        cent = value-real*100;
    };
    currency(int real, int cent){
        std::stringstream ss;
        ss << real << (cent<10?"0":"")+std::to_string(cent);
        ss >> value; 
    };

    int add(int other_value){
        currency _temp_c(other_value);
        real+=_temp_c.real;
        cent+=_temp_c.cent;
        if(cent>99){
            real++;
            cent-=100;
        }
        return value;
    }

    void reset(){value=real=cent=0;}
    bool operator >(const currency& OtherCurrency){
        return value>OtherCurrency.value;
    }
    bool operator <(const currency&OtherCurrency){
        return value<OtherCurrency.value;
    }
};

std::string _currency_to_str(int value){
    currency c(value);
    std::ostringstream str;
    str << c.real << (c.cent<10?",0":",") << c.cent;
    return str.str();
}