#include "Parser/StringEncoding.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlstr.h>

namespace Angel::Parser
{

    std::wstring from_utf8(const std::string_view& utf8)
    {
        if (utf8.empty()) return std::wstring(); // nothing to do
        std::wstring ws;
        int cc = 0;
        // get length (cc) of the new widechar excluding the \0 terminator first
        if ((cc = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size() + 1, NULL, 0)) > 0)
        {
            // convert
            ws.resize(cc-1);
            if (!MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size() + 1, ws.data(), cc))
                throw std::exception("Failed to convert");
        }
        else
        {
            throw std::exception("Failed to convert");
        }
        return ws;
    }

    std::string to_utf8(const std::wstring_view& ws)
    {
        if (ws.empty()) return ""; // nothing to do
        CStringA utf8;
        int cc = 0;
        // get length (cc) of the new multibyte string excluding the \0 terminator first
        if ((cc = WideCharToMultiByte(CP_UTF8, 0, ws.data(), ws.size(), NULL, 0, 0, 0)) > 0)
        {
            // convert
            char *buf = utf8.GetBuffer(cc+1);
            if (!WideCharToMultiByte(CP_UTF8, 0, ws.data(), ws.size(), buf, cc, 0, 0))
                throw std::exception("Failed to convert");
            buf[cc] = 0;
            utf8.ReleaseBuffer();
        }
        else
        {
            throw std::exception("Failed to convert");
        }
        return utf8.GetBuffer();
    }

}