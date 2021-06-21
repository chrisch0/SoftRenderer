#pragma once
#include <vector>
#include <cstring>
#include <cctype>

inline 
bool IsLineEnd(char c)
{
	return (c == '\r' || c == '\n' || c == '\0' || c == '\f');
}

inline 
size_t FindLineEnd(const std::string& data, size_t lineBeg)
{
	size_t line_end = lineBeg + 1;
	while (line_end < data.size() && !IsLineEnd(data[line_end]))
	{
		++line_end;
	}
	return line_end;
}

inline 
bool SkipSpaces(const std::string& data, size_t& iter)
{
	while (data[iter] == ' ' || data[iter] == '\t')
	{
		++iter;
	}
	return !IsLineEnd(data[iter]);
}

inline 
void SkipToken(const std::string& data, size_t& begin, size_t end)
{
	while (begin < end && data[begin] != ' ')
	{
		++begin;
	}
}

inline 
bool IsNumeric(const std::string& data, size_t iter)
{
	return (data[iter] >= '0' && data[iter] <= '9') || data[iter] == '+' || data[iter] == '-';
}

inline 
bool IsNanOrInf(const std::string& data, size_t iter)
{
	if (data[iter] == 'N' || data[iter] == 'n')
	{
		return std::tolower(data[iter]) == 'n' && std::tolower(data[iter + 1]) == 'a' && std::tolower(data[iter + 2] == 'n');
	}
	else if (data[iter] == 'I' || data[iter] == 'i')
	{
		return std::tolower(data[iter]) == 'i' && std::tolower(data[iter + 1]) == 'n' && std::tolower(data[iter + 2] == 'f');
	}
	return false;
}

inline 
int CountNumricComponentsInLine(const std::string& data, size_t lineBeg, size_t lineEnd)
{
	int num_components = 0;
	while (lineBeg < lineEnd)
	{
		if (!SkipSpaces(data, lineBeg))
		{
			break;
		}
		bool isNum = IsNumeric(data, lineBeg) || IsNanOrInf(data, lineBeg);
		if (isNum)
		{
			++num_components;
		}
		SkipSpaces(data, lineBeg);
		SkipToken(data, lineBeg, lineEnd);
	}
	return num_components;
}