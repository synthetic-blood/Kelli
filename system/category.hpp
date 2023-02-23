#pragma once
#include<vector>
#include<imgui.h>
#include<string>
struct Category:SYSTEM_GUI_EXTENSION{
    int id = 0;
    static int s_counter;
    ImVec4 color;
    std::string name;
    std::vector<product*> products;
    static std::vector<Category> categories;
    Category(std::string name, ImVec4 color, const std::vector<int> idxs):name(name),color(color){
        is_selected = true;
        for(const auto X:idxs){
            products.push_back(&product::storage[X]);
        }
        id = s_counter;
        s_counter++;
    }
};
int Category::s_counter = 0; 
std::vector<Category> Category::categories;
 