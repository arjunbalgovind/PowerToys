#pragma once
#include <vector>
#include <keyboardmanager/common/Helpers.h>
#include <variant>

namespace LoadingAndSavingRemappingHelper
{
    // Function to check if the set of remappings in the buffer are valid
    KeyboardManagerHelper::ErrorType CheckIfRemappingsAreValid(const std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>>& remappings);

    // Function to return the set of keys that have been orphaned from the remap buffer
    std::vector<DWORD> GetOrphanedKeys(std::vector<std::pair<std::vector<std::variant<DWORD, Shortcut>>, std::wstring>>& remappings);

    // Function to combine remappings if the L and R version of the modifier is mapped to the same key
    void CombineRemappings(std::unordered_map<DWORD, std::variant<DWORD, Shortcut>>& table, DWORD leftKey, DWORD rightKey, DWORD combinedKey);

    // Function to pre process the remap table before loading it into the UI
    void PreProcessRemapTable(std::unordered_map<DWORD, std::variant<DWORD, Shortcut>>& table);
}
