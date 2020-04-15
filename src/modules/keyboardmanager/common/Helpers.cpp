#include "pch.h"
#include "Helpers.h"
#include <sstream>

// Function to split a wstring based on a delimiter and return a vector of split strings
std::vector<std::wstring> splitwstring(const std::wstring& input, wchar_t delimiter)
{
    std::wstringstream ss(input);
    std::wstring item;
    std::vector<std::wstring> splittedStrings;
    while (std::getline(ss, item, L' '))
    {
        splittedStrings.push_back(item);
    }

    return splittedStrings;
}

// Function to return the next sibling element for an element under a stack panel
IInspectable getSiblingElement(IInspectable const& element)
{
    FrameworkElement frameworkElement = element.as<FrameworkElement>();
    StackPanel parentElement = frameworkElement.Parent().as<StackPanel>();
    uint32_t index;

    parentElement.Children().IndexOf(frameworkElement, index);
    return parentElement.Children().GetAt(index + 1);
}

// Function to check if the key is a modifier key
bool IsModifierKey(DWORD key)
{
    return (GetKeyType(key) != KeyType::Action);
}

// Function to get the type of the key
KeyType GetKeyType(DWORD key)
{
    switch (key)
    {
    case VK_LWIN:
    case VK_RWIN:
        return KeyType::Win;
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
        return KeyType::Ctrl;
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
        return KeyType::Alt;
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
        return KeyType::Shift;
    default:
        return KeyType::Action;
    }
}
