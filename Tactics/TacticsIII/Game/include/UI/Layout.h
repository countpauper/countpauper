#pragma once 

#include "UI/Label.h"
#include "UI/Button.h"
#include "UI/Splitter.h"
#include "UI/Bar.h"
#include <array>
#include <vector>
#include <functional>

template<typename CT>
std::vector<std::reference_wrapper<Engine::Control>> make_control_refs(std::span<CT> controls) 
{
    std::vector<std::reference_wrapper<Engine::Control>> refs;
    refs.reserve(controls.size());
    for(auto& item : controls) {
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
    std::array<Engine::Button, 10> buttons;
    Engine::Bar button_bar;
    Engine::Splitter log_button_splitter;
    Engine::Splitter hud_bottom_splitter;

    Layout()
        : info_lbl("left_lbl", "Select an avatar", 20)
        , plan_lbl("right_lbl", "Make a plan", 20)
        , info_plan_v_splitter(info_lbl, plan_lbl)
        , log_lbl("log_lbl", "", 20)
        , buttons({
            Engine::Button("button0", "1"), 
            Engine::Button("button1", "2"), 
            Engine::Button("button2", "3"), 
            Engine::Button("button3", "4"), 
            Engine::Button("button4", "5"), 
            Engine::Button("button5", "6"), 
            Engine::Button("button6", "7"), 
            Engine::Button("button7", "8"), 
            Engine::Button("button8", "9"), 
            Engine::Button("button9", "10")})
        , button_bar(make_control_refs<Engine::Button>(buttons))
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
