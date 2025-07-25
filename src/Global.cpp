#include "Global.h"

wstring ToWString(const string& value)
{
    return wstring(value.begin(), value.end());
}

string ToString(const wstring& value)
{
    return string(value.begin(), value.end());
}

void Replace(OUT string& str, IN const string& comp, IN const string& rep)
{
    string temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != string::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}

void Replace(OUT wstring& str, IN const wstring& comp, IN const wstring& rep)
{
    wstring temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != string::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}
