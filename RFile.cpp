#include "RFile.h"
#include "RString.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <io.h>
#endif
#include <boost/filesystem.hpp>
#include <iostream>

namespace rgaa
{

	std::string File::GetFileNameFromPath(const std::string& path) {
		std::string target_path = path;
		Replace(target_path, R"(\\)", R"(/)");
		Replace(target_path, R"(\)", R"(/)");
		int idx = target_path.rfind("/");
		if (idx >= 0) {
			return target_path.substr(idx + 1, target_path.size());
		}
		return "";
	}

	std::string File::GetFileFolder(const std::string& path) {
		std::string target_path = path;
		Replace(target_path, R"(\\)", R"(/)");
		Replace(target_path, R"(\)", R"(/)");
		int idx = target_path.rfind("/");
		if (idx >= 0) {
			return target_path.substr(0, idx);
		}
		return "";
	}

	std::string File::GetFileNameFromPathNoSuffix(const std::string& path) {
		auto filename = GetFileNameFromPath(path);
		int idx = filename.rfind(".");
		return filename.substr(0, idx);
	}

	std::string File::GetFileNameFromPath(const std::wstring& path) {
		auto p = UNICODEtoUTF8(path);
		return GetFileNameFromPath(p);
	}

	std::string File::GetFileFolder(const std::wstring& path) {
		auto p = UNICODEtoUTF8(path);
		std::cout << "P : " << p << std::endl;
		return GetFileFolder(p);
	}

	std::string File::GetFileNameFromPathNoSuffix(const std::wstring& path) {
		auto p = UNICODEtoUTF8(path);
		return GetFileNameFromPathNoSuffix(p);
	}

	sp<File> File::OpenForRead(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "r");
	}

	sp<File> File::OpenForWrite(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "w");
	}

	sp<File> File::OpenForRW(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "w+");
	}

	sp<File> File::OpenForAppend(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "a+");
	}

	sp<File> File::OpenForReadB(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "rb");
	}

	sp<File> File::OpenForWriteB(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "wb");
	}

	sp<File> File::OpenForRWB(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "wb+");
	}

	sp<File> File::OpenForAppendB(const std::string& path) {
		return std::make_shared<File>(path.c_str(), "ab+");
	}

	File::File(const std::string& path, const std::string& mode) {
		auto origin_path = path;
		Replace(origin_path, R"(\\)", "/");
		Replace(origin_path, R"(\)", "/");
		this->file_path = origin_path;

		auto target_path = UTF8toUNICODE(path);

#ifdef _WIN32
		_wfopen_s(&fp, target_path.c_str(), UTF8toUNICODE(mode).c_str());
#else
        //fp = fopen(target_path.c_str(), mode.c_str());
#endif

	}

	File::~File() {
		Close();
	}

	bool File::Exists() {
		auto p = boost::filesystem::path(file_path.c_str());
#ifdef _WIN32
		return fp != nullptr;//_waccess(p.c_str(), 0) == 0;
#else
        //return _access(this->file_path.c_str(), 0) == 0;
#endif
	}

	bool File::IsOpen() {
		return fp != nullptr;
	}

	DataPtr File::Read(uint64_t offset, uint64_t size, uint64_t& readed_size) {
		if (!IsOpen()) {
			return nullptr;
		}

		rewind(fp);

		char* readed_data = (char*)malloc(size);

		fseek(fp, offset, SEEK_SET);
		readed_size = fread(readed_data, 1, size, fp);
		if (readed_size <= 0) {
			free(readed_data);
			return nullptr;
		}

		auto data = Data::Make(readed_data, readed_size);
		free(readed_data);
		return data;
	}

	DataPtr File::ReadAll() {
		uint64_t readed_size;
		return Read(0, Size(), readed_size);
	}

	void File::ReadAll(std::function<void(uint64_t, DataPtr)> cbk) {
		uint64_t offset = 0;
		uint64_t file_size = Size();
		uint32_t block_size = 1 * 1024 * 1024;

		while (offset < file_size) {
			uint64_t readed_size = 0;
			auto data = Read(offset, block_size, readed_size);
			if (data && readed_size != 0) {
				cbk(offset, data);
				offset += readed_size;
			}
		}
	}

	std::string File::ReadAllAsString() {
		auto data = ReadAll();
		if (data) {
			return data->AsString();
		}
		return "";
	}

	uint64_t File::Size() {
		if (!IsOpen()) {
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		auto size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		return size;
	}

	int64_t File::Write(uint64_t offset, DataPtr data) {
		if (!data) {
			return -1;
		}
		return Write(offset, data->CStr(), data->Size());
	}

	int64_t File::Write(uint64_t offset, const std::string& data) {
		if (data.empty()) {
			return -1;
		}
		return Write(offset, data.c_str(), data.size());
	}

	int64_t File::Write(uint64_t offset, const char* data, uint64_t size) {
		if (!IsOpen()) {
			return -1;
		}

		rewind(fp);

		fseek(fp, offset, SEEK_SET);
		auto writed_size = fwrite(data, 1, size, fp);
		return writed_size;
	}

	void File::Close() {
		if (fp) {
			fflush(fp);
			fclose(fp);
			fp = nullptr;
		}
	}

	int File::GetFileDescriptor() {
		if (fp) {
			return fileno(fp);
		}
		return -1;
	}

}
