#include "pch.h"
#include "CppUnitTest.h"
#include <keyboardmanager/ui/BufferValidationHelpers.h>
#include "TestHelpers.h"
#include <common/keyboard_layout.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RemappingUITests
{
    // Tests for methods in the BufferValidationHelpers namespace
    TEST_CLASS (BufferValidationTests)
    {
        std::wstring testApp1 = L"testprocess1.exe";
        std::wstring testApp2 = L"testprocess2.exe";
        LayoutMap keyboardLayout;

    public:
        TEST_METHOD_INITIALIZE(InitializeTestEnv)
        {
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to null in a new row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNullInANewRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add 2 empty rows
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, NULL }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, NULL }), std::wstring()));

            // Validate and update the element when -1 i.e. null selection is made on an empty row.
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(0, 0, -1, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[1]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a new row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInANewRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add an empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, NULL }), std::wstring()));

            // Validate and update the element when selecting B on an empty row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(0, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to key
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToKeyRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row with A as the target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, 0x41 }), std::wstring()));

            // Validate and update the element when selecting B on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(0, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)0x41, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to shortcut
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToShortcutRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row with Ctrl+A as the target
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x41);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, dest }), std::wstring()));

            // Validate and update the element when selecting B on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(0, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual(true, dest == std::get<Shortcut>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column to the same value as the right column
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnMapToSameKeyError_OnSettingFirstColumnToSameValueAsRightColumn)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row with A as the target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, 0x41 }), std::wstring()));

            // Validate and update the element when selecting A on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(0, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::MapToSameKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)0x41, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to the same value as in another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToKeyRowToSameValueAsInAnotherRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row from A->B and a row with C as target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ 0x41, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, 0x43 }), std::wstring()));

            // Validate and update the element when selecting A on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::SameKeyPreviouslyMapped);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)0x43, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to the same value as in another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToShortcutRowToSameValueAsInAnotherRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row from A->B and a row with Ctrl+A as target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ 0x41, 0x42 }), std::wstring()));
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x41);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, dest }), std::wstring()));

            // Validate and update the element when selecting A on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::SameKeyPreviouslyMapped);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual(true, dest == std::get<Shortcut>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to a conflicting modifier with another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToKeyRowToConflictingModifierWithAnotherRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row from Ctrl->B and a row with C as target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ VK_CONTROL, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, 0x43 }), std::wstring()));

            // Validate and update the element when selecting LCtrl on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_LCONTROL));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)0x43, std::get<DWORD>(remapBuffer[1].first[1]));

            // Change first row to LCtrl->B
            remapBuffer[0].first[0] = VK_LCONTROL;

            // Select Ctrl
            index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL));
            error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)0x43, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to a conflicting modifier with another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToShortcutRowToConflictingModifierWithAnotherRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add a row from Ctrl->B and a row with Ctrl+A as target
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ VK_CONTROL, 0x42 }), std::wstring()));
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x41);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ NULL, dest }), std::wstring()));

            // Validate and update the element when selecting LCtrl on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_LCONTROL));
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual(true, dest == std::get<Shortcut>(remapBuffer[1].first[1]));

            // Change first row to LCtrl->B
            remapBuffer[0].first[0] = VK_LCONTROL;

            // Select Ctrl
            index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL));
            error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(1, 0, (int)index, keyList, remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual(true, dest == std::get<Shortcut>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateShortcutBufferElement method is successful and no drop down action is required on setting a column to null in a new row
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingColumnToNullInANewRow)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add empty rows
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), NULL }), std::wstring()));

            // Case 1: Validate the element when making null-selection (-1 index) on first column of empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            std::vector<int32_t> selectedIndices = std::vector<int32_t>({ -1 });
            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, -1, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when making null-selection (-1 index) on first column of empty shortcut to key row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, -1, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when making null-selection (-1 index) on second column of empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, -1, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when making null-selection (-1 index) on second column of empty shortcut to key row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, -1, selectedIndices, std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no drop down action is required on setting first drop down to an action key on a non-hybrid control column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnANonHybridColumn)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add empty rows and Ctrl+C->Ctrl+A
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x41);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), NULL }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting A on first dropdown of first column of empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            std::vector<int32_t> selectedIndices = std::vector<int32_t>({ (int32_t)index });
            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on first dropdown of first column of empty shortcut to key row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, (int)index, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting A on first dropdown of second column of empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, selectedIndices, std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting A on first dropdown of first column of valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(2, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting A on first dropdown of second column of valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(2, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting first drop down to an action key on an empty hybrid control column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnAnEmptyHybridColumn)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Add empty rows and Ctrl+C->Ctrl+A
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x41);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), NULL }), std::wstring()));

            // Case 1: Validate the element when selecting A on first dropdown of second column of empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            std::vector<int32_t> selectedIndices = std::vector<int32_t>({ (int32_t)index });
            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, selectedIndices, std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on first dropdown of second column of empty shortcut to key row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, selectedIndices, std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting first drop down to an action key on a hybrid control column with full shortcut
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonLastDropDownToActionKeyOnAHybridColumnWithFullShortcut)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C and Ctrl+Shift+B on right column
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(VK_SHIFT);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), dest1 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting A on first dropdown of second column of hybrid shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));
            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on second dropdown of second column of hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting non first non last drop down to an action key on a non hybrid control column with full shortcut
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonFirstNonLastDropDownToActionKeyOnANonHybridColumnWithFullShortcut)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+Shift+C on left column, Ctrl+Shift+B on right column
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(VK_SHIFT);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(VK_SHIFT);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting A on second dropdown of first column of shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on second dropdown of second column of shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting last drop down to an action key on a column with atleast two drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingLastDropDownToActionKeyOnAColumnWithAtleastTwoDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+Shift+C on left column, Ctrl+Shift+B on right column
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey(VK_SHIFT);
            src1.SetKey(0x43);
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(VK_SHIFT);
            dest1.SetKey(0x42);
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src1, dest1 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src2, dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting A on last dropdown of first column of three key shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on second dropdown of second column of three key shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting A on second dropdown of hybrid second column of three key shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting A on last dropdown of first column of two key shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting A on second dropdown of second column of two key shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting A on second dropdown of hybrid second column of two key shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting non first drop down to an action key on a column if all the drop downs after it are empty
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingNonFirstDropDownToActionKeyOnAColumnIfAllTheDropDownsAfterItAreEmpty)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));

            // Case 1: Validate the element when selecting A on second dropdown of first column of 3 dropdown shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);

            // Case 2: Validate the element when selecting A on second dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);

            // Case 3: Validate the element when selecting A on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);

            // Case 4: Validate the element when selecting A on second dropdown of first column of empty 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);

            // Case 2: Validate the element when selecting A on second dropdown of second column of empty 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);

            // Case 3: Validate the element when selecting A on second dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting first drop down to an action key on a hybrid column if all the drop downs after it are empty
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingFirstDropDownToActionKeyOnAHybridColumnIfAllTheDropDownsAfterItAreEmpty)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));

            // Case 1: Validate the element when selecting A on first dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x41));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no drop down action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and AddDropDown action is required on setting last drop down to a non-repeated modifier key on a column there are less than 3 drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndAddDropDownAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), 0x44 }), std::wstring()));

            // Case 1: Validate the element when selecting A on second dropdown of first column of 2 dropdown shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 2: Validate the element when selecting Shift on second dropdown of second column of 2 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 3: Validate the element when selecting Shift on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 4: Validate the element when selecting Shift on first dropdown of first column of 1 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 5: Validate the element when selecting Shift on first dropdown of second column of 1 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 6: Validate the element when selecting Shift on first dropdown of second column of 1 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);

            // Case 7: Validate the element when selecting Shift on first dropdown of second column of 1 dropdown hybrid shortcut to key row with an action key selected
            result = BufferValidationHelpers::ValidateShortcutBufferElement(2, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and AddDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting last drop down to a repeated modifier key on a column there are less than 3 drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting Ctrl on second dropdown of first column of 2 dropdown shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting Ctrl on second dropdown of second column of 2 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting Ctrl on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting LCtrl on second dropdown of first column of 2 dropdown shortcut to shortcut row
            index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_LCONTROL));
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting LCtrl on second dropdown of second column of 2 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting LCtrl on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a non repeated modifier key on a column there 3 or more drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey(0x43);
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(0x42);
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey(VK_MENU);
            src2.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(VK_MENU);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src1, dest1 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), 0x44 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src2, dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting A on second dropdown of first column of 3 dropdown shortcut to shortcut row with middle empty
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown shortcut to shortcut row with middle empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with middle empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting Shift on second dropdown of first column of 3 dropdown shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 7: Validate the element when selecting Shift on first dropdown of first column of 3 dropdown shortcut to shortcut row with first two empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 8: Validate the element when selecting Shift on first dropdown of second column of 3 dropdown shortcut to shortcut row with first two empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 9: Validate the element when selecting Shift on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first two empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 10: Validate the element when selecting Shift on first dropdown of second column of 3 dropdown hybrid shortcut to key row with an action key selected and with first two empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(2, 1, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 11: Validate the element when selecting Shift on second dropdown of first column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(3, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 12: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(3, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 13: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(3, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a repeated modifier key on a column there 3 or more drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey(0x43);
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(0x42);
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey(VK_MENU);
            src2.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(VK_MENU);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src1, dest1 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src2, dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting Ctrl on second dropdown of first column of 3 dropdown shortcut to shortcut row with middle empty
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown shortcut to shortcut row with middle empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with middle empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting Ctrl on second dropdown of first column of 3 dropdown shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first empty
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 7: Validate the element when selecting Ctrl on third dropdown of first column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 8: Validate the element when selecting Ctrl on third dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 9: Validate the element when selecting Ctrl on third dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no action is required on setting non-last drop down to a non repeated modifier key on a column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingNonLastDropDownToNonRepeatedModifierKeyOnAColumn)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey(0x43);
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(0x42);
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey(VK_MENU);
            src2.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(VK_MENU);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src1, dest1 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src2, dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting Shift on first dropdown of first column of 2 dropdown shortcut to shortcut
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting Shift on first dropdown of second column of 2 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting Shift on first dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting Shift on first dropdown of first column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting Shift on first dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting Shift on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 7: Validate the element when selecting Shift on second dropdown of first column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 8: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 9: Validate the element when selecting Shift on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting non-last drop down to a repeated modifier key on a column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingNonLastDropDownToRepeatedModifierKeyOnAColumn)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // Ctrl+C on left column, Ctrl+B on right column
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey(0x43);
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey(0x42);
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey(VK_MENU);
            src2.SetKey(0x43);
            Shortcut dest2;
            dest2.SetKey(VK_CONTROL);
            dest2.SetKey(VK_MENU);
            dest2.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src2, dest2 }), std::wstring()));

            // Case 1: Validate the element when selecting Shift on first dropdown of first column of 3 dropdown shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU));

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting Alt on first dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting Alt on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_MENU)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            index = std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL));

            // Case 4: Validate the element when selecting Alt on second dropdown of first column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting Ctrl on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no action is required on setting first drop down to None on a non-hybrid column with one drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithOneDropDown)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));

            // Case 1: Validate the element when selecting None on first dropdown of first column of 1 dropdown shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting None on first dropdown of second column of 1 dropdown shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting first drop down to None on a hybrid column with one drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyErrorAndNoAction_OnSettingFirstDropDownToNoneOnHybridColumnWithOneDropDown)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));

            // Case 1: Validate the element when selecting None on first dropdown of first column of 1 dropdown hybrid shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutAtleast2Keys error and no action is required on setting first drop down to None on a non-hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutAtleast2KeysAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting None on first dropdown of first column of 2 dropdown empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutAtleast2Keys);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting None on first dropdown of second column of 2 dropdown empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutAtleast2Keys);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting None on first dropdown of first column of 2 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutAtleast2Keys);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting None on first dropdown of second column of 2 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutAtleast2Keys);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting second drop down to None on a non-hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyAndNoAction_OnSettingSecondDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting None on second dropdown of first column of 2 dropdown empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting None on second dropdown of second column of 2 dropdown empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting None on second dropdown of first column of 2 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting None on second dropdown of second column of 2 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting drop down to None on a hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldNoErrorAndDeleteDropDownAction_OnSettingDropDownToNoneOnHybridColumnWithTwoDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting None on first dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 2: Validate the element when selecting None on second dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 3: Validate the element when selecting None on first dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 4: Validate the element when selecting None on second dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);
        }

        // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting non last drop down to None on a column with three drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldNoErrorAndDeleteDropDownAction_OnSettingNonLastDropDownToNoneOnColumnWithThreeDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(VK_SHIFT);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(VK_SHIFT);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting None on first dropdown of first column of 3 dropdown empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 2: Validate the element when selecting None on second dropdown of first column of 3 dropdown empty hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 3: Validate the element when selecting None on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 4: Validate the element when selecting None on second dropdown of first column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x43)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 5: Validate the element when selecting None on first dropdown of second column of 3 dropdown empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 6: Validate the element when selecting None on second dropdown of second column of 3 dropdown empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 7: Validate the element when selecting None on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 8: Validate the element when selecting None on second dropdown of second column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 9: Validate the element when selecting None on first dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, -1, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 10: Validate the element when selecting None on second dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, true, (int)index, std::vector<int32_t>({ -1, (int32_t)index, -1 }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 11: Validate the element when selecting None on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 0, true, (int)index, std::vector<int32_t>({ (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);

            // Case 12: Validate the element when selecting None on second dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 1, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)index, (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), 0x42)) }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is valid and DeleteDropDown action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting last drop down to None on a column with three drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldShortcutOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNoneOnColumnWithThreeDropDowns)
        {
            std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>> remapBuffer;

            // empty row
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ Shortcut(), Shortcut() }), std::wstring()));
            Shortcut src;
            src.SetKey(VK_CONTROL);
            src.SetKey(VK_SHIFT);
            src.SetKey(0x43);
            Shortcut dest;
            dest.SetKey(VK_CONTROL);
            dest.SetKey(VK_SHIFT);
            dest.SetKey(0x42);
            remapBuffer.push_back(std::make_pair(std::vector<std::variant<DWORD, Shortcut>>({ src, dest }), std::wstring()));

            // Case 1: Validate the element when selecting None on last dropdown of first column of 3 dropdown empty shortcut to shortcut row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            size_t index = 0;

            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 0, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 2: Validate the element when selecting None on last dropdown of first column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 0, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 3: Validate the element when selecting None on last dropdown of second column of 3 dropdown empty shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 4: Validate the element when selecting None on last dropdown of second column of 3 dropdown valid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, false);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 5: Validate the element when selecting None on last dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 2, true, (int)index, std::vector<int32_t>({ -1, -1, (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);

            // Case 6: Validate the element when selecting None on last dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            result = BufferValidationHelpers::ValidateShortcutBufferElement(1, 1, 2, true, (int)index, std::vector<int32_t>({ (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_CONTROL)), (int32_t)std::distance(keyList.begin(), std::find(keyList.begin(), keyList.end(), VK_SHIFT)), (int32_t)index }), std::wstring(), keyList, remapBuffer, true);

            // Assert that the element is invalid and no action is required
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }
    };
}
