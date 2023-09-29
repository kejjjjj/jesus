#pragma once

#include "pch.hpp"

DWORD Sys_MilliSeconds();

inline ivec2 GetCPos()
{
	tagPOINT p;
	GetCursorPos(&p);

	return { p.x, p.y };
}
inline bool MouseHovered(const ivec2& mins, const ivec2& maxs)
{
	tagPOINT p;
	GetCursorPos(&p);

	return
		p.x > mins.x && p.x < maxs.x
		&& p.y > mins.y && p.y < maxs.y;
}
inline bool ValidNumber(const std::string_view& expr)
{
    size_t dot_count{ 0 }, dot_idx, index{ 0 }, sub_count{ 0 }, sub_idx;

    if (expr.size() == 1) {
        if (!std::isalnum(expr[0]) && expr[0] != '_')
            return false;
    }

    for (const auto& i : expr) {
        switch (i) {

        case '.': //has a decimal
            dot_count++;

            if (dot_count > 1)
                return false;
            dot_idx = index;
            break;
        case '-': //is a negative number

            if (index != 0)  //only the first character can be this
                return false;

            break;

        default:

            if (!std::isdigit(i))
                return false;
            break;
        }
        index++;

    }



    if (dot_count) {

        if (dot_idx == expr.size() - 1) //last character cannot be a dot
            return false;

        if (!std::isdigit(expr[dot_idx + 1])) //next character is not a number
            return false;
    }


    return true;
}
inline bool IsInteger(const std::string_view& expr)
{
	return ValidNumber(expr) && expr.find('.') == std::string_view::npos; //no decimal
}
inline bool IsHex(char c)
{
    return (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') || std::isdigit(c);
}