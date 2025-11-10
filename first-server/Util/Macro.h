#pragma once

#define NON_COPYABLE(ClassName)            \
private:                                    \
    ClassName(const ClassName&) = delete;   \
    ClassName& operator=(const ClassName&) = delete;