#pragma once

namespace Engine
{
class Window
{
public:
        Window();
        ~Window();

        void Fullscreen();
private:
        static void Display();

        int handle = 0;
};

}