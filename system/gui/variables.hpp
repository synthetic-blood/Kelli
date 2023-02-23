#pragma once
#include<imgui.h>
#include<SDL_render.h>
#include<SDL_image.h>
ImFont *SYSTEM_FONT_DEFAULT,
        *SYSTEM_FONT_DEFAULT_BIG,
        *SYSTEM_FONT_ITALIC,
        *SYSTEM_FONT_ITALIC_BIG,
        *SYSTEM_FONT_BOLD;
ImVec2 icon_size(int mod = 0){
        return ImVec2(ImGui::GetFontSize()+mod,ImGui::GetFontSize()+mod);  
};     
SDL_Texture *warning3_icon, *delete_icon, *add_icon, *next_icon;

SDL_Texture *view_texture[4];
void load_system_texture(SDL_Renderer* renderer){
        warning3_icon = IMG_LoadTexture(renderer,"resources/warning-5-512.png");
        delete_icon = IMG_LoadTexture(renderer,"resources/delete-2-512.png");
        add_icon = IMG_LoadTexture(renderer,"resources/add-512.png");
        next_icon = IMG_LoadTexture(renderer,"resources/account-login-512.png");
        int i=0; 
        for(const char* filename:{"resources/grid-three-up-512.png","resources/list-view-512.png","resources/package-2-512.png","resources/settings-icon-18-512.png"})
        {
                view_texture[i]=IMG_LoadTexture(renderer,filename); i++;
                
        }
}
ImVec4 modImVec4(const ImVec4 &base, const float mod, const bool change_only_alpha = false){
    return change_only_alpha?ImVec4(base.x,base.y,base.z,1.f*mod):ImVec4(base.x*mod,base.y*mod,base.z*mod,1);
}