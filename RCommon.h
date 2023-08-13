#pragma once
#ifdef WIN32

#include <Windows.h>
#include <string>

#pragma comment(lib, "User32.lib")

#include <boost/beast/core/detail/base64.hpp>

namespace rgaa
{

	class RCommon {
	public:

		static bool DontCareDPI();

		static std::string GetExeFolderPath();
		static std::wstring GetExeFolderPathWStr();
		static bool IsFileExist(const std::string& path);
		static void Base64Encode(const std::string& input, std::string& output);
		static void Base64Decode(const std::string& input, std::string& output);

	};

}
#endif
