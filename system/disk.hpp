#pragma once
#include<fstream>
#include<string>
#include<sstream>
#include"product.hpp"
#include<iostream>
#include"category.hpp"
void load(){
    std::ifstream data("data.txt");
    if(data.is_open()){
        std::string line;
        std::string name,validate,price;
        int quantity,category;
        bool format_as_MMYYYY;
        while(std::getline(data,line)){
            std::istringstream stream(line);
            stream >> name;
            stream >> quantity;
            stream >> price;
            stream >> validate;
            stream >> format_as_MMYYYY;
            stream >> category;
            product::store(name,validate,quantity,price,format_as_MMYYYY,category);
        }
    }
    data.close();
    data.open("_category.txt");
    if(data.is_open()){
        std::string line;
        while(std::getline(data,line)){
            std::string name;
            int id;
            Uint32 color;
            std::vector<int> products;
            std::istringstream stream(line);

            stream >> name;
            stream >> color;
            int psize;
            stream >> psize;
            for(int i = 0; i < psize; i++){
                stream >> id;
                products.push_back(id);
            }
            Category::categories.push_back(Category(name,ImGui::ColorConvertU32ToFloat4(color),products));
        }
    }
    data.close();
}
void save(){
    std::ofstream data("data.txt");
    if(data.is_open()){
        for(const auto&p:product::storage){
            std::string name = p.name;
            std::replace(name.begin(),name.end(),' ', '_');
            data << name << " " << p._quantity << " " << p._price << " " << p._validate << " " << p.format_as_MMYYYY << " " << p.category_id << "\n";
        }
    }
    data.close();
    data.open("_category.txt");
    if(data.is_open()){
        for(const auto c:Category::categories){
            data << c.name << " " << ImGui::ColorConvertFloat4ToU32(c.color) << " " << c.products.size();
            for(const auto p:c.products){
                data << " " << p->id; 
            }
            data << "\n";
        }
    }
    data.close();
}