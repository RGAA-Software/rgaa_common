#pragma once

#include <memory>
#include <string>

namespace rgaa 
{

	class Data {
	public:
		Data(const char* data, int size);

		~Data();

		const char* CStr();
		char* DataAddr();
		int Size();
		std::string AsString();
		void ConvertToStr(std::string& out);
		std::shared_ptr<Data> Dup();
		void Save(const std::string& path);

		static std::shared_ptr<Data> Make(const char* data, int size);
		static std::shared_ptr<Data> From(const std::string& data);
	private:

		char* data{ nullptr };
		int   size = 0;

	};


	typedef std::shared_ptr<Data> DataPtr;

}
