#pragma once

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#include "Generated Files\version_gen.h"

#define FILE_VERSION VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, 0
#define FILE_VERSION_STRING  \
    STRINGIZE(VERSION_MAJOR) \
    "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_REVISION) ".0"
#define PRODUCT_VERSION FILE_VERSION
#define PRODUCT_VERSION_STRING FILE_VERSION_STRING

#define COMPANY_NAME "Microsoft Corporation"
#define COPYRIGHT_NOTE "Copyright (C) 2020 Microsoft Corporation"
#define PRODUCT_NAME "PowerToys"

enum class version_architecture
{
    x64,
    arm
};

version_architecture get_current_architecture();
const wchar_t* get_architecture_string(const version_architecture);