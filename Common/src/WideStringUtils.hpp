#pragma once

#include <string>

namespace WideStringUtils
{
    std::wstring utf8ToWString(const std::string& str) noexcept;
    std::string WStringToString(const std::wstring& wstr) noexcept;
}; // namespace WideStringUtils
