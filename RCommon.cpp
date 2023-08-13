#include "RCommon.h"
#include "RString.h"

#include <Poco/File.h>

#ifdef WIN32
namespace rgaa
{

	bool RCommon::DontCareDPI() {
		typedef HRESULT* (__stdcall* SetProcessDpiAwarenessFunc)(DPI_AWARENESS_CONTEXT);
		bool res = true;
		HMODULE shCore = LoadLibraryA("User32.dll");
		if (shCore)
		{
			SetProcessDpiAwarenessFunc setProcessDpiAwareness =
				(SetProcessDpiAwarenessFunc)GetProcAddress(shCore, "SetProcessDpiAwarenessContext");

			if (setProcessDpiAwareness != nullptr)
			{
				setProcessDpiAwareness(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			}
			else {
				res = false;
			}
			FreeLibrary(shCore);
		}
		return res;
	}

	std::string RCommon::GetExeFolderPath() {
		char szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
		(strrchr(szFilePath, '\\'))[0] = 0;
		std::string path = szFilePath;
		return path;
	}

	std::wstring RCommon::GetExeFolderPathWStr() {
		std::string path = GetExeFolderPath();
		return ToWstring(path);
	}

	bool RCommon::IsFileExist(const std::string& path) {
		Poco::File file(path);
		return file.exists();
	}

	void RCommon::Base64Encode(const std::string& input, std::string& output) {
		std::size_t len = input.size();
		output.resize(boost::beast::detail::base64::encoded_size(len));
		output.resize(boost::beast::detail::base64::encode(&output[0], input.c_str(), len));
	}

	void RCommon::Base64Decode(const std::string& input, std::string& output) {
		std::size_t len = input.size();
		output.resize(boost::beast::detail::base64::decoded_size(len));
		auto result = boost::beast::detail::base64::decode(&output[0], input.data(), len);
		output.resize(result.first);
	}

}

#endif
