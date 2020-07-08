#include "pch.h"
#include "TestHelpers.h"
#include "MockedInput.h"
#include "keyboardmanager/common/KeyboardManagerState.h"

namespace TestHelpers
{
    // Function to reset the environment variables for tests
    void ResetTestEnv(MockedInput& input, KeyboardManagerState& state)
    {
        input.ResetKeyboardState();
        input.SetHookProc(nullptr);
        input.SetSendVirtualInputTestHandler(nullptr);
        input.SetForegroundProcess(L"");
        state.ClearSingleKeyRemaps();
        state.ClearOSLevelShortcuts();
    }
}
