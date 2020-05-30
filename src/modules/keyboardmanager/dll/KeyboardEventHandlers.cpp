#include "pch.h"
#include "KeyboardEventHandlers.h"

namespace KeyboardEventHandlers
{
    // Function to a handle a single key remap
    intptr_t HandleSingleKeyRemapEvent(LowlevelKeyboardEvent* data, KeyboardManagerState& keyboardManagerState) noexcept
    {
        // Check if the key event was generated by KeyboardManager to avoid remapping events generated by us.
        if (!(data->lParam->dwExtraInfo & CommonSharedConstants::KEYBOARDMANAGER_INJECTED_FLAG))
        {
            // The mutex should be unlocked before SendInput is called to avoid re-entry into the same mutex. More details can be found at https://github.com/microsoft/PowerToys/pull/1789#issuecomment-607555837
            std::unique_lock<std::mutex> lock(keyboardManagerState.singleKeyReMap_mutex);
            auto it = keyboardManagerState.singleKeyReMap.find(data->lParam->vkCode);
            if (it != keyboardManagerState.singleKeyReMap.end())
            {
                // If mapped to 0x0 then the key is disabled
                if (it->second == 0x0)
                {
                    return 1;
                }

                int key_count = 1;
                LPINPUT keyEventList = new INPUT[size_t(key_count)]();
                memset(keyEventList, 0, sizeof(keyEventList));

                // Handle remaps to VK_WIN_BOTH
                DWORD target = it->second;
                // If a key is remapped to VK_WIN_BOTH, we send VK_LWIN instead
                if (target == CommonSharedConstants::VK_WIN_BOTH)
                {
                    target = VK_LWIN;
                }

                if (data->wParam == WM_KEYUP || data->wParam == WM_SYSKEYUP)
                {
                    KeyboardManagerHelper::SetKeyEvent(keyEventList, 0, INPUT_KEYBOARD, (WORD)target, KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SINGLEKEY_FLAG);
                }
                else
                {
                    KeyboardManagerHelper::SetKeyEvent(keyEventList, 0, INPUT_KEYBOARD, (WORD)target, 0, KeyboardManagerConstants::KEYBOARDMANAGER_SINGLEKEY_FLAG);
                }

                lock.unlock();
                UINT res = SendInput(key_count, keyEventList, sizeof(INPUT));
                delete[] keyEventList;
                return 1;
            }
        }

        return 0;
    }

    // Function to a change a key's behavior from toggle to modifier
    intptr_t HandleSingleKeyToggleToModEvent(LowlevelKeyboardEvent* data, KeyboardManagerState& keyboardManagerState) noexcept
    {
        // Check if the key event was generated by KeyboardManager to avoid remapping events generated by us.
        if (!(data->lParam->dwExtraInfo & CommonSharedConstants::KEYBOARDMANAGER_INJECTED_FLAG))
        {
            // The mutex should be unlocked before SendInput is called to avoid re-entry into the same mutex. More details can be found at https://github.com/microsoft/PowerToys/pull/1789#issuecomment-607555837
            std::unique_lock<std::mutex> lock(keyboardManagerState.singleKeyToggleToMod_mutex);
            auto it = keyboardManagerState.singleKeyToggleToMod.find(data->lParam->vkCode);
            if (it != keyboardManagerState.singleKeyToggleToMod.end())
            {
                // To avoid long presses (which leads to continuous keydown messages) from toggling the key on and off
                if (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN)
                {
                    if (it->second == false)
                    {
                        keyboardManagerState.singleKeyToggleToMod[data->lParam->vkCode] = true;
                    }
                    else
                    {
                        lock.unlock();
                        return 1;
                    }
                }
                int key_count = 2;
                LPINPUT keyEventList = new INPUT[size_t(key_count)]();
                memset(keyEventList, 0, sizeof(keyEventList));
                KeyboardManagerHelper::SetKeyEvent(keyEventList, 0, INPUT_KEYBOARD, (WORD)data->lParam->vkCode, 0, KeyboardManagerConstants::KEYBOARDMANAGER_SINGLEKEY_FLAG);
                KeyboardManagerHelper::SetKeyEvent(keyEventList, 1, INPUT_KEYBOARD, (WORD)data->lParam->vkCode, KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SINGLEKEY_FLAG);

                lock.unlock();
                UINT res = SendInput(key_count, keyEventList, sizeof(INPUT));
                delete[] keyEventList;

                // Reset the long press flag when the key has been lifted.
                if (data->wParam == WM_KEYUP || data->wParam == WM_SYSKEYUP)
                {
                    lock.lock();
                    keyboardManagerState.singleKeyToggleToMod[data->lParam->vkCode] = false;
                    lock.unlock();
                }

                return 1;
            }
        }

        return 0;
    }

    // Function to a handle a shortcut remap
    intptr_t HandleShortcutRemapEvent(LowlevelKeyboardEvent* data, std::map<Shortcut, RemapShortcut>& reMap, std::mutex& map_mutex) noexcept
    {
        // The mutex should be unlocked before SendInput is called to avoid re-entry into the same mutex. More details can be found at https://github.com/microsoft/PowerToys/pull/1789#issuecomment-607555837
        std::unique_lock<std::mutex> lock(map_mutex);

        // Check if any shortcut is currently in the invoked state
        bool isShortcutInvoked = false;
        for (auto& it : reMap)
        {
            if (it.second.isShortcutInvoked)
            {
                isShortcutInvoked = true;
                break;
            }
        }

        // Iterate through the shortcut remaps and apply whichever has been pressed
        for (auto& it : reMap)
        {
            // If a shortcut is currently in the invoked state then skip till the shortcut that is currently invoked
            if (isShortcutInvoked && !it.second.isShortcutInvoked)
            {
                continue;
            }

            const size_t src_size = it.first.Size();
            const size_t dest_size = it.second.targetShortcut.Size();

            // If the shortcut has been pressed down
            if (!it.second.isShortcutInvoked && it.first.CheckModifiersKeyboardState())
            {
                if (data->lParam->vkCode == it.first.GetActionKey() && (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN))
                {
                    // Check if any other keys have been pressed apart from the shortcut. If true, then check for the next shortcut
                    if (!it.first.IsKeyboardStateClearExceptShortcut())
                    {
                        continue;
                    }

                    size_t key_count;
                    LPINPUT keyEventList;

                    // Remember which win key was pressed initially
                    if (GetAsyncKeyState(VK_RWIN) & 0x8000)
                    {
                        it.second.winKeyInvoked = ModifierKey::Right;
                    }
                    else if (GetAsyncKeyState(VK_LWIN) & 0x8000)
                    {
                        it.second.winKeyInvoked = ModifierKey::Left;
                    }

                    // Get the common keys between the two shortcuts
                    int commonKeys = it.first.GetCommonModifiersCount(it.second.targetShortcut);

                    // If the original shortcut modifiers are a subset of the new shortcut
                    if (commonKeys == src_size - 1)
                    {
                        // key down for all new shortcut keys except the common modifiers
                        key_count = dest_size - commonKeys;
                        keyEventList = new INPUT[key_count]();
                        memset(keyEventList, 0, sizeof(keyEventList));
                        int i = 0;

                        if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.second.targetShortcut.GetCtrlKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetCtrlKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.second.targetShortcut.GetAltKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetAltKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.second.targetShortcut.GetShiftKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetShiftKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    else
                    {
                        // Dummy key, key up for all the original shortcut modifier keys and key down for all the new shortcut keys but common keys in each are not repeated
                        key_count = 1 + (src_size - 1) + (dest_size) - (2 * (size_t)commonKeys);
                        keyEventList = new INPUT[key_count]();
                        memset(keyEventList, 0, sizeof(keyEventList));

                        // Send dummy key
                        int i = 0;
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)KeyboardManagerConstants::DUMMY_KEY, KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                        // Release original shortcut state (release in reverse order of shortcut to be accurate)
                        if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.first.GetShiftKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetShiftKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.first.GetAltKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetAltKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.first.GetCtrlKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetCtrlKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.first.GetWinKey(it.second.winKeyInvoked) != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetWinKey(it.second.winKeyInvoked), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }

                        // Set new shortcut key down state
                        if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.second.targetShortcut.GetCtrlKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetCtrlKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.second.targetShortcut.GetAltKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetAltKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.second.targetShortcut.GetShiftKey() != NULL)
                        {
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetShiftKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }

                    it.second.isShortcutInvoked = true;
                    lock.unlock();
                    UINT res = SendInput((UINT)key_count, keyEventList, sizeof(INPUT));
                    delete[] keyEventList;
                    return 1;
                }
            }
            // The shortcut has already been pressed down at least once, i.e. the shortcut has been invoked
            // There are 4 cases to be handled if the shortcut has been pressed down
            // 1. The user lets go of one of the modifier keys - reset the keyboard back to the state of the keys actually being pressed down
            // 2. The user keeps the shortcut pressed - the shortcut is repeated (for example you could hold down Ctrl+V and it will keep pasting)
            // 3. The user lets go of the action key - keep modifiers of the new shortcut until some other key event which doesn't apply to the original shortcut
            // 4. The user presses a modifier key in the original shortcut - suppress that key event since the original shortcut is already held down physically (This case can occur only if a user has a duplicated modifier key (possibly by remapping) or if user presses both L/R versions of a modifier remapped with "Both")
            // 5. The user presses any key apart from the action key or a modifier key in the original shortcut - revert the keyboard state to just the original modifiers being held down along with the current key press
            // 6. The user releases any key apart from original modifier or original action key - This can't happen since the key down would have to happen first, which is handled above
            else if (it.second.isShortcutInvoked)
            {
                // Get the common keys between the two shortcuts
                int commonKeys = it.first.GetCommonModifiersCount(it.second.targetShortcut);

                // Case 1: If any of the modifier keys of the original shortcut are released before the normal key
                if ((it.first.CheckWinKey(data->lParam->vkCode) || it.first.CheckCtrlKey(data->lParam->vkCode) || it.first.CheckAltKey(data->lParam->vkCode) || it.first.CheckShiftKey(data->lParam->vkCode)) && (data->wParam == WM_KEYUP || data->wParam == WM_SYSKEYUP))
                {
                    // Release new shortcut, and set original shortcut keys except the one released
                    size_t key_count;
                    // if the released key is present in both shortcuts' modifiers (i.e part of the common modifiers)
                    if (it.second.targetShortcut.CheckWinKey(data->lParam->vkCode) || it.second.targetShortcut.CheckCtrlKey(data->lParam->vkCode) || it.second.targetShortcut.CheckAltKey(data->lParam->vkCode) || it.second.targetShortcut.CheckShiftKey(data->lParam->vkCode))
                    {
                        // release all new shortcut keys and the common released modifier except the other common modifiers, and add all original shortcut modifiers except the common ones
                        key_count = (dest_size - commonKeys) + (src_size - 1 - commonKeys);
                    }
                    else
                    {
                        // release all new shortcut keys except the common modifiers and add all original shortcut modifiers except the common ones
                        key_count = (dest_size - 1) + (src_size - 2) - (2 * (size_t)commonKeys);
                    }

                    // If the target shortcut's action key is pressed, then it should be released
                    bool isActionKeyPressed = false;
                    if (GetAsyncKeyState(it.second.targetShortcut.GetActionKey()) & 0x8000)
                    {
                        isActionKeyPressed = true;
                        key_count += 1;
                    }

                    LPINPUT keyEventList = new INPUT[key_count]();
                    memset(keyEventList, 0, sizeof(keyEventList));

                    // Release new shortcut state (release in reverse order of shortcut to be accurate)
                    int i = 0;
                    if (isActionKeyPressed)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if (((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) || (it.second.targetShortcut.CheckShiftKey(data->lParam->vkCode))) && it.second.targetShortcut.GetShiftKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetShiftKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if (((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) || (it.second.targetShortcut.CheckAltKey(data->lParam->vkCode))) && it.second.targetShortcut.GetAltKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetAltKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if (((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) || (it.second.targetShortcut.CheckCtrlKey(data->lParam->vkCode))) && it.second.targetShortcut.GetCtrlKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetCtrlKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if (((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) || (it.second.targetShortcut.CheckWinKey(data->lParam->vkCode))) && it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }

                    // Set original shortcut key down state except the action key and the released modifier since the original action key may or may not be held down. If it is held down it will generate it's own key message
                    if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && (!it.first.CheckWinKey(data->lParam->vkCode)) && it.first.GetWinKey(it.second.winKeyInvoked) != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetWinKey(it.second.winKeyInvoked), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && (!it.first.CheckCtrlKey(data->lParam->vkCode)) && it.first.GetCtrlKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetCtrlKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && (!it.first.CheckAltKey(data->lParam->vkCode)) && it.first.GetAltKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetAltKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }
                    if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && (!it.first.CheckShiftKey(data->lParam->vkCode)) && it.first.GetShiftKey() != NULL)
                    {
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetShiftKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                        i++;
                    }

                    it.second.isShortcutInvoked = false;
                    it.second.winKeyInvoked = ModifierKey::Disabled;
                    lock.unlock();
                    if (key_count > 0)
                    {
                        UINT res = SendInput((UINT)key_count, keyEventList, sizeof(INPUT));
                        delete[] keyEventList;
                    }
                    return 1;
                }

                // The system will see the modifiers of the new shortcut as being held down because of the shortcut remap
                if (it.second.targetShortcut.CheckModifiersKeyboardState())
                {
                    // Case 2: If the original shortcut is still held down the keyboard will get a key down message of the action key in the original shortcut and the new shortcut's modifiers will be held down (keys held down send repeated keydown messages)
                    if (data->lParam->vkCode == it.first.GetActionKey() && (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN))
                    {
                        size_t key_count = 1;
                        LPINPUT keyEventList = new INPUT[key_count]();
                        memset(keyEventList, 0, sizeof(keyEventList));
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, 0, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);

                        it.second.isShortcutInvoked = true;
                        lock.unlock();
                        UINT res = SendInput((UINT)key_count, keyEventList, sizeof(INPUT));
                        delete[] keyEventList;
                        return 1;
                    }

                    // Case 3: If the action key is released from the original shortcut keep modifiers of the new shortcut until some other key event which doesn't apply to the original shortcut
                    if (data->lParam->vkCode == it.first.GetActionKey() && (data->wParam == WM_KEYUP || data->wParam == WM_SYSKEYUP))
                    {
                        size_t key_count = 1;
                        LPINPUT keyEventList = new INPUT[key_count]();
                        memset(keyEventList, 0, sizeof(keyEventList));
                        KeyboardManagerHelper::SetKeyEvent(keyEventList, 0, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);

                        it.second.isShortcutInvoked = true;
                        lock.unlock();
                        UINT res = SendInput((UINT)key_count, keyEventList, sizeof(INPUT));
                        delete[] keyEventList;
                        return 1;
                    }

                    // Case 4: If a modifier key in the original shortcut is pressed then suppress that key event since the original shortcut is already held down physically - This case can occur only if a user has a duplicated modifier key (possibly by remapping) or if user presses both L/R versions of a modifier remapped with "Both"
                    if ((it.first.CheckWinKey(data->lParam->vkCode) || it.first.CheckCtrlKey(data->lParam->vkCode) || it.first.CheckAltKey(data->lParam->vkCode) || it.first.CheckShiftKey(data->lParam->vkCode)) && (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN))
                    {
                        it.second.isShortcutInvoked = true;
                        return 1;
                    }

                    // Case 5: If any key apart from the action key or a modifier key in the original shortcut is pressed then revert the keyboard state to just the original modifiers being held down along with the current key press
                    if (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN)
                    {
                        size_t key_count;
                        LPINPUT keyEventList;

                        // If the original shortcut is a subset of the new shortcut
                        if (commonKeys == src_size - 1)
                        {
                            key_count = dest_size - commonKeys + 2;
                            keyEventList = new INPUT[key_count]();
                            memset(keyEventList, 0, sizeof(keyEventList));

                            int i = 0;
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                            if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.second.targetShortcut.GetShiftKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetShiftKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.second.targetShortcut.GetAltKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetAltKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.second.targetShortcut.GetCtrlKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetCtrlKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }

                            // Send current key pressed
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)data->lParam->vkCode, 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;

                            // Send dummy key since the current key pressed could be a modifier
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)KeyboardManagerConstants::DUMMY_KEY, KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }
                        else
                        {
                            // Key up for all new shortcut keys, key down for original shortcut modifiers, dummy key and current key press but common keys aren't repeated
                            key_count = (dest_size) + (src_size - 1) + 2 - (2 * (size_t)commonKeys);
                            keyEventList = new INPUT[key_count]();
                            memset(keyEventList, 0, sizeof(keyEventList));

                            // Release new shortcut state (release in reverse order of shortcut to be accurate)
                            int i = 0;
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetActionKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                            if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.second.targetShortcut.GetShiftKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetShiftKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.second.targetShortcut.GetAltKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetAltKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.second.targetShortcut.GetCtrlKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetCtrlKey(), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked), KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }

                            // Set old shortcut key down state

                            if ((it.second.targetShortcut.GetWinKey(it.second.winKeyInvoked) != it.first.GetWinKey(it.second.winKeyInvoked)) && it.first.GetWinKey(it.second.winKeyInvoked) != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetWinKey(it.second.winKeyInvoked), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetCtrlKey() != it.first.GetCtrlKey()) && it.first.GetCtrlKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetCtrlKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetAltKey() != it.first.GetAltKey()) && it.first.GetAltKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetAltKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }
                            if ((it.second.targetShortcut.GetShiftKey() != it.first.GetShiftKey()) && it.first.GetShiftKey() != NULL)
                            {
                                KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)it.first.GetShiftKey(), 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                                i++;
                            }

                            // Send current key pressed
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)data->lParam->vkCode, 0, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;

                            // Send dummy key
                            KeyboardManagerHelper::SetKeyEvent(keyEventList, i, INPUT_KEYBOARD, (WORD)KeyboardManagerConstants::DUMMY_KEY, KEYEVENTF_KEYUP, KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG);
                            i++;
                        }

                        it.second.isShortcutInvoked = false;
                        it.second.winKeyInvoked = ModifierKey::Disabled;
                        lock.unlock();
                        UINT res = SendInput((UINT)key_count, keyEventList, sizeof(INPUT));
                        delete[] keyEventList;
                        return 1;
                    }
                    // Case 6: If any key apart from original modifier or original action key is released - This can't happen since the key down would have to happen first, which is handled above
                }

                // Code added for safety: Should not generally occur unless some weird keyboard interaction occurs
                // If it was in isShortcutInvoked state and none of the above cases occur, then reset the flags
                it.second.isShortcutInvoked = false;
                it.second.winKeyInvoked = ModifierKey::Disabled;
            }
        }

        return 0;
    }

    // Function to a handle an os-level shortcut remap
    intptr_t HandleOSLevelShortcutRemapEvent(LowlevelKeyboardEvent* data, KeyboardManagerState& keyboardManagerState) noexcept
    {
        // Check if the key event was generated by KeyboardManager to avoid remapping events generated by us.
        if (data->lParam->dwExtraInfo != KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG)
        {
            bool result = HandleShortcutRemapEvent(data, keyboardManagerState.osLevelShortcutReMap, keyboardManagerState.osLevelShortcutReMap_mutex);
            return result;
        }

        return 0;
    }

    // Function to a handle an app-specific shortcut remap
    intptr_t HandleAppSpecificShortcutRemapEvent(LowlevelKeyboardEvent* data, KeyboardManagerState& keyboardManagerState) noexcept
    {
        // Check if the key event was generated by KeyboardManager to avoid remapping events generated by us.
        if (data->lParam->dwExtraInfo != KeyboardManagerConstants::KEYBOARDMANAGER_SHORTCUT_FLAG)
        {
            std::wstring process_name = KeyboardManagerHelper::GetCurrentApplication(false);
            if (process_name.empty())
            {
                return 0;
            }

            std::unique_lock<std::mutex> lock(keyboardManagerState.appSpecificShortcutReMap_mutex);
            auto it = keyboardManagerState.appSpecificShortcutReMap.find(process_name);
            if (it != keyboardManagerState.appSpecificShortcutReMap.end())
            {
                lock.unlock();
                bool result = HandleShortcutRemapEvent(data, keyboardManagerState.appSpecificShortcutReMap[process_name], keyboardManagerState.appSpecificShortcutReMap_mutex);
                return result;
            }
        }

        return 0;
    }
}
