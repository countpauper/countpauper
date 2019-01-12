// Command lines: 
//  --map=1on1.map --hide

#include "stdafx.h"
#include "Tactics.h"
#include <time.h>
#include <GL/gl.h>            /* OpenGL header file */
#include <GL/glu.h>            /* OpenGL utilities header file */
#include <math.h>
#include <fstream>
#include "Engine/Light.h"
#include "Engine/Camera.h"
#include "Engine/Random.h"
#include "game.h"
#include "SkillBar.h"
#include "TurnList.h"


#define MAX_LOADSTRING 100

// Global Variables:
// TODO: move to window class
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                    // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int windowShow;                                 // WIndow state send to ShowWindow
HGLRC hGLRC;
RECT clientRect;
std::wstring mapName(L"Game.map");
std::unique_ptr<Game::Game> game;
std::unique_ptr<Game::SkillBar> skills;
std::unique_ptr<Game::TurnList> list;
Engine::Light light;
Engine::TopCamera camera;

bool ParseCommandline(const std::wstring& cmdLine)
{
    if (cmdLine.find(L"--hide") != std::wstring::npos)
        windowShow = SW_HIDE;
    static const std::wstring mapOption(L"--map=");
    auto mappos = cmdLine.find(mapOption);
    if (mappos != std::wstring::npos)
    {
        auto mapStart = mappos + mapOption.size();
        auto mapEnd = cmdLine.find(L" ", mapStart);
        if (mapEnd == std::string::npos)
            mapName = cmdLine.substr(mapStart, mapEnd);
        else
            mapName = cmdLine.substr(mapStart, mapEnd - mapStart);
    }
    return true;
}

struct Input
{
    Input() :
        drag(false)
    {
    }
    bool drag;
    int x;
    int y;
} input;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

     // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    windowShow = nCmdShow;

    if (!ParseCommandline(lpCmdLine))
        return 1;
    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_TACTICS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TACTICS));
    OutputDebugStringW((std::wstring(L"Random=") + std::to_wstring(Engine::Random.Seed())).c_str());
    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style            = CS_OWNDC;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance        = hInstance;
    wcex.hIcon            = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TACTICS));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground    =  NULL;
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCE(IDC_TACTICS);
    wcex.lpszClassName    = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}


//   FUNCTION: SetPixelFormat(HWND )
//
// Set the pixel format to be used for OpenGL

BOOL SetPixelFormat(HWND hWnd)
{
    HDC hDC = GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    //pfd.cColorBits   = 32;
    //pfd.cStencilBits = 8;
    //pfd.cDepthBits   = 24;


    int pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
    MessageBox(NULL, L"ChoosePixelFormat() failed:  "
        L"Cannot find a suitable pixel format.", L"Error", MB_OK); 
        return FALSE;
    } 
 
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
        MessageBox(NULL, L"SetPixelFormat() failed:  "
           L"Cannot set format specified.", L"Error", MB_OK);
        return FALSE;
    } 

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    // TODO move and delete on close
    hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);

    ReleaseDC( hWnd, hDC);
    return TRUE;
}    

BOOL Start()
{
    glEnable(GL_TEXTURE_2D);
    game = std::make_unique<Game::Game>();
    {
        std::wifstream fs(mapName);
        if (fs.fail())
        {
            MessageBox(NULL, L"Loading Game failed:  ", L"Error", MB_OK);
            return FALSE;
        }
        fs >> *game;
        if (fs.fail())
        {
            MessageBox(NULL, L"Reading Game failed:  ", L"Error", MB_OK);
            return FALSE;
        }
    }
    skills = std::make_unique<Game::SkillBar>(*game, 64);
    list = std::make_unique<Game::TurnList>(*game, 128);
    game->Start();

    return TRUE;
}

//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   if (!Start())
       return FALSE;
 
   ShowWindow(hWnd, windowShow);
   UpdateWindow(hWnd);
   SetTimer(hWnd, 1,  0,(TIMERPROC) NULL);

   light.Move(Engine::Coordinate(2, 10, 0));
   camera.Move(Engine::Coordinate(2, 5, -5));
   return TRUE;
}

// TODO: move to own file(s), with input state
struct Hit
{
    Hit(unsigned type, unsigned value) :
        type(type),
        value(value)
    {
    }
    unsigned type;
    unsigned value;
    operator bool() const { return type != 0;  }
};


void ZoomToScreenCoord(int x, int y)
{
    // gluPickMatrix
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glTranslatef(float(viewport[2] - 2.0f * float(x - viewport[0])), float(viewport[3] - 2 * ((clientRect.bottom-y) - viewport[1])), 0.0f);
    glScalef(float(viewport[2]), float(viewport[3]), 1.0f);
}

Hit Select(int x, int y)
{
    GLuint buffer[512];
    glSelectBuffer(512, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();

    // TODO: organize panels automatically in a list/tree, click in main "window", split render setup from render (draw)
    int panelHeight = skills->Height();
    if (y <= clientRect.bottom - panelHeight)
    {
        if (x > clientRect.right- int(list->Width()))
        {
            glViewport(clientRect.right- list->Width(), panelHeight, list->Width(), clientRect.bottom- panelHeight);
            glDisable(GL_LIGHTING);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            ZoomToScreenCoord(x, y);
            list->Render();
        }
        else
        {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(clientRect.top, panelHeight, clientRect.right-clientRect.left, clientRect.bottom- panelHeight);
            glEnable(GL_CULL_FACE);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            ZoomToScreenCoord(x, y);

            camera.Render();
            game->Render();
        }
    }
    else
    {
        glViewport(clientRect.left, clientRect.top, clientRect.right-clientRect.left, panelHeight);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ZoomToScreenCoord(x, y);
        skills->Render();
    }

    GLint hits = glRenderMode(GL_RENDER);
    GLuint bestType = 0;
    GLuint bestObject = 0;
    if (hits > 0)
    {
        unsigned index = 0;
        for (int hit = 0; hit < hits; ++hit)
        {
            GLuint names = buffer[index];
            assert(names == 2 && "Each hit should be a type name + an object name");
            GLuint type = buffer[index + 3];
            if (type > bestType)
            {    // Since z-buffer isn't working, use sorted type for preference, 
                bestType = type;
                bestObject = buffer[index + 4];
            }
            index += names + 3;
        }
    }
    return Hit( bestType, bestObject );
}

void Render()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int panelHeight = skills->Height();
    glViewport(clientRect.left, panelHeight, clientRect.right-clientRect.left, clientRect.bottom- panelHeight);

    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_BLEND);    TODO: first render non alpha tiles, then alpha tiles with depth test
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Lighting makes it nice but dark
    //glEnable(GL_LIGHTING);
    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    light.Render();

    // Render game
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (input.drag)
    {
        ZoomToScreenCoord(input.x, input.y);
    }
    camera.Face(game->focus);
    camera.Render();
    game->Render();
   

    // render panel 
    glViewport(clientRect.left, clientRect.top, clientRect.right-clientRect.left, panelHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (input.drag)
    {
        ZoomToScreenCoord(input.x, input.y);
    }
    skills->Render();
    // Render turn list
    glViewport(clientRect.right- list->Width(), panelHeight, list->Width(), clientRect.bottom- panelHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (input.drag)
    {
        ZoomToScreenCoord(input.x, input.y);
    }
    list->Render();

}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND    - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY    - post a quit message and return
//  WM_SIZE        Resize the viewport
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
         SetPixelFormat(hWnd);

         hdc = GetDC(hWnd);
         SelectObject(hdc, GetStockObject(SYSTEM_FONT));
         wglUseFontBitmaps(hdc, 0, 255, 1000);

         ReleaseDC(hWnd, hdc);
        break;
    case WM_TIMER:
        game->Tick();
        Sleep(0);
        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_LBUTTONDOWN:
        input.x = LOWORD(lParam);
        input.y = HIWORD(lParam);
        input.drag = true;
        SetCapture(hWnd); 
        break;
    case WM_LBUTTONUP:
        {
            int x = LOWORD(lParam), y = HIWORD(lParam);
            int dx = x- input.x, dy = y- input.y;
            input.drag = false;
            //camera.FinishDrag(dx/100.0f, dy/-100.0f);
            if ((std::abs(dx) < 1) && (std::abs(dy) < 1))
            {
                auto hit = Select(x, y);
                if (hit)
                    game->Click(Game::Game::Selection(hit.type), hit.value);
            }
            ReleaseCapture();
            InvalidateRect(hWnd, nullptr, TRUE);
    }
    case WM_MOUSEMOVE:
        if (input.drag)
        {
            int dx = LOWORD(lParam) - input.x;
            int dy = HIWORD(lParam) - input.y;
            // camera.Drag(dx / 100.0f, dy / -100.0f);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        break;
    case WM_MOUSEWHEEL:
        {
            int delta = int(wParam)>>16;
            camera.Zoom(float(delta) * 1e-3f);
            InvalidateRect(hWnd, nullptr, TRUE);
    }
        break;
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        Render();
        SwapBuffers(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_SIZE:
        clientRect.right = LOWORD(lParam);
        clientRect.bottom = HIWORD(lParam);
        PostMessage(hWnd, WM_PAINT, 0, 0);
    return 0;
    case WM_KEYDOWN:
        skills->Key(wParam);
        list->Key(wParam);
        game->Key(wParam);
        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        hdc = GetDC(hWnd);
        wglMakeCurrent(hdc, 0);
        wglDeleteContext(hGLRC);
        ReleaseDC(hWnd, hdc);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
