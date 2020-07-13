#include <utils.h>
HWND _WORKERW = nullptr;
HWND _WORKERWSHELLDLL = nullptr;
HWND _PROGMAN = nullptr;

inline BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
{
    HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
    if (defview != nullptr)
    {
        _WORKERWSHELLDLL = tophandle;
        //SendMessageA(tophandle,WM_PAINT,0,0);
        _WORKERW = FindWindowEx(0, tophandle, L"WorkerW", 0);
    }
    return true;
}


void Utils::InitProgman()
{
    int result;
    _PROGMAN = FindWindow(L"Progman", nullptr);
    //使用 0x3e8 命令分割出两个 WorkerW
    SendMessageTimeout(_PROGMAN, 0x052c, 0 ,0, SMTO_NORMAL, 0x3e8,(PDWORD_PTR)&result);
    //SendMessageA(windowHandle,WM_PAINT,0,0);
    //遍历窗体获得窗口句柄
    EnumWindows(EnumWindowsProc,(LPARAM)nullptr);
    //ShowWindow(_WORKERW,SW_SHOW);
    //SendMessageA(_WORKERW,WM_PAINT,0,0);
    // 显示WorkerW
    //ShowWindow(_WORKERW,SW_HIDE);
}
HWND Utils::GetProgman(){
    return _PROGMAN;
}

HWND Utils::GetWorkerWShell(){
    return _WORKERWSHELLDLL;
}

HWND Utils::GetWorkerW(){
    return _WORKERW;
}

void Utils::SetParentProgman(HWND hWnd)
{
    ShowWindow(_WORKERW,SW_HIDE);
    SetWindowLongA(hWnd, GWL_STYLE,0/*,WS_EX_TRANSPARENT*/);
    SetParent(hWnd, _PROGMAN);
    SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOACTIVATE);
}

void Utils::SetParentWorkerW(HWND hWnd)
{
    ShowWindow(_WORKERW,SW_SHOW);
    SetWindowLongA(hWnd, GWL_STYLE,0/*,WS_EX_TRANSPARENT*/);
    SetParent(hWnd, _WORKERW);
    SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOACTIVATE);
}

void Utils::SetParentWorkerWShell(HWND hWnd)
{
    SetWindowLongA(hWnd, GWL_STYLE,0/*,WS_EX_TRANSPARENT*/);
    SetParent(hWnd, _WORKERWSHELLDLL);
    SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOACTIVATE);
}
