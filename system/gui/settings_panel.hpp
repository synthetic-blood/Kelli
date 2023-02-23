#pragma once
#include<imgui.h>
#include"../variables.hpp"
#include"variables.hpp"
#include<sstream>
#include<fstream>
#include"../product.hpp"
#include"../lucro.hpp"
bool open_settings = true;

void _DRAW_MAIN_SETTINGS_PANEL(){
    static bool b = true;
    ImGui::Checkbox("SEMPRE PERGUNTAR ANTES DE DELETAR",&always_ask);
    
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0,0,0,0));


    if(button_confirmation("LIMPAR PRODUTOS/TAGS")){
        product::storage.clear();
        NOTIFY_UPDATE;
    }
    if(button_confirmation("LIMPAR REGISTRO DE VENDAS")){
        lucro::total_of_the_day = 0;
        lucro::historico.clear();
        lucro::registrar();
    }

    if(ImGui::Button("SOBRE")){
        ImGui::OpenPopup("SOBRE");
        b = true;
    }
    ImGui::PopStyleColor(2);
    if(ImGui::BeginPopupModal("SOBRE",&b,ImGuiWindowFlags_NoResize)){
        const int offsetX = 60;
        ImGui::Image(view_texture[2],icon_size(20));
        ImGui::SameLine();
        ImGui::SetCursorPosX(offsetX);
        ImGui::PushFont(SYSTEM_FONT_DEFAULT_BIG);
        ImGui::Text("Kelli");
        ImGui::PopFont();
        ImGui::SetCursorPos(ImVec2(offsetX,ImGui::GetCursorPosY()-15));
        ImGui::PushFont(SYSTEM_FONT_ITALIC);
        ImGui::Text("versão: nano\nbuild: release 1 112022");
        ImGui::PopFont();

        ImGui::PushFont(SYSTEM_FONT_BOLD);
        ImGui::SetCursorPosX(offsetX);
        ImGui::Text("FUCHS");
        ImGui::PopFont();
        ImGui::EndPopup();
    }
}
namespace settings{
    void registrar(){            
        std::ofstream data("settings.txt");
        data << always_ask << "\n";
        data.close();
    }
    void carregar(){
        std::ifstream data("settings.txt");
        data >> always_ask;
        data.close();
    }
}