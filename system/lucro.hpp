#pragma once
#include<map>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include"currency.hpp"
namespace lucro{
    int total_of_the_day = 0;
    std::map<std::string,int> historico;
    void registrar(){
        std::ofstream arquivo("data1.txt");
        char buf[20];
        time_t rawtime;
        time(&rawtime);
        tm* date = localtime(&rawtime);
        strftime(buf,20,"%d/%m/%Y",date);
        historico[std::string(buf)] = total_of_the_day;
        arquivo << "TOTAL_OF_THE_DAY " << total_of_the_day << " " << buf << "\n"; 
        for(const auto &r:historico){
            arquivo << r.first << " " << r.second << "\n";
        }
    }
    void carregar(){
        std::ifstream arquivo("data1.txt");
        std::string gravacao, last_register;
        while(std::getline(arquivo,gravacao)){
            std::istringstream gravacao_stream(gravacao);
            std::string key;
            int value;
            gravacao_stream >> key;
            gravacao_stream >> value;
            if(key == "TOTAL_OF_THE_DAY"){
                total_of_the_day = value;
                gravacao_stream >> last_register;
                if(last_register!="*")
                    if(time_to_str(time(0))!=last_register)
                        total_of_the_day = 0;
                continue;
            }
            historico[key] = value;
        }
        arquivo.close();
    }
}