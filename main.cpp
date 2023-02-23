#include"fuchs/nano_sdl_wrapper.hpp"
#include<imgui.h>
#include<imgui_impl_sdl.h>
#include<imgui_impl_sdlrenderer.h>
#include<imgui_stdlib.h>
#include<iostream>

#include<chrono>
#include<thread>
#include"system/gui/table.hpp"
#include"system/gui/sell_panel.hpp"
#include"system/category.hpp"
#include"system/lucro.hpp"
#include<Windows.h>
//FIX
//product category id does not update/points/shift  (_UPDATE_DATABASE)
int main(int argc, char *argv[]){
    lucro::carregar();
    settings::carregar();
    load();
    
    nano window("kelli");
    load_system_texture(window.p_renderer);
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
        
    SYSTEM_FONT_DEFAULT = io.Fonts->AddFontFromFileTTF("resources/fonts/GoldbillXlLight-VG7zx.otf",27.f);
    SYSTEM_FONT_DEFAULT_BIG = io.Fonts->AddFontFromFileTTF("resources/fonts/GoldbillXlLight-VG7zx.otf",30.f);
    SYSTEM_FONT_ITALIC = io.Fonts->AddFontFromFileTTF("resources/fonts/GoldbillXlLightItalic-JREJo.otf",27.f);
    SYSTEM_FONT_ITALIC_BIG = io.Fonts->AddFontFromFileTTF("resources/fonts/GoldbillXlLightItalic-JREJo.otf",45.f);
    SYSTEM_FONT_BOLD = io.Fonts->AddFontFromFileTTF("resources/fonts/GoldbillXsExtrabold-GO0lP.otf",36.f);

    ImGui_ImplSDL2_InitForSDLRenderer(window.p_window,window.p_renderer);
    ImGui_ImplSDLRenderer_Init(window.p_renderer);

    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]=ImVec4(0,0,0,1);
    ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]=ImVec4(0.05f,0.05f,0.05f,255);
    ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]=ImVec4(0,0,0,1);
    ImGui::GetStyle().Colors[ImGuiCol_Button]=ImVec4(1,0.2f,0,0.81f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]=ImVec4(1,0.1f,0,0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]=ImVec4(1,0.1f,0,0.6f);
    ImGui::GetStyle().Colors[7] = ImVec4(0.1f,0.1f,0.1f,1);
    
    ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(1,0.1f,0,0.2f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f,0.2f,0.2f,0.6f);//hovered
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(1,0.1f,0,0.3f);
    ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1,0.2,0.2,0.1);
    force_sort = true;
    ImGui::GetStyle().AntiAliasedFill = true;
    ImGui::GetStyle().AntiAliasedLinesUseTex = true;
    while(window.p_window){
        time(&now);
        
        window.process(&ImGui_ImplSDL2_ProcessEvent);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(window.p_window);
        
        ImGui::NewFrame();
        
        if(SYSTEM_UPDATED){
            force_sort = true;
            _UPDATE_DATABASE();
            save();
            CLEAR_STATE;
        }

        //ImGui::AcceptDragDropPayload("A");
        ImGui::SetWindowPos("SYSTEM",ImVec2(0,0));
        ImGui::SetWindowSize("SYSTEM",ImVec2(window.getSize().x,window.getSize().y));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
        bool openned = ImGui::Begin("SYSTEM",0,ImGuiWindowFlags_NoBackground|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleVar();
        if(openned){
            
            if(ImGui::BeginTable("KELLI",2,ImGuiTableFlags_Resizable)){
                ImGui::TableSetupColumn("VENDAS");
                ImGui::TableSetupColumn("INVENTÁRIO");

                ImGui::PushStyleColor(ImGuiCol_TableHeaderBg,ImVec4(0,0,0,0));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(0,0,0,0));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive,ImVec4(0,0,0,0));
                ImGui::TableHeadersRow();
                ImGui::PopStyleColor(3);

                ImGui::TableNextColumn();
                ImGui::BeginGroup();
                _DRAW_MAIN_SELL_PANEL_();
                ImGui::EndGroup();
                
                ImGui::TableNextColumn();
                ImGui::BeginGroup();
                _DRAW_MAIN_TABLE_();
                ImGui::EndGroup();

                ImGui::EndTable();
            }
            ImGui::End();
        }
        
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        
        window.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(18));
    }
    save();
    settings::registrar();
    ImGui::DestroyContext();

    return 0;
}