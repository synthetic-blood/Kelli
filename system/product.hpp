#pragma once
#include<string>
#include<vector>
#include"date.hpp"
#include"currency.hpp"
#include"gui/extend.hpp"
#include<algorithm>
extern time_t now;

struct product: SYSTEM_GUI_EXTENSION{
    static int s_counter;
    int id = 0;
    int sort_id = 0;
    std::string name;
    std::string _validate;
    int _quantity = 1;
    std::string _price;
    int category_id = -1;
    bool is_valid = true;
    bool expire = true;
    bool format_as_MMYYYY = true;
    int package = 0;
    void check_validate();
    
    static std::vector<product> storage;
    static void store(std::string name, std::string validate, int quantity, std::string price, bool format_as_MMYYYY, int category_id);
    
    int sell_quantity=1;
    
    product (std::string name, std::string validate, int quantity, std::string price, const bool format_as_MMYYYY, int category_id):_validate(validate),_quantity(quantity),format_as_MMYYYY(format_as_MMYYYY),category_id(category_id){
        std::replace(name.begin(),name.end(),'_',' ');
        this->name = name;
        _price = price;
        _str_to_currency(_price);
        if(_price.compare("R$")==0)
            _price+="R$";
        id = s_counter;
        s_counter++;
    }
    product* ptr(){
        return this;
    }

};
int product::s_counter = 0;
std::vector<product> product::storage;
void product::store(std::string name, std::string validate, int quantity, std::string price, bool format_as_MMYYYY,int category_id){
    storage.push_back(product(name,validate,quantity,price,format_as_MMYYYY,category_id));
}
void product::check_validate(){
    if(_validate=="*") return;
    tm __validate={0};
    str_to_tm((std::string(format_as_MMYYYY?"01/":"")+_validate).c_str(),&__validate);
    is_valid = now<=mktime(&__validate);
}