#pragma once

#include <string>
#include <memory>
#include <functional>
#include "RData.h"

template<typename T>
using sp = std::shared_ptr<T>;

namespace rgaa
{

	class File {
	public:
		
		static std::string GetFileNameFromPath(const std::string& path);
		static std::string GetFileFolder(const std::string& path);
		static std::string GetFileNameFromPathNoSuffix(const std::string& path);

		static std::string GetFileNameFromPath(const std::wstring& path);
		static std::string GetFileFolder(const std::wstring& path);
		static std::string GetFileNameFromPathNoSuffix(const std::wstring& path);

		static sp<File> OpenForRead(const std::string& path);
		static sp<File> OpenForWrite(const std::string& path);
		static sp<File> OpenForRW(const std::string& path);
		static sp<File> OpenForAppend(const std::string& path);

		static sp<File> OpenForReadB(const std::string& path);
		static sp<File> OpenForWriteB(const std::string& path);
		static sp<File> OpenForRWB(const std::string& path);
		static sp<File> OpenForAppendB(const std::string& path);


		File(const std::string& path, const std::string& mode);
		~File();

		uint64_t Size();

		bool Exists();

		bool IsOpen();

		DataPtr Read(uint64_t offset, uint64_t size, uint64_t& readed_size);
		DataPtr ReadAll();
		void ReadAll(std::function<void(uint64_t offset, DataPtr)> cbk);
		std::string ReadAllAsString();

		int64_t Write(uint64_t offset, DataPtr data);
		int64_t Write(uint64_t offset, const std::string& data);
		int64_t Write(uint64_t offset, const char* data, uint64_t size);

		void Close();

		int GetFileDescriptor();

		static void Test();

	private:

		std::string file_path;

		FILE* fp = nullptr;

	};

	typedef std::shared_ptr<File> FilePtr;
}