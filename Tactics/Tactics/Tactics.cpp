// Tactics.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tactics.h"
#include <GL/gl.h>            /* OpenGL header file */
#include <GL/glu.h>            /* OpenGL utilities header file */
#include <math.h>
#include <fstream>
#include "game.h"

#define M_PI 3.14159265358979323846
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                    // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HGLRC hGLRC;
std::unique_ptr<Game::Game> game;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
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

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_TACTICS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TACTICS));

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
    game = std::make_unique<Game::Game>();
    std::wifstream fs("Game");
    if (fs.fail())
    {
        MessageBox(NULL, L"Loading Game failed:  ", L"Error", MB_OK);
        return FALSE;
    }
    fs >> *game.get();
    if (fs.fail())
    {
        MessageBox(NULL, L"Reading Game failed:  ", L"Error", MB_OK);
        return FALSE;
    }

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
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
 
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


void Render()
{
    float angleOfView = 90; // makes scale = 1
    float scale = static_cast<float>(1.0 / tan(angleOfView * 0.5 * M_PI / 180.0));
    float n = 10;
    float f = 0;
    float z = 1;
    GLfloat perspectiveMatrix[16] = 
    {
        scale, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, f / (f - n), 1,
        0, 0, f*n /(f-n), 0
    };
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(perspectiveMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* rotate a triangle around */
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0,  1, z);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1, -1, z);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1, -1, z);
    glEnd();
    glFlush();
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
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        Render();
        SwapBuffers(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        PostMessage(hWnd, WM_PAINT, 0, 0);
    return 0;
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
