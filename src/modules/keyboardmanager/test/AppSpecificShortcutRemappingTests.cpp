#include "pch.h"
#include "CppUnitTest.h"
#include "MockedInput.h"
#include <keyboardmanager/common/KeyboardManagerState.h>
#include <keyboardmanager/dll/KeyboardEventHandlers.h>
#include "TestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace RemappingLogicTests
{
    TEST_CLASS (AppSpecificShortcutRemappingTests)
    {
    private:
        MockedInput mockedInputHandler;
        KeyboardManagerState testState;
        std::wstring testApp1 = L"testtrocess1.exe";
        std::wstring testApp2 = L"testprocess2.exe";

    public:
        TEST_METHOD_INITIALIZE(InitializeTestEnv)
        {
            // Reset test environment
            TestHelpers::ResetTestEnv(mockedInputHandler, testState);

            // Set HandleOSLevelShortcutRemapEvent as the hook procedure
            std::function<intptr_t(LowlevelKeyboardEvent*)> currentHookProc = std::bind(&KeyboardEventHandlers::HandleAppSpecificShortcutRemapEvent, std::ref(mockedInputHandler), std::placeholders::_1, std::ref(testState));
            mockedInputHandler.SetHookProc(currentHookProc);
        }

        // Test if the app specific remap takes place when the target app is in foreground
        TEST_METHOD (AppSpecificShortcut_ShouldGetRemapped_WhenAppIsInForeground)
        {
            // Remap Ctrl+A to Alt+V
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x41);
            Shortcut dest;
            dest.SetKey(VK_MENU);
            dest.SetKey(0x56);
            testState.AddAppSpecificShortcut(testApp1, src, dest);
            
            // Set the testApp as the foreground process
            mockedInputHandler.SetForegroundProcess(testApp1);

            const int nInputs = 2;
            INPUT input[nInputs] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = VK_CONTROL;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0x41;

            // Send Ctrl+A keydown
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));

            // Ctrl and A key states should be unchanged, Alt and V key states should be true
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_CONTROL), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x41), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_MENU), true);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x56), true);
        }

        // Test if the app specific remap takes place when the target app is not in foreground
        TEST_METHOD (AppSpecificShortcut_ShouldNotGetRemapped_WhenAppIsNotInForeground)
        {
            // Remap Ctrl+A to Alt+V
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x41);
            Shortcut dest;
            dest.SetKey(VK_MENU);
            dest.SetKey(0x56);
            testState.AddAppSpecificShortcut(testApp1, src, dest);

            // Set the testApp as the foreground process
            mockedInputHandler.SetForegroundProcess(testApp2);

            const int nInputs = 2;
            INPUT input[nInputs] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = VK_CONTROL;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0x41;

            // Send Ctrl+A keydown
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));

            // Ctrl and A key states should be unchanged, Alt and V key states should be true
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_CONTROL), true);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x41), true);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_MENU), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x56), false);
        }

        // Test if the the keyboard manager state's activated app is correctly set after an app specific remap takes place
        TEST_METHOD (AppSpecificShortcut_ShouldSetCorrectActivatedApp_WhenRemapOccurs)
        {
            // Remap Ctrl+A to Alt+V
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x41);
            Shortcut dest;
            dest.SetKey(VK_MENU);
            dest.SetKey(0x56);
            testState.AddAppSpecificShortcut(testApp1, src, dest);

            // Set the testApp as the foreground process
            mockedInputHandler.SetForegroundProcess(testApp1);

            const int nInputs = 2;
            INPUT input[nInputs] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = VK_CONTROL;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0x41;

            // Send Ctrl+A keydown
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));

            // Activated app should be testApp1
            Assert::AreEqual(testApp1, testState.GetActivatedApp());

            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0x41;
            input[0].ki.dwFlags = KEYEVENTF_KEYUP;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = VK_CONTROL;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;

            // Release A then Ctrl
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));

            // Activated app should be empty string
            Assert::AreEqual(std::wstring(L""), testState.GetActivatedApp());
        }
        // Test if the key states get cleared if foreground app changes after app-specific shortcut is invoked and then released
        TEST_METHOD (AppSpecificShortcut_ShouldClearKeyStates_WhenForegroundAppChangesAfterShortcutIsPressedOnRelease)
        {
            // Remap Ctrl+A to Alt+V
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x41);
            Shortcut dest;
            dest.SetKey(VK_MENU);
            dest.SetKey(0x56);
            testState.AddAppSpecificShortcut(testApp1, src, dest);

            // Set the testApp as the foreground process
            mockedInputHandler.SetForegroundProcess(testApp1);

            const int nInputs = 2;
            INPUT input[nInputs] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = VK_CONTROL;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0x41;

            // Send Ctrl+A keydown
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));

            // Set the testApp as the foreground process
            mockedInputHandler.SetForegroundProcess(testApp2);

            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0x41;
            input[0].ki.dwFlags = KEYEVENTF_KEYUP;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = VK_CONTROL;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;

            // Release A then Ctrl
            mockedInputHandler.SendVirtualInput(nInputs, input, sizeof(INPUT));


            // Ctrl, A, Alt and V should all be false
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_CONTROL), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x41), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(VK_MENU), false);
            Assert::AreEqual(mockedInputHandler.GetVirtualKeyState(0x56), false);
        }
    };
}
