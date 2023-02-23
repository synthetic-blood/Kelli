#include<imgui.h>
#include"variables.hpp"
#include"../product.hpp"
#include"../currency.hpp"
#include"../lucro.hpp"
#include<implot/implot.h>
extern std::vector<product*> selected;
static bool MERGE_CHANGES(0);
static int affected_items;

void _DRAW_MAIN_SELL_PANEL_(){
    //ImGui::Begin("OPERAR VENDA");
    if(SYSTEM_STATE!=SELL){
        ImGui::Text("TOTAL DO DIA: %s R$", _currency_to_str(lucro::total_of_the_day).c_str());
        ImGui::BeginTable("TABELA HISTORICO",2);
        ImGui::TableSetupColumn("DATA");
        ImGui::TableSetupColumn("LUCRO");
        ImGui::TableHeadersRow();
        for(const auto& l:lucro::historico){
            ImGui::TableNextColumn();
            ImGui::Text("%s",l.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s R$", _currency_to_str(l.second).c_str());
        }
        ImGui::EndTable();
    }
    else{
        static int real = 0, cent = 0;
        if(MERGE_CHANGES){
            if(time(0)%86400==0)
                lucro::total_of_the_day = 0;
            else
                lucro::total_of_the_day += currency(real,cent).value;
            lucro::registrar();
        }
        ImGui::PushFont(SYSTEM_FONT_ITALIC);
        ImGui::Text("VALOR A PAGAR ");
        ImGui::SameLine();
        ImGui::PopFont();

        ImGui::PushFont(SYSTEM_FONT_ITALIC_BIG);
        ImGui::TextColored(ImVec4(0,1,0,1),"%s R$",_currency_to_str(currency(real,cent).value).c_str());
        ImGui::PopFont();
        ImGui::Separator();

        currency total(0);
        
        if(ImGui::BeginChild(ImGuiID(1),ImVec2(0,-60),true)){
            ImGui::BeginTable("##sell",2,ImGuiTableFlags_Resizable);
            ImGui::TableSetupColumn("PRODUTO");
            ImGui::TableSetupColumn("QUANTIDADE");
            ImGui::TableHeadersRow();
            int _ITR_ = 0;
            for(auto &item:selected){
                if(item->_quantity>0){
                    ImGui::TableNextColumn();//-->|PRODUTO|
                    for(int q = 0; q < item->sell_quantity; q++)
                        total.add(_str_digit_to_int_(item->_price));

                    ImGui::Text("%s",item->name.c_str());

                    ImGui::TableNextColumn();//-->|QUANTIDADE|
                    ImGui::TextColored(ImVec4(0,1,0,1),"%d",item->_quantity);
                    ImGui::SameLine(54);
                    ImGui::Text("/",item->_quantity);
                    ImGui::SameLine(81);
                    ImGui::SetNextItemWidth(150);
                    ImGui::InputInt((std::string("##_sell_quantity_")+std::to_string(_ITR_)).c_str(),&item->sell_quantity);
                    if(item->sell_quantity>item->_quantity)
                        item->sell_quantity=item->_quantity;
                    else if(item->sell_quantity<1)
                        item->sell_quantity = 1;
                    _ITR_++;
                    if(MERGE_CHANGES){
                        item->_quantity-=item->sell_quantity;
                        item->sell_quantity = 1;
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        //
        if(MERGE_CHANGES){
            NOTIFY_UPDATE;
            MERGE_CHANGES = 0;
        }
        if(ImGui::Button("FINALIZAR")){
            MERGE_CHANGES = 1;
        }

        real = total.real;
        cent = total.cent;
    }
    //ImGui::End();
}