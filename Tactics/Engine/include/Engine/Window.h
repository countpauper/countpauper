#pragma once

namespace Engine
{
class Window
{
public:
        Window();
        ~Window();
        void SetTitle(const char* title);
        void Fullscreen();
private:
        static void Mouse(int button, int state, int x, int y);
        static void Key(unsigned char key,  int x, int y);
        static void Display();

        int handle = 0;
};

}