#pragma once
#include<string>
#include<imgui.h>
std::string product_name, 
            validate,
            price;

int quantity = 1;
time_t      now;

enum SYSTEM_STATE_LIST{
    NO_OPERATION,
    SELL,
    UPDATE
};
enum MONO_OPERATION{
    INITIALIZED,
    STOP
};
uint8_t SYSTEM_STATE = NO_OPERATION;
#define NOTIFY_UPDATE SYSTEM_STATE = UPDATE
#define SYSTEM_UPDATED SYSTEM_STATE == UPDATE
#define CLEAR_STATE SYSTEM_STATE = NO_OPERATION
bool always_ask = true;
bool button_confirmation(const char * button_label){
    bool r = false;
    if(ImGui::Button(button_label)){
        if(always_ask)
            ImGui::OpenPopup(button_label);
        else
            r = true;
    }
    ImGui::PushStyleColor(ImGuiCol_Button,ImGui::GetStyleColorVec4(ImGuiCol_Header));
    if(ImGui::BeginPopupModal(button_label,0,ImGuiWindowFlags_NoResize)){
        ImGui::Checkbox("NÃO PERGUNTAR NOVAMENTE",&always_ask);
        if(ImGui::Button("CONFIRMAR")){
            r = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("CANCELAR")){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();
    return r;
}
