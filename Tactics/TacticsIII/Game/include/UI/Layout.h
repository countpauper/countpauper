#pragma once 

#include "UI/Label.h"
#include "UI/Splitter.h"
#include "UI/Bar.h"
#include <array>
#include <vector>
#include <functional>

template<typename CT, size_t N>
std::vector<std::reference_wrapper<Engine::Control>> make_control_refs(std::array<CT, N>& arr) 
{
    std::vector<std::reference_wrapper<Engine::Control>> refs;
    refs.reserve(N);
    for(auto& item : arr) {
        refs.emplace_back(std::ref(static_cast<Engine::Control&>(item)));
    }
    return refs;
}

struct Layout 
{
    Engine::Label info_lbl;
    Engine::Label plan_lbl;
    Engine::Splitter info_plan_v_splitter;
    Engine::Label log_lbl;
    std::array<Engine::Label, 10> buttons;
    Engine::Bar button_bar;
    Engine::Splitter log_button_splitter;
    Engine::Splitter hud_bottom_splitter;

    Layout()
        : info_lbl("left_lbl", "Select an avatar", 20)
        , plan_lbl("right_lbl", "Make a plan", 20)
        , info_plan_v_splitter(info_lbl, plan_lbl)
        , log_lbl("log_lbl", "", 20)
        , buttons({
            Engine::Label("button0", "1"), 
            Engine::Label("button1", "2"), 
            Engine::Label("button2", "3"), 
            Engine::Label("button3", "4"), 
            Engine::Label("button4", "5"), 
            Engine::Label("button5", "6"), 
            Engine::Label("button6", "7"), 
            Engine::Label("button7", "8"), 
            Engine::Label("button8", "9"), 
            Engine::Label("button9", "10")})
        , button_bar(make_control_refs(buttons))
        , log_button_splitter(log_lbl, button_bar, false, 0.75)
        , hud_bottom_splitter(info_plan_v_splitter, log_button_splitter, false, 0.75)
    {
        log_lbl.vertical_align = Engine::Align::bottom;
        for(auto& button : buttons)
        {
            button.vertical_align = Engine::Align::center;
            button.outline = Engine::RGBA(128, 128, 128);
        }
    }
    
    Engine::Control& root() 
    {
        return hud_bottom_splitter;
    }
};
