#pragma once
#include <keyboardmanager/dll/InputInterface.h>
#include <vector>
#include <functional>
#include <interface/lowlevel_keyboard_event_data.h>

// Class for mocked keyboard input
class MockedInput :
    public InputInterface
{
private:
    // Stores the states for all the keys - false for key up, and true for key down
    std::vector<bool> keyboardState;

    // Function to be executed as a low level hook. By default it is nullptr so the hook is skipped
    std::function<intptr_t(LowlevelKeyboardEvent*)> hookProc;

public:
    MockedInput()
    {
        keyboardState.resize(256, false);
    }

    // Set the keyboard hook procedure to be tested
    void SetHookProc(std::function<intptr_t(LowlevelKeyboardEvent*)> hookProcedure);

    // Function to simulate keyboard input
    UINT SendVirtualInput(UINT cInputs, LPINPUT pInputs, int cbSize);

    // Function to simulate keyboard hook behavior
    intptr_t MockedKeyboardHook(LowlevelKeyboardEvent* data);

    // Function to get the state of a particular key
    bool GetVirtualKeyState(DWORD key);

    // Function to reset the mocked keyboard state
    void ResetKeyboardState();
};
