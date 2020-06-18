#include "pch.h"
#include "ShiftKeyHook.h"

#pragma region public

HHOOK ShiftKeyHook::hHook = {};
std::function<void(bool)> ShiftKeyHook::callback = {};

ShiftKeyHook::ShiftKeyHook(std::function<void(bool)> extCallback)
{
    callback = std::move(extCallback);
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, ShiftKeyHookProc, GetModuleHandle(NULL), 0);
}

void ShiftKeyHook::enable()
{
    if (!hHook)
    {
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, ShiftKeyHookProc, GetModuleHandle(NULL), 0);
    }
}

void ShiftKeyHook::disable()
{
    if (hHook)
    {
        UnhookWindowsHookEx(hHook);
        hHook = NULL;
    }
}

#pragma endregion

#pragma region private

LRESULT CALLBACK ShiftKeyHook::ShiftKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN)
        {
            PKBDLLHOOKSTRUCT kbdHookStruct = (PKBDLLHOOKSTRUCT)lParam;
            if (kbdHookStruct->vkCode == VK_LSHIFT || kbdHookStruct->vkCode == VK_RSHIFT)
            {
                callback(true);
            }
        }
        else if (wParam == WM_KEYUP)
        {
            PKBDLLHOOKSTRUCT kbdHookStruct = (PKBDLLHOOKSTRUCT)lParam;
            if (kbdHookStruct->vkCode == VK_LSHIFT || kbdHookStruct->vkCode == VK_RSHIFT)
            {
                callback(false);
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

#pragma endregion
