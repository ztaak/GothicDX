#pragma once

#include <Windows.h>
#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class LOG
{
public:
	static std::string argsToStr(char* fmt, std::va_list args)
	{
		va_list tmp_args;
		va_copy(tmp_args, args);
		const int required_len = vsnprintf(nullptr, 0, fmt, tmp_args) + 1;
		va_end(tmp_args);

		std::string buf(required_len, '\0');
		if (std::vsnprintf(&buf[0], buf.size(), fmt, args) < 0) {
			throw std::runtime_error{ "string_vsprintf encoding error" };
		}
		return buf;
	}
	static void ERR(char* fmt, ...)
	{
		std::va_list args;
		va_start(args, fmt);
		std::string str{ argsToStr(fmt, args) };
		va_end(args);

		LPCSTR mess = str.c_str();
		MessageBoxA(NULL, mess, "Error", NULL);
	}
	static void DEB(char* fmt, ...)
	{
		std::va_list args;
		va_start(args, fmt);
		std::string str{ argsToStr(fmt, args) };
		va_end(args);
		
	}
};

