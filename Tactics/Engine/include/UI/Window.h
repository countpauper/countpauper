#pragma once
#include "UI/Scene.h"
#include "UI/Bus.h"
#include "UI/HUD.h"

#include <map>

namespace Engine
{

class Window : public Passenger
{
public:
    Window();
    ~Window();
    void SetTitle(const char* title);
    void Fullscreen();
    Scene& GetScene();
    HUD& GetHUD();
    static Window* CurrentWindow();
    Vector PixelScale() const;
private:
    void Init();
    void OnMouse(int button, int state, int x, int y);
    void OnKey(std::uint8_t code, unsigned char ascii,  int x, int y);
    void Render();
    void OnMessage(const Message& message);
    Coordinate Screen2View(int x, int y) const;

    static void Mouse(int button, int state, int x, int y);
    static void SpecialKey(int key,  int x, int y);
    static void Key(unsigned char key,  int x, int y);
    static void Display();

    // TODO: eventually support subwindows with each a scene
    Scene scene;
    HUD hud;
    int handle = 0;
    static std::map<int, Window*> allWindows;
};

}
