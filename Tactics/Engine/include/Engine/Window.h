#pragma once
#include "Engine/Scene.h"
#include <map>

namespace Engine
{
class Window
{
public:
        Window();
        ~Window();
        void SetTitle(const char* title);
        void Fullscreen();
        Scene& GetScene();
private:

        static void Mouse(int button, int state, int x, int y);
        static void SpecialKey(int key,  int x, int y);
        static void Key(unsigned char key,  int x, int y);
        static void Display();
        static Window* CurrentWindow();

        // TODO: eventually support subwindows with each a scene
        Scene scene;
        int handle = 0;
        static std::map<int, Window*> allWindows;
};

}
