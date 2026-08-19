#include "WideStringUtils.hpp"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace WideStringUtils
{
    std::wstring utf8ToWString(const std::string& str) noexcept
    {
#if defined(_WIN32)
        if (str.empty())
        {
            return L"";
        }
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], sizeNeeded);
        return wstrTo;
#endif
    }

    std::string WStringToString(const std::wstring& wstr) noexcept
    {
#if defined(_WIN32)
        if (wstr.empty())
        {
            return std::string();
        }

        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

        std::string strTo(sizeNeeded, 0);

        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);

        return strTo;
#endif
    }
}; // namespace WideStringUtils