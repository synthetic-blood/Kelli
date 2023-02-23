#pragma once
#include"variables.hpp"

//SYSTEM
#include"../variables.hpp" 
#include"../date.hpp"
#include"../disk.hpp"
#include"../category.hpp"
#include<imgui_stdlib.h>
extern ImFont *SYSTEM_FONT_DEFAULT,
              *SYSTEM_FONT_ITALIC;
static bool _is_input_valid();
static void _reset_input_values(); 
int _text_input_callback(ImGuiInputTextCallbackData* data);
static int _price_input_callback(ImGuiInputTextCallbackData* data);
static bool enable_validate=true;
static bool initialised = true;
bool initialize_package = true;
static std::string edit_validate;
static int category_selected;
bool is_editing_product = false;
bool sorted = true;
bool force_sort = false;
void change_product_category(product* PRODUCT_PTR, const int ID_TO){
    const int ID_FROM = PRODUCT_PTR->category_id;
    if(sorted)
        PRODUCT_PTR = &product::storage[PRODUCT_PTR->id];
    if(ID_FROM!=-1){
        Category::categories[ID_FROM].products.erase(std::find(Category::categories[ID_FROM].products.begin(),Category::categories[ID_FROM].products.end(),PRODUCT_PTR));
    }
    if(ID_TO!=-1){
        Category::categories[ID_TO].products.push_back(PRODUCT_PTR);
        PRODUCT_PTR->category_id = ID_TO;
    }
    else
        PRODUCT_PTR->category_id = -1;
}
void remove_product_from_category(product* PRODUCT_PTR){
    Category::categories[PRODUCT_PTR->category_id].products.erase(std::find(Category::categories[PRODUCT_PTR->category_id].products.begin(),Category::categories[PRODUCT_PTR->category_id].products.end(),PRODUCT_PTR));
    PRODUCT_PTR->category_id = -1;
}
void add_product_to_category(product* PRODUCT_PTR, const int CATEGORY_ID){
    Category::categories[CATEGORY_ID].products.push_back(PRODUCT_PTR);
    PRODUCT_PTR->category_id = CATEGORY_ID;
}
void add_last_added_product_to_category(const int CATEGORY_ID){
    Category::categories[CATEGORY_ID].products.push_back((product::storage.end()-1)->ptr());
}
void _UPDATE_DATABASE(){
    //UPDATE SESSION ID
    for(int c = 0; c < Category::categories.size(); c++){
        Category::categories[c].id = c;
        for(auto &p:Category::categories[c].products){
            p->category_id = c;
        }
        Category::categories[c].products.clear();
    }
    Category::s_counter = Category::categories.size();
    for(int p = 0; p < product::storage.size(); p++){
        product::storage[p].id = p;
        if(product::storage[p].category_id!=-1&&!Category::categories.empty())
            Category::categories[product::storage[p].category_id].products.push_back(&product::storage[p]);
    }
    product::s_counter = product::storage.size();
}
void erase_product(int ID){
    if(product::storage[ID].category_id!=-1){
        std::vector<product*>*p = &Category::categories[product::storage[ID].category_id].products;
        p->erase(std::find(p->begin(),p->end(),(product::storage.begin()+ID)->ptr()));
    }
    product::storage.erase(product::storage.begin()+ID);
}
static bool format_as_MMYYYY = true;
static bool *SESSION_format_as_MMYYYY = nullptr;
static std::string *SESSION_validate = nullptr;
static product* product_edit = nullptr;
void _DRAW_MAIN_ADD_PANEL_(product* PRODUCT = nullptr){
    SESSION_format_as_MMYYYY = &(PRODUCT?PRODUCT->format_as_MMYYYY:format_as_MMYYYY);
    SESSION_validate = &(PRODUCT?edit_validate:validate);
    is_editing_product = true;
    bool category_empty = Category::categories.empty();
    if(PRODUCT&&initialised){
        category_selected = PRODUCT->category_id==-1?0:PRODUCT->category_id;
        edit_validate.clear();
        enable_validate = PRODUCT->_validate.compare("*");
        if(enable_validate)
            edit_validate = PRODUCT->_validate;
        initialised = false;
    }
    //STYLE
    
    ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.2,0.2,0.2,1));

    ImGui::Text("NOME: ","");
    ImGui::SameLine();
    ImGui::SetCursorPosX(150);
    ImGui::PushFont(SYSTEM_FONT_ITALIC);
    ImGui::SetNextItemWidth(720);
    ImGui::InputTextWithHint("##input(product_name)","DIGITE O NOME DO PRODUTO",&(PRODUCT?PRODUCT->name:product_name),ImGuiInputTextFlags_CharsUppercase);
    if(!PRODUCT){
        ImGui::SameLine();
        if(ImGui::ImageButton(next_icon,icon_size())){
            product::store(product_name,enable_validate?validate:"*",quantity,price,*SESSION_format_as_MMYYYY,category_empty?-1:category_selected);
            if(!category_empty&&category_selected!=-1)
                add_last_added_product_to_category(category_selected);
            NOTIFY_UPDATE;
            product_name.clear();
            quantity = 1;
            validate.clear();
            price.clear();
        }
    }
    //CATEGORY
        
    ImGui::SameLine();

    ImGui::SetNextItemWidth(300);
    if(ImGui::BeginCombo("TAG",category_empty?"...":((PRODUCT?PRODUCT->category_id:category_selected)==-1?"N/A":Category::categories[PRODUCT?(PRODUCT->category_id!=-1?PRODUCT->category_id:0):category_selected].name.c_str()),ImGuiComboFlags_NoArrowButton)){
        if(ImGui::Selectable("N/A")){
            if(PRODUCT){
                remove_product_from_category(PRODUCT);
            }
            else
                category_selected = -1;
        }
        for(auto &c:Category::categories){
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,modImVec4(c.color,0.5f));
            bool clicked = ImGui::Selectable(c.name.c_str());
            ImGui::PopStyleColor();
            if(clicked){
                category_selected = c.id;
                if(PRODUCT){
                    if(PRODUCT->category_id!=c.id){
                        change_product_category(PRODUCT,c.id);
                        PRODUCT->category_id = c.id;
                        NOTIFY_UPDATE;
                        break;
                        }
                    }  
                }
            }
            ImGui::EndCombo();
        }
        //--|
    if((ImGui::IsItemHovered())&&category_empty){
        ImGui::SetTooltip("NENHUMA TAG REGISTRADA");
    }
 
    ImGui::PopFont();

    static float X_INPUT = 0;

    if(ImGui::Checkbox("VALIDADE ",&enable_validate));

    if(enable_validate){
        ImGui::SameLine();
        ImGui::Text("");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("00/00/00000").x+9);
        ImGui::SetCursorPosX(X_INPUT);
        ImGui::PushFont(SYSTEM_FONT_ITALIC);
        ImGui::InputTextWithHint("##input(validate)",(PRODUCT?PRODUCT->format_as_MMYYYY:format_as_MMYYYY)?"MM/AAAA":"DD/MM/AAAA",&(PRODUCT?edit_validate:validate),ImGuiInputTextFlags_CharsDecimal|ImGuiInputTextFlags_CallbackEdit,_text_input_callback, (void*)&validate);
        ImGui::SameLine();
        if(ImGui::Checkbox("MM/AAAA",SESSION_format_as_MMYYYY))
        {
            SESSION_validate->clear();
        }
        ImGui::PopFont();
    }
    if(enable_validate)
    {
        tm ___test_date_vality;
        str_to_tm(PRODUCT?edit_validate:validate,&___test_date_vality);
        if((PRODUCT?edit_validate:validate).length()!=0){
            ImGui::SameLine();
            std::string warning;
            switch(is_str_date_valid(PRODUCT?edit_validate:validate)){
                case -4:
                    warning = "...";
                    break;
                case -1:
                    warning = "dia inválido";
                    break;
                case -2:
                    warning = "mês inválido";
                    break;
                case -3:
                    warning = "ano invalido";
                    break;
                case -5:
                    warning = "data expirada";
                    break;
                case 0:{
                    if(PRODUCT){
                        PRODUCT->_validate = edit_validate;
                    }
                }
                break;
            }
            ImGui::Text("%s",warning.c_str());
        }
    }
    else if(PRODUCT){
        PRODUCT->_validate = "*";
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(30,0));

    //QUANTIDADE
    /*constraint*/ if(quantity<1) quantity=1;
    ImGui::Text("QUANTIDADE: ");
    X_INPUT = ImGui::GetItemRectSize().x+30;//<-links padding 
    ImGui::PopStyleVar();
    ImGui::SameLine();
    ImGui::SetCursorPosX(X_INPUT);
    ImGui::SetNextItemWidth(ImGui::CalcTextSize("000").x+120);
    ImGui::InputInt("##input(quantity)",PRODUCT?&PRODUCT->_quantity:&quantity,1,1,ImGuiInputTextFlags_CharsDecimal);
    //ImGui::SameLine();
    ImGui::Text("PREÇO:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::SetCursorPosX(X_INPUT);
    ImGui::InputText("R$",PRODUCT?&PRODUCT->_price:&price,ImGuiInputTextFlags_CallbackEdit,_price_input_callback);
    if(PRODUCT!=nullptr) _str_to_currency(PRODUCT->_price);
    //ImGui::SetCursorPosX(150);
    ImGui::BeginDisabled(!_is_input_valid());
    if(!PRODUCT) 
        if(ImGui::Button("+ADICIONAR PRODUTO(S)")){
            product::store(product_name,enable_validate?validate:"*",quantity,price,*SESSION_format_as_MMYYYY,category_empty?-1:category_selected);
            if(!category_empty&&category_selected!=-1)
                add_last_added_product_to_category(category_selected);
            NOTIFY_UPDATE;
        }
    
    ImGui::EndDisabled();

    ImGui::Text("%s",getDateNow().c_str());
    
    ImGui::PopStyleColor();

    if(!ImGui::IsAnyItemFocused()&&!ImGui::IsAnyItemActive()&&PRODUCT){
        if(ImGui::IsKeyPressed(ImGuiKey_RightArrow,true)){
            product_edit = &product::storage[PRODUCT->id+1<product::storage.size()?PRODUCT->id+1:0];
            initialised = true;
        }
        else if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow,true)){
            product_edit = &product::storage[PRODUCT->id-1>=0?PRODUCT->id-1:product::storage.size()-1];
            initialised = true;
        }
    }
}

bool _is_input_valid(){
    if((enable_validate?is_str_date_valid(validate)==0:true)&&!product_name.empty()&&!price.empty())
        return true;
    return false;
}

void _reset_input_values(){
    validate.clear();
    product_name.clear();
    quantity = 1;
}

int _text_input_callback(ImGuiInputTextCallbackData* data){
    bool erase = ImGui::IsKeyPressed(ImGuiKey_Backspace);
    int &pos = data->CursorPos;
    for(int i = 0; i < data->BufTextLen; i++){
        if(i==2||(*SESSION_format_as_MMYYYY?false:i==5)){
            data->InsertChars(i,"/");
            continue;
        }
        if(data->Buf[i]=='/'){
            data->DeleteChars(i,1);
        }
    } 
    if(data->BufTextLen>(*SESSION_format_as_MMYYYY?7:10)) data->DeleteChars(data->CursorPos-1,1);
    return 0;
}

int _price_input_callback(ImGuiInputTextCallbackData* data){
    int comma_occurence=0;
    int comma_pos = 0; 
    for(int i = 0; i < data->BufTextLen; i++)
    {
        if(data->Buf[i]==','){
            comma_occurence++;
            comma_pos=i;
        }
        if((comma_occurence==1&&data->CursorPos>=comma_pos+2&&data->BufTextLen>=comma_pos+4))
            data->DeleteChars(data->CursorPos-1,1);
    }
    return 0;
}