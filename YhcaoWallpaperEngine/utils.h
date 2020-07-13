#ifndef UTILS_H
#define UTILS_H
#include <Windows.h>

namespace Utils {
void InitProgman();
HWND GetProgman();
HWND GetWorkerWShell();
HWND GetWorkerW();
void SetParentProgman(HWND hWnd);
void SetParentWorkerW(HWND hWnd);
void SetParentWorkerWShell(HWND hWnd);
}
#endif // UTILS_H
