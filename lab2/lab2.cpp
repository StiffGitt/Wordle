// lab2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab2.h"

#define MAX_LOADSTRING 100
#define SQSIZE 55
#define MARGIN 6
#define COLUMNS 5
#define LINE_LENGTH 6
#define MAX_ROW 10
#define MAX_APPS 4
#define ANIM_FRAMES 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
std::vector<HWND> windows;
std::vector<std::string> words;
std::map<HWND, std::string> appMap;
std::map<HWND, std::map<char, int>> wordMap;
std::map<HWND, int> stateOfGame;
int type;
int currentRow;
std::string wordsRow[MAX_ROW];
HWND mainWnd;
int animSize;
int animCurr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass2(HINSTANCE hInstance);
void DeleteWindows();
void EasyWindow();
void MediumWindow();
void HardWindow();
void MakeWindow(int windowX, int windowY, int windowWidth, int windowHeight);
void PaintSquares(HWND hWnd);
void PaintKeyboard(HWND hWnd);
bool CheckWord(std::string lastWord);
void ClearWordsRow();
void DrawKeyboardSquare(HDC hdc, HBRUSH brush, int x, int y, int size = SQSIZE);
void DrawLetter(HDC hdc, int x, int y, wchar_t letter);
void DrawMultiKeyboardSquare(HDC hdc, int startY, int startX, char cLetter);
void MakeOverlay(HWND hWnd);
void EndLine(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    MyRegisterClass2(hInstance);
    

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(255, 255, 255));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(MYICON1));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterClass2(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc2;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"okno_rozgrywki";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(MYICON1));

    return RegisterClassExW(&wcex);
}

//
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
   hInst = hInstance; // Store instance handle in our global variable

   std::ifstream infile("wordle.txt");
   if (infile)
   {
       std::string line;
       while (std::getline(infile, line))
       {
           words.push_back(line);
       }
   }
   infile.close();

   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   int windowWidth = SQSIZE * 10 + MARGIN * 11;
   int windowHeight = SQSIZE * 3 + MARGIN * 2 + 40;
   int windowX = (screenWidth - windowWidth) / 2;
   int windowY = (screenHeight - windowHeight)*5/6;

   RECT rect = { 0, 0, windowWidth, windowHeight }; 
   AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0); 
   windowWidth = rect.right - rect.left;
   windowHeight = rect.bottom - rect.top;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION |
       WS_SYSMENU | WS_MINIMIZEBOX ,
       windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
   mainWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_SIZEBOX | WS_EX_LAYERED);
   SetLayeredWindowAttributes(hWnd, 0, (255 * 50) / 100, LWA_ALPHA);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   animSize = SQSIZE;
   animCurr = COLUMNS;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    srand(time(NULL));
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EASY:
            {
                type = 0;
                WritePrivateProfileStringA("WORDLE", "DIFFICULTY", "1", "./Wordle.ini");
                ClearWordsRow();
                EasyWindow();
                CheckMenuItem(GetMenu(hWnd), IDM_EASY, MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_MEDIUM, MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_HARD, MF_UNCHECKED);
                break;
            }
            case IDM_MEDIUM:
            {
                type = 1;
                WritePrivateProfileStringA("WORDLE", "DIFFICULTY", "2", "./Wordle.ini");
                ClearWordsRow();
                MediumWindow();
                CheckMenuItem(GetMenu(hWnd), IDM_EASY, MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_MEDIUM, MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_HARD, MF_UNCHECKED);
                break;
            }
            case IDM_HARD:
            {
                type = 2;
                WritePrivateProfileStringA("WORDLE", "DIFFICULTY", "4", "./Wordle.ini");
                ClearWordsRow();
                HardWindow();
                CheckMenuItem(GetMenu(hWnd), IDM_EASY, MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_MEDIUM, MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_HARD, MF_CHECKED);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE: 
    {
        SetWindowText(hWnd, L"WORDLE-KEYBOARD");
        char buf[256];
        GetPrivateProfileStringA("WORDLE", "DIFFICULTY", "0", buf, 256, "./Wordle.ini");
        if (buf[0] == '0')
        {
            char choice[3] = { '1', '2', '4' };
            buf[0] = choice[rand() % 3];
        }   
        switch (buf[0])
        {
        case '1':
            SendMessage(hWnd, WM_COMMAND, IDM_EASY, 0);
            break;
        case '2':
            SendMessage(hWnd, WM_COMMAND, IDM_MEDIUM, 0);
            break;
        case '4':
            SendMessage(hWnd, WM_COMMAND, IDM_HARD, 0);
            break;
        default:
            break;
        }
        break;
    }
    case WM_TIMER:
    {
        if (wParam == 7)
        {
            int add = 6;
            animSize += add;
            for (int i = 0; i < windows.size(); i++)
            {
                InvalidateRect(windows[i], NULL, TRUE);
            }
            if (animSize >= SQSIZE)
            {
                animSize = 0;
                animCurr += 1;
                if (animCurr >= COLUMNS)
                {
                    animSize = SQSIZE;
                    KillTimer(hWnd, 7);
                    for (int i = 0; i < windows.size(); i++) 
                    {
                        EndLine(windows[i]);
                    }
                }
            }
        }
        break;
    }
    case WM_PAINT:
        {

            
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
            PaintKeyboard(hWnd);
            
        }
        break;
    case WM_ERASEBKGND:
        return 1;
    
    case WM_CHAR:
    {
        if (animCurr < COLUMNS)
            break;
        char c = (char)wParam;
        if (isupper(c))
            c = tolower(c);
        if (wParam == VK_RETURN)
        {
            if (wordsRow[currentRow].size() == COLUMNS)
            {
                if (CheckWord(wordsRow[currentRow]))
                {
                    currentRow++;
                    animSize = 0;
                    animCurr = 0;
                    SetTimer(hWnd, 7, 10, NULL);
                }
                else
                {
                    wordsRow[currentRow] = "";
                }
            }
        }
        else if (wParam == VK_BACK)
        {
            if (wordsRow[currentRow].size() > 1)
            {
                wordsRow[currentRow].pop_back();

            }
            else if (wordsRow[currentRow].size() == 1)
            {
                wordsRow[currentRow] = "";
            }
        }
        else if(wordsRow[currentRow].size() < COLUMNS && islower(c))
        {
            wordsRow[currentRow].push_back(c);
        }
        for (int i = 0; i < windows.size(); i++)
        {
            InvalidateRect(windows[i], NULL, true);
        }

        InvalidateRect(hWnd, NULL, true);
        break;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_RESTORED)
        {
            for (int i = 0; i < windows.size(); i++)
            {
                ShowWindow(windows[i], SW_SHOW);
                InvalidateRect(windows[i], NULL, true);
            }
        }
        else if (wParam == SIZE_MINIMIZED)
        {
            for (int i = 0; i < windows.size(); i++)
            {
                ShowWindow(windows[i], SW_HIDE);
                stateOfGame[windows[i]] = 3;
            }
        }
        break;
    }
        
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        InvalidateRect(hWnd, NULL, true);
        SetWindowText(hWnd, L"WORDLE-KEYBOARD");
        break;
    }

    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        if (stateOfGame[hWnd] != 2)
        {
            PaintSquares(hWnd);
        }
        if (stateOfGame[hWnd] == 3)
        {
            stateOfGame[hWnd] = 0;
            EndLine(hWnd);
        }
        
        
        break;
    }
    case WM_NCHITTEST:
        return HTCAPTION;
    break;
    case WM_DESTROY:
        //PostQuitMessage(0);
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

void DeleteWindows()
{
    while (!windows.empty())
    {
        DestroyWindow(windows.back());
        windows.pop_back();
    }
    appMap.clear();
    stateOfGame.clear();
    for (auto& pair : wordMap)
    {
        pair.second.clear();
    }
    wordMap.clear();
    ClearWordsRow();
    currentRow = 0;
}
void EasyWindow()
{
    DeleteWindows();
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int rows = type * 2 + 6;
    int windowWidth = COLUMNS * SQSIZE + (COLUMNS + 1) * MARGIN;
    int windowHeight = rows * SQSIZE + (rows + 1) * MARGIN;
    RECT rect = { 0, 0, windowWidth, windowHeight }; 
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0); 
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
}
void MediumWindow()
{
    DeleteWindows();
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int rows = type * 2 + 6;
    int windowWidth = COLUMNS * SQSIZE + (COLUMNS + 1) * MARGIN;
    int windowHeight = rows * SQSIZE + (rows + 1) * MARGIN;
    RECT rect = { 0, 0, windowWidth, windowHeight }; 
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0); 
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    int windowX = (screenWidth - windowWidth)* 1/ 6;
    int windowY = (screenHeight - windowHeight) / 2;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
    windowX = (screenWidth - windowWidth) * 5 / 6;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
}

void HardWindow()
{
    DeleteWindows();
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int rows = type * 2 + 6;
    int windowWidth = COLUMNS * SQSIZE + (COLUMNS + 1) * MARGIN;
    int windowHeight = rows * SQSIZE + (rows + 1) * MARGIN;
    RECT rect = { 0, 0, windowWidth, windowHeight }; 
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0); 
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    int windowX = (screenWidth - windowWidth) * 1 / 6;
    int windowY = 0;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
    windowX = (screenWidth - windowWidth) * 5 / 6;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
    windowX = (screenWidth - windowWidth) * 1 / 6;
    windowY = (screenHeight - windowHeight);
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
    windowX = (screenWidth - windowWidth) * 5 / 6;
    MakeWindow(windowX, windowY, windowWidth, windowHeight);
}

void MakeWindow(int windowX,int windowY,int windowWidth,int windowHeight)
{
    HWND newWind = CreateWindowW(L"okno_rozgrywki", szTitle, WS_OVERLAPPED | WS_CAPTION |
        WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInst, nullptr);
    SetWindowLongPtrW(newWind, GWL_EXSTYLE, GetWindowLongPtrW(newWind, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
    SetWindowLongPtrW(newWind, GWL_STYLE, GetWindowLongPtrW(newWind, GWL_STYLE) & ~WS_SYSMENU);
    SetWindowLongPtrW(newWind, GWLP_HWNDPARENT, (LONG_PTR)mainWnd);
    windows.push_back(newWind);
    int randomWord = rand() % words.size();
    wordMap[newWind] = std::map<char, int>();
    appMap[newWind] = words[randomWord];
    stateOfGame[newWind] = 0;
    InvalidateRect(newWind, NULL, true);
}
void PaintSquares(HWND hWnd)
{
    int rows = type * 2 + 6;
    RECT rc;
    //HDC hdc = GetDC(hWnd);
    HDC Mhdc = GetDC(hWnd);
    HDC hdc = CreateCompatibleDC(Mhdc);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(164, 174, 196));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    GetClientRect(hWnd, &rc);
    HBITMAP memBitmap = CreateCompatibleBitmap(Mhdc, rc.right - rc.left, rc.bottom - rc.top);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(hdc, memBitmap);
    HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
    HBRUSH brushGray = CreateSolidBrush(RGB(164, 174, 196));
    HBRUSH brushYellow = CreateSolidBrush(RGB(243, 194, 55));
    HBRUSH brushGreen = CreateSolidBrush(RGB(121, 184, 81));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HFONT hFont = CreateFont(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
    SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    HBRUSH backgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT backgroundRect = { 0, 0, width, height };
    FillRect(hdc, &backgroundRect, backgroundBrush);
    DeleteObject(backgroundBrush);

    std::string password = appMap[hWnd];
    std::map<char, int> *map = &wordMap[hWnd];

    for (int j = 0; j < rows; j++)
    {
        for (int i = 0; i < COLUMNS; i++)
        {
            if (wordsRow[j].size() > i && j < currentRow )
            {
                SelectObject(hdc, brushGray);
                if (wordsRow[j][i] == password[i])
                {
                    SelectObject(hdc, brushGreen);
                    (*map)[toupper(wordsRow[j][i])] = 2;
                }
                else
                {
                    for (int k = 0; k < password.size(); k++)
                    {
                        if (wordsRow[j][i] == password[k])
                        {
                            SelectObject(hdc, brushYellow);
                            if ((*map)[toupper(wordsRow[j][i])] != 2)
                                (*map)[toupper(wordsRow[j][i])] = 1;
                            break;
                        }
                        if ((*map)[toupper(wordsRow[j][i])] != 2)
                            (*map)[toupper(wordsRow[j][i])] = 0;
                    }
                }
            }
            int sqOff = (j == currentRow - 1 && i == animCurr)? (SQSIZE - animSize) / 2 : 0;
            if (i > animCurr && j == currentRow - 1)
            {
                SelectObject(hdc, brush);
            }
            RoundRect(hdc, rc.left + MARGIN * (i + 1) + SQSIZE * i,
                rc.top + MARGIN * (j + 1) + SQSIZE * j + sqOff,
                rc.left + MARGIN * (i + 1) + SQSIZE * (i + 1),
                rc.top + MARGIN * (j + 1) + SQSIZE * (j + 1) - sqOff,
                SQSIZE / 5, SQSIZE / 5);
            if (wordsRow[j].size() > i)
            {
                RECT textRect;
                textRect.left = rc.left + MARGIN * (i + 1) + SQSIZE * i;
                textRect.top = rc.top + MARGIN * (j + 1) + SQSIZE * j;
                textRect.right = rc.left + MARGIN * (i + 1) + SQSIZE * (i + 1);
                textRect.bottom = rc.top + MARGIN * (j + 1) + SQSIZE * (j + 1);
                std::string tempS = "";
                tempS.push_back(toupper(wordsRow[j].at(i)));
                std::wstring temp = std::wstring(tempS.begin(), tempS.end());
                DrawText(hdc, temp.c_str(), 1, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
            
            SelectObject(hdc, brush);
        }
    }
    DeleteObject(hFont);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    DeleteObject(brushGray);
    DeleteObject(brushYellow);
    DeleteObject(brushGreen);
    if (stateOfGame[hWnd] != 1)
    {
        BitBlt(Mhdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);
    }
    SelectObject(hdc, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(hdc);
    ReleaseDC(hWnd, Mhdc);
    InvalidateRect(mainWnd, NULL, true);
    
}
void PaintKeyboard(HWND hWnd)
{
    RECT rc;
    HDC Mhdc = GetDC(hWnd);
    HDC hdc = CreateCompatibleDC(Mhdc);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(164, 174, 196));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HPEN NoPen = CreatePen(PS_NULL, 1, RGB(164, 174, 196));
    GetClientRect(hWnd, &rc);
    HBITMAP memBitmap = CreateCompatibleBitmap(Mhdc, rc.right - rc.left, rc.bottom - rc.top);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(hdc, memBitmap);
    HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
    HBRUSH brushGray = CreateSolidBrush(RGB(164, 174, 196));
    HBRUSH brushYellow = CreateSolidBrush(RGB(243, 194, 55));
    HBRUSH brushGreen = CreateSolidBrush(RGB(121, 184, 81));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int sqSizeY = SQSIZE;
    int spaceY = MARGIN;
    int sqSizeX = SQSIZE;
    int spaceX = MARGIN;
    HFONT hFont = CreateFont(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
    SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT backgroundRect = { 0, 0, width, height };
    FillRect(hdc, &backgroundRect, backgroundBrush);
    DeleteObject(backgroundBrush);


    wchar_t letters1[] = { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'};
    char cLetters1[] = { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' };
    int startY = rc.left + (width - (sizeof(letters1) / sizeof(wchar_t) * (SQSIZE + MARGIN) - MARGIN)) / 2;
    int startX = rc.top + (height - 3 * SQSIZE - 2* MARGIN) / 2;
    for (int i = 0; i < sizeof(letters1) / sizeof(wchar_t); i++)
    {
        DrawMultiKeyboardSquare(hdc, startY + MARGIN * i + SQSIZE * i, startX, cLetters1[i]);
        DrawLetter(hdc, startY + MARGIN * i + SQSIZE * i, startX, letters1[i]);
    }
    wchar_t letters2[] = {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'};
    char cLetters2[] = { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' };
    startY = rc.left + (width - (sizeof(letters2) / sizeof(wchar_t) * (SQSIZE + MARGIN) - MARGIN)) / 2;
    startX = startX + SQSIZE + MARGIN;
    for (int i = 0; i < sizeof(letters2) / sizeof(wchar_t); i++)
    {
        DrawMultiKeyboardSquare(hdc, startY + MARGIN * i + SQSIZE * i, startX, cLetters2[i]);
        DrawLetter(hdc, startY + MARGIN * i + SQSIZE * i, startX, letters2[i]);
    }
    wchar_t letters3[] = { 'Z', 'X', 'C', 'V', 'B', 'N', 'M'};
    char cLetters3[] = { 'Z', 'X', 'C', 'V', 'B', 'N', 'M' };
    startY = rc.left + (width - (sizeof(letters3) / sizeof(wchar_t) * (SQSIZE + MARGIN) - MARGIN)) / 2;
    startX = startX + SQSIZE + MARGIN;
    for (int i = 0; i < sizeof(letters3) / sizeof(wchar_t); i++)
    {
        DrawMultiKeyboardSquare(hdc, startY + MARGIN * i + SQSIZE * i, startX, cLetters3[i]);
        DrawLetter(hdc, startY + MARGIN * i + SQSIZE * i, startX, letters3[i]);
    }
    DeleteObject(hFont);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    DeleteObject(NoPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    DeleteObject(brushGray);
    DeleteObject(brushYellow);
    DeleteObject(brushGreen);
    BitBlt(Mhdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(hdc);
    DeleteDC(hdc);
    ReleaseDC(hWnd, Mhdc);
}
bool CheckWord(std::string lastWord)
{
    if (std::find(words.begin(), words.end(), lastWord) != words.end())
        return true;
    else
        return false;
}
void ClearWordsRow()
{
    for (int i = 0; i < MAX_ROW; i++)
    {
        wordsRow[i] = "";
    }
}
void DrawKeyboardSquare(HDC hdc, HBRUSH brush, int x, int y, int size)
{
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    RoundRect(hdc, x,
        y,
        x + size,
        y + size,
        size / 5, size / 5);
    SelectObject(hdc, oldBrush);
}
void DrawLetter(HDC hdc, int x, int y, wchar_t letter)
{
    RECT textRect;
    textRect.left = x;
    textRect.top = y;
    textRect.right = x + SQSIZE;
    textRect.bottom = y + SQSIZE;
    DrawText(hdc, &letter, 1, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}
void DrawMultiKeyboardSquare(HDC hdc, int startY, int startX, char cLetter)
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(164, 174, 196));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HPEN NoPen = CreatePen(PS_NULL, 1, RGB(164, 174, 196));
    HBRUSH brush = CreateSolidBrush(RGB(251, 252, 255));
    HBRUSH brushGray = CreateSolidBrush(RGB(164, 174, 196));
    HBRUSH brushYellow = CreateSolidBrush(RGB(243, 194, 55));
    HBRUSH brushGreen = CreateSolidBrush(RGB(121, 184, 81));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    DrawKeyboardSquare(hdc, brush, startY, startX);
    int pp;
    for (int p = 0; p < MAX_APPS; p++)
    {
        int left = startY;
        int top = startX;
        if (windows.size() < 1)
        {
            break;
        }
        pp = p;
        if (windows.size() <= p)
        {
            if (p < 3)
                pp = 0;
            else
                pp = (windows.size() > 1) ? 1 : 0;
        }
        if (p % 2 == 1)
            left = startY + SQSIZE / 2;
        if (p > 1)
            top = startX + SQSIZE / 2;
        if (wordMap[windows[pp]].count(cLetter) > 0)
        {
            SelectObject(hdc, NoPen);
            switch (wordMap[windows[pp]][cLetter])
            {
            case 0:
                DrawKeyboardSquare(hdc, brushGray, left, top, SQSIZE / 2 + 2);
                break;
            case 1:
                DrawKeyboardSquare(hdc, brushYellow, left, top, SQSIZE / 2 + 2);
                break;
            case 2:
                DrawKeyboardSquare(hdc, brushGreen, left, top, SQSIZE / 2 + 2);
                break;
            default:
                break;
            }
        }
        else
        {
            DrawKeyboardSquare(hdc, brush, startY, startX);
        }
        SelectObject(hdc, pen);
    }

    DeleteObject(pen);
    DeleteObject(NoPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    DeleteObject(brushGray);
    DeleteObject(brushYellow);
    DeleteObject(brushGreen);
}
void MakeOverlay(HWND hWnd)
{
    RECT rc;
    HDC hdc = GetDC(hWnd);
    HDC Ohdc = CreateCompatibleDC(hdc);
    GetClientRect(hWnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(Ohdc, hBitmap);
    HBRUSH brush = CreateSolidBrush(RGB((stateOfGame[hWnd] == 1) ? 0 : 255, (stateOfGame[hWnd] == 1) ? 255 : 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(Ohdc, brush);
    RECT backgroundRect = { 0, 0, width, height };
    FillRect(Ohdc, &backgroundRect, brush);
    POINT ptSrc = { 0, 0 };
    POINT ptDst = { rc.left, rc.top };
    SIZE size = { width, height };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 150, 0 };
    AlphaBlend(hdc, ptDst.x, ptDst.y, size.cx, size.cy, Ohdc, ptSrc.x, ptSrc.y, 100, 100, blend);
    if(stateOfGame[hWnd] == 2)
    { 
        HFONT hFont = CreateFont(35, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
        SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));

        std::string word = appMap[hWnd];
        for (auto& c : word)
        {
            c = toupper(c);
        }
        int len = MultiByteToWideChar(CP_UTF8, 0, word.c_str(), -1, NULL, 0);
        wchar_t* text = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, word.c_str(), -1, text, len);

        RECT textRect = { 0, 0, 0, 0 };
        DrawText(hdc, text, -1, &textRect, DT_CALCRECT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        int x = (rc.right - rc.left - textRect.right) / 2;
        int y = (rc.bottom - rc.top - textRect.bottom) / 2;
        DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        delete[] text;
        DeleteObject(hFont);
    }
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(Ohdc);
    ReleaseDC(hWnd, hdc);
}
void EndLine(HWND hWnd)
{
    if (stateOfGame[hWnd] == 0)
    {
        for (int i = 0; i < currentRow; i++)
        {
            if (wordsRow[i] == appMap[hWnd])
            {
                stateOfGame[hWnd] = 1;
                MakeOverlay(hWnd);
            }
        }
    }
    if (currentRow >= type * 2 + 6 && stateOfGame[hWnd] == 0)
    {
        stateOfGame[hWnd] = 2;
        MakeOverlay(hWnd);
    }
}