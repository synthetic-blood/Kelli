#pragma once
#include<regex>
#include<imgui.h>
#include"add_panel.hpp"
#include"category_panel.hpp"
#include"settings_panel.hpp"
bool _warning = false;
void begin_warning_style(const bool warning){
    _warning = warning;
    if(warning){
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(1,0.2,0,0.2));
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,0.2,0.1,1));
    }
}
void end_warning_style(){
    if(_warning){
        ImGui::PopStyleColor(2);
        _warning = false;
    }
}

static std::string search;
static std::vector<product> search_result;
std::vector<product*> selected;
void formaly_delete_product(){
    is_editing_product = false;
    erase_product(product_edit->id);
    NOTIFY_UPDATE;
    ImGui::CloseCurrentPopup();
}
bool formaly_delete_bool = false;
void product_dialog(product* edit){
    if(ImGui::BeginPopupModal("EDITAR")){
        _DRAW_MAIN_ADD_PANEL_(edit);
        if(button_confirmation("DELETAR PRODUTO")){
            formaly_delete_product();
        }
        if(ImGui::Button("FECHAR")){
            is_editing_product = false;
            force_sort = true;
            save();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void _DRAW_MAIN_TABLE_(){
    static unsigned int option_view=0;
    selected.clear();
    //SEARCH BAR
    ImGui::InputTextWithHint("##search","PESQUISAR PRODUTO",&search,ImGuiInputTextFlags_CharsUppercase);
    for(const product& meta:product::storage){
        if(meta.name.find(search)!=std::string::npos)
            search_result.push_back(meta);
    }

    ImGui::SameLine();
    if(SYSTEM_STATE==NO_OPERATION)
    {
        if(ImGui::Button("INICIAR VENDA"))
            SYSTEM_STATE = SELL;
    }
    else if(SYSTEM_STATE==SELL)
        if(ImGui::Button("CANCELAR"))
            SYSTEM_STATE = NO_OPERATION;

    ImGui::SameLine();

    //SETTINGS
    if(ImGui::ImageButton(view_texture[3],icon_size())){
        ImGui::OpenPopup("KELLI");
        open_settings = true;
    }
    if(ImGui::BeginPopupModal("KELLI",&open_settings,ImGuiWindowFlags_NoResize)){
        _DRAW_MAIN_SETTINGS_PANEL();
        ImGui::EndPopup();
    }
    //-------------------------
    if(option_view==0){
        if(ImGui::Button("+ADICIONAR PRODUTO")){
            ImGui::OpenPopup("ADICIONAR");
        }
    
        if(ImGui::BeginPopupModal("ADICIONAR",0,ImGuiWindowFlags_NoResize)){
            _DRAW_MAIN_ADD_PANEL_();
            if (ImGui::Button("FECHAR")){
                is_editing_product = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    //ImGui::SameLine();
    else if(option_view==1){
        if(ImGui::Button("+CRIAR TAG")){
            ImGui::OpenPopup("CRIAR TAG");
        }
        if(ImGui::BeginPopupModal("CRIAR TAG")){
            _DRAW_CREATE_CATEGORY_PANEL_();
            if(ImGui::Button("FECHAR"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    ImGui::Separator();
    //VIEW
    int _ITR_ = 0;
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0,0,0,0));   
    ImGui::Text("VER");
    ImGui::SameLine();
    for(auto const view:{"TABELA","CATEGORIA"}){
        ImGui::BeginDisabled(option_view==_ITR_);
        if(ImGui::ImageButton(view_texture[_ITR_],icon_size(12))){
            option_view=_ITR_;
        }
        ImGui::EndDisabled();
        if(option_view==0?true:_ITR_==0)
            ImGui::SameLine();
        _ITR_++;
    }
    ImGui::PopStyleColor(2);
    if(option_view==0){

    //FILTER
    static bool filter_out_of_stock = false;
    static bool filter_expired = false;
    static bool filter_no_category = true;
    
    if(ImGui::TreeNodeEx("FILTRO")){
        ImGui::Checkbox("ESGOTADO",&filter_out_of_stock);
        ImGui::Checkbox("VENCIDO",&filter_expired);
        if(ImGui::TreeNode("TAG")){
            ImGui::BeginDisabled(Category::categories.empty());
            ImGui::Checkbox("N/A",&filter_no_category);
            ImGui::EndDisabled();
            for(auto &c:Category::categories){
                static int ID = -1;
                static bool choose = false;
                static bool select_all = false;
                static int counter = 0; 
                static int lastID = ID;
                if(choose){
                    if(ID!=c.id){
                        c.is_selected = false;
                    }
                    if(ID==c.id){
                        choose = false;
                        lastID = ID;
                        ID = -1;
                    }
                }
                if(select_all){
                    if(counter<Category::categories.size()){
                        counter++;
                        c.is_selected = true;
                    }
                    else{
                        select_all = false;
                        counter = 0;
                        lastID = ID;
                        ID = -1;
                    }
                    
                }
                
                ImGui::Checkbox(c.name.c_str(),&c.is_selected);
                if(ImGui::IsItemHovered()&&ImGui::IsMouseClicked(ImGuiMouseButton_Right)&&ID==-1){
                    if(lastID==c.id)
                        select_all = true;
                    else{
                        ID=c.id;
                        c.is_selected = true;
                        choose = true;
                    }
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    //PRODUCT TABLE
    if(ImGui::BeginChild("TABLE")){
        static std::vector<product> SORTED_STORAGE;
        ImGui::BeginTable("##mytable",5,ImGuiTableFlags_Resizable|ImGuiTableFlags_RowBg|ImGuiTableFlags_BordersH|ImGuiTableFlags_Sortable|ImGuiTableFlags_SortMulti|ImGuiTableFlags_SortTristate,ImVec2(-1,-1));

        ImGui::TableSetupColumn("PRODUTOS");
        ImGui::TableSetupColumn("QUANTIDADE");
        ImGui::TableSetupColumn("PREÇO");
        ImGui::TableSetupColumn("VALIDADE");
        ImGui::TableSetupColumn("TAG");
        
        ImGuiTableSortSpecs *s = ImGui::TableGetSortSpecs();
        static bool last_SortDirection;
        static bool d;
        if((s&&!is_editing_product)||(force_sort&&product::storage.size()>1))
            if(s->SpecsCount>=1){
                d = !(s->Specs->SortDirection-1);
                if(d!=last_SortDirection||force_sort){
                    SORTED_STORAGE = product::storage;
                    sorted = true;
                    int idx = s->Specs->ColumnIndex;
                    std::sort(SORTED_STORAGE.begin(),SORTED_STORAGE.end(),
                    [&](const product &lhs,const product &rhs){
                        
                        if(idx==0)//nome do produto
                            return d?lhs.name > rhs.name:lhs.name < rhs.name;
                        else if(idx==1)//quantidade
                            return d?lhs._quantity > rhs._quantity:lhs._quantity < rhs._quantity;
                        else if(idx==2){
                            currency c1(lhs._price), c2(rhs._price);
                            return d? c1 > c2 : c1 < c2;
                        }
                        else if(idx==3)//validade
                        {
                            tm date1, date2;
                            str_to_tm(lhs._validate,&date1);
                            time_t t1 = mktime(&date1);
                            str_to_tm(rhs._validate,&date2);
                            time_t t2 = mktime(&date2);
                            return d? t1 > t2 : t1 < t2;
                        }
                        else if(idx==4){
                            if(lhs.category_id==-1)
                                return false;
                            else if(rhs.category_id==-1)
                                return true;
                            std::string n1 = Category::categories[lhs.category_id].name;
                            std::string n2 = Category::categories[rhs.category_id].name;
                            return d? n1 > n2 : n1 < n2;
                        }
                        return false;
                    });
                    last_SortDirection = d;
                    if(force_sort)
                        force_sort = false;
                }
            }else{
                if(sorted){
                    sorted = false;
                    if(product::storage.size()>1)
                        std::sort(product::storage.begin(),product::storage.end(),[](const product&lhs,const product&rhs){
                            return lhs.id < rhs.id;
                        });
                    }
            }
        ImGui::TableHeadersRow();
        
        int _ITR_ = 0;

        for(const auto& meta:product::storage){
            if(meta.is_selected){
                selected.push_back(&product::storage[meta.id]);
            }
        }

        //SWAP
        static bool captured_item = false;
        static bool swapped = false;
        static int swapped_id = -1;
        static int captured_id = -1;
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            captured_item = false;
        }

        for(product& meta:!search.empty()?search_result:(sorted?SORTED_STORAGE:product::storage))
        {
            bool OUT_OF_STOCK = meta._quantity<=0;
            meta.check_validate();
            bool INVALID = !meta.is_valid||OUT_OF_STOCK;
            product * ptr = &product::storage[meta.id];
            if(SYSTEM_STATE==SELL&&INVALID)
                continue;
             
            if(filter_expired&&filter_out_of_stock){
                if(meta.is_valid&&!OUT_OF_STOCK)
                    continue;
            }else{
                if(filter_expired&&meta.is_valid)
                    continue;
                if(filter_out_of_stock&&!OUT_OF_STOCK)
                    continue;
            }
            
            if(!Category::categories.empty()&&!Category::categories[meta.category_id].is_selected&&(filter_no_category&&meta.category_id==-1?false:true))
                continue;

            ImGui::TableNextColumn();//-->|PRODUTO|
            begin_warning_style(INVALID);
                ImGui::PushID(_ITR_);
                bool clicked = ImGui::Selectable(meta.name.c_str(),&product::storage[meta.id].is_selected,ImGuiSelectableFlags_SpanAllColumns);
                ImGui::PopID();
                
                if(!sorted&&ImGui::IsMouseDown(ImGuiMouseButton_Left)&&ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
                {
                    if(!captured_item){
                        captured_id = meta.id;
                        captured_item = true;
                    }else if(!swapped&&meta.id!=captured_id){
                        swapped = true;
                        swapped_id = meta.id;
                        if(product::storage[swapped_id].category_id!=-1&&product::storage[captured_id].category_id!=-1){
                            std::swap(*(std::find(Category::categories[product::storage[captured_id].category_id].products.begin(),Category::categories[product::storage[captured_id].category_id].products.end(),&product::storage[captured_id])),
                            *(std::find(Category::categories[product::storage[swapped_id].category_id].products.begin(),Category::categories[product::storage[swapped_id].category_id].products.end(),&product::storage[swapped_id])));
                            std::swap(product::storage[captured_id],product::storage[swapped_id]);
                            std::swap(product::storage[captured_id].id,product::storage[swapped_id].id);
                            std::swap(captured_id,swapped_id);
                        }
                    }else if(swapped&&meta.id!=swapped_id)
                        swapped = false;
                }

                if(clicked){
                    product_edit = &product::storage[meta.id];
                    if(SYSTEM_STATE!=SELL){
                        ImGui::OpenPopup("EDITAR");
                        initialised = true;
                    }
                }
                if(SYSTEM_STATE!=SELL)
                    ptr->is_selected = false;


                if(INVALID){
                    ImGui::SameLine();
                    ImGui::Image(warning3_icon,icon_size());
                    end_warning_style();
                }

            ImGui::TableNextColumn();//-->|QUANTIDADE|
            if(ptr->is_selected){
                ImGui::Text("%d >",meta._quantity);
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0,1,0,1),"%d",meta._quantity-meta.sell_quantity);
            }
            else{
                begin_warning_style(OUT_OF_STOCK);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(0,0));
                ImGui::BeginChildFrame(ImGuiID(_ITR_+1),ImVec2(ImGui::GetColumnWidth(2),ImGui::GetFontSize()),ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoBackground);
                ImGui::Text(OUT_OF_STOCK?"ESGOTADO":"%d",meta._quantity);
                ImGui::EndChildFrame();
                ImGui::PopStyleVar();
                end_warning_style();
            }
            
            if(SYSTEM_STATE!=SELL){
                static bool capture = true;
                static int captured_at = -1;
                static int temp = 0;
                if(temp<0)temp=0;
                if(ImGui::BeginPopupContextItem(std::to_string(_ITR_).append("_popup").c_str())){
                    ImGui::SetNextItemWidth(150);
                    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,1,1,1));
                    ImGui::InputInt("##EDITAR QUANTIDADE",&temp,1,1,ImGuiInputTextFlags_CharsDecimal);
                    ImGui::SameLine();
                    if(ImGui::Button("EDITAR")){
                        ptr->_quantity = temp;
                        NOTIFY_UPDATE;
                    }
                    ImGui::PopStyleColor();
                    ImGui::EndPopup();
                    if(capture){
                        temp = ptr->_quantity;
                        captured_at = _ITR_;
                        capture = false;
                    }
                }else if(captured_at==_ITR_)
                    capture=true;
            }

            ImGui::TableNextColumn();//-->PREÇO
            ImGui::Text("%s",ptr->_price.c_str());
            
            begin_warning_style(!meta.is_valid);
            ImGui::TableNextColumn();//-->|VALIDADE|
            ImGui::PushFont(SYSTEM_FONT_ITALIC);
            ImGui::Text("%s",ptr->_validate.c_str());
            ImGui::PopFont();
            end_warning_style();

            ImGui::TableNextColumn();//-->CATEGORIA
            if(meta.category_id==-1){
                ImGui::Text("N/A");
            }else{
                ImGui::Text("%s",Category::categories[meta.category_id].name.c_str());
                ImGui::SameLine();
                ImGui::ColorButton("SAMPLE",Category::categories[meta.category_id].color,ImGuiColorEditFlags_NoPicker|ImGuiColorEditFlags_NoInputs,icon_size());
            }
            _ITR_++;
        }
        product_dialog(product_edit);
        ImGui::EndTable();
    }
    ImGui::EndChild();
    }//END OF TABLE
    else if(option_view==1){
        if(ImGui::BeginChild("CATEGORY")){
            ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,90);
            bool open_popup = false, product_edit_popup = false;
            bool _BREAK_ = false;
            for(auto c:Category::categories){
                if(!_BREAK_){
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,modImVec4(c.color,0.5f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive,modImVec4(c.color,0.5f));
                }else {
                    break;
                }
                ImGui::PushFont(SYSTEM_FONT_BOLD);//418
                bool openned = ImGui::TreeNode(c.name.c_str());
                if(ImGui::IsItemHovered()&&ImGui::IsMouseDown(ImGuiMouseButton_Right)){
                    category_edit = &Category::categories[c.id];
                    open_popup = true;
                    _BREAK_ = true;
                }
                ImGui::PopFont();//411
                if(openned){
                    int _ITR_ = 1;
                    for(auto const p:c.products){
                        
                        ImGui::PushFont(SYSTEM_FONT_DEFAULT_BIG);
                        ImGui::PushID(_ITR_);
                        if(ImGui::Selectable((p->name+(p->_validate!="*"?" - "+p->_validate:"")).c_str())){
                            product_edit = p;
                            product_edit_popup = true;
                        }
                        ImGui::PopID();
                        ImGui::PopFont();    
                        _ITR_++;
                    }
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor(2);
            }
            if(open_popup){
                initialise_category = true;
                ImGui::OpenPopup("EDITAR TAG");
            }
            if(ImGui::BeginPopupModal("EDITAR TAG")){
                _DRAW_CREATE_CATEGORY_PANEL_(category_edit);
                ImGui::EndPopup();
            }
            
            if(product_edit_popup){
                ImGui::OpenPopup("EDITAR");
            }
            product_dialog(product_edit);
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }
    search_result.clear();
}