#pragma once
#include<imgui.h>
#include<imgui_stdlib.h>
#include<string>
#include"../category.hpp"
#include"../product.hpp"
#include"../disk.hpp"
bool get_product_from_database(void *data, int idx, const char **out_text){
    product *p = (product*)data;
    *out_text = p[idx].name.c_str();
    return true;
}
void erase_category(const int ID){
    auto where = (Category::categories.begin()+ID);

    for(auto const &p:where->products){
        p->category_id = -1;
    }
    Category::categories.erase(where);
    NOTIFY_UPDATE;
}
bool initialise_category = false;
static Category* category_edit  = nullptr;
void _DRAW_CREATE_CATEGORY_PANEL_(Category *CATEGORY = nullptr){
    static float color[3] = {9.f/255.f,1,1};
    if(initialise_category&&CATEGORY){
        ImGui::ColorConvertRGBtoHSV(CATEGORY->color.x,CATEGORY->color.y,CATEGORY->color.z,color[0],color[1],color[2]);
        initialise_category = false;
    }else{
        //category_edit = nullptr;
    }
    static std::string name;
    static std::vector<int> idxs;
    static float* SESSION_COLOR;
    ImGui::Text("NOME:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##name",&(CATEGORY?CATEGORY->name:name),ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine();
    if(CATEGORY){
        if(ImGui::ImageButton(next_icon,icon_size())){
            category_edit = &Category::categories[CATEGORY->id+1<Category::categories.size()?CATEGORY->id+1:0];
            initialise_category = true;
        }
        ImGui::SameLine();
    }
    ImGui::ColorEdit3("DESTAQUE",color,ImGuiColorEditFlags_HDR|ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_InputHSV|ImGuiColorEditFlags_Uint8|ImGuiColorEditFlags_PickerHueWheel|ImGuiColorEditFlags_NoOptions|ImGuiColorEditFlags_NoBorder);
    if(CATEGORY)
        ImGui::ColorConvertHSVtoRGB(color[0],color[1],color[2],CATEGORY->color.x,CATEGORY->color.y,CATEGORY->color.z);
    static int s;
    ImGui::Text("SELECIONE");
    static int SELECT_HEIGHT = icon_size().y*6;
    if(ImGui::BeginChild("SELECIONAR",ImVec2(0,SELECT_HEIGHT))){
        ImGui::Columns(2);
        bool any_selected = false;
        if(ImGui::BeginListBox("##SELECIONAR",ImVec2(-1,SELECT_HEIGHT))){
            
            for(auto &p:product::storage){
                if(p.category_id!=-1)
                    continue;
                if(CATEGORY){
                    if(CATEGORY->id!=p.category_id)
                        ImGui::Selectable(p.name.c_str(),&p.is_selected);
                }else{
                    for(const int idx:idxs){
                        if(idx==p.id){
                            goto here;
                        }
                    }
                    ImGui::Selectable(p.name.c_str(),&p.is_selected);
                }
                if(!any_selected) any_selected = p.is_selected;
                here:;
            }
            ImGui::EndListBox();
        }
        ImGui::NextColumn();
        ImGui::SameLine(0,21);
        bool clicked = false;
        ImGui::BeginDisabled(!any_selected);
        //ImGui::PushID(1);
        clicked = ImGui::ImageButton(next_icon,icon_size());
        //ImGui::PopID();
        ImGui::EndDisabled();
        if(clicked){
            idxs.clear();
            for(auto &p:product::storage){
                if(p.is_selected){
                    if(CATEGORY){
                        change_product_category(&p,CATEGORY->id);
                    }else{
                        idxs.push_back(p.id);
                    }
                    p.is_selected = false;
                }
            }
            NOTIFY_UPDATE;
        }
        ImGui::SameLine();
        if(ImGui::BeginChild("PRODUCTS",ImVec2(0,icon_size().y*6),true))
        {
            if(CATEGORY){
                for(const auto& p:CATEGORY->products){
                    ImGui::Text(p->name.c_str());
                    ImGui::SameLine();
                    ImGui::PushID(p->id);
                    if(ImGui::ImageButton(delete_icon,icon_size(-10))){
                        change_product_category(p,-1);
                    }
                    ImGui::PopID();
                }
            }else{
                int _ITR_ = 0;
                for(const int IDX:idxs){
                    ImGui::Text(product::storage[IDX].name.c_str());
                    ImGui::SameLine();
                    ImGui::PushID(IDX);
                    bool clicked = ImGui::ImageButton(delete_icon,icon_size(-10));
                    ImGui::PopID();
                    if(clicked){
                        idxs.erase(idxs.begin()+_ITR_);
                        break;
                    }
                    _ITR_++;
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    if(!CATEGORY){
        if(ImGui::Button("+CRIAR TAG")){
            float R,G,B;
            ImGui::ColorConvertHSVtoRGB(color[0],color[1],color[2],R,G,B);
            Category::categories.push_back(Category(name,ImVec4(R,G,B,1),idxs));
            for(const int IDX:idxs){
                product::storage[IDX].category_id = Category::s_counter-1;
            }
            save();
            idxs.clear();
        }
    }else{
        if(button_confirmation("DELETAR TAG")){
            erase_category(CATEGORY->id);
            NOTIFY_UPDATE;
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("FECHAR")){
            NOTIFY_UPDATE;
            ImGui::CloseCurrentPopup();
        }
    }
    if(!ImGui::IsAnyItemFocused()&&!ImGui::IsAnyItemActive()&&CATEGORY){
        if(ImGui::IsKeyPressed(ImGuiKey_RightArrow,true))
            category_edit = &Category::categories[CATEGORY->id+1<product::storage.size()?CATEGORY->id+1:0];
        else if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow,true))
            category_edit = &Category::categories[CATEGORY->id-1>=0?CATEGORY->id-1:Category::categories.size()-1];
        initialise_category = true;
    }
}
