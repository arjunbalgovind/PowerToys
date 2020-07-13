#pragma once
#include "Shortcut.h"
#include <variant>

// This class stores all the variables associated with each shortcut remapping
class RemapShortcut
{
public:
    std::variant<DWORD, Shortcut> target;
    bool isShortcutInvoked;
    ModifierKey winKeyInvoked;

    RemapShortcut(const Shortcut& sc) :
        target(sc), isShortcutInvoked(false), winKeyInvoked(ModifierKey::Disabled)
    {
    }

    RemapShortcut(const DWORD& key) :
        target(key), isShortcutInvoked(false), winKeyInvoked(ModifierKey::Disabled)
    {
    }

    RemapShortcut() :
        isShortcutInvoked(false), winKeyInvoked(ModifierKey::Disabled)
    {
    }
};
