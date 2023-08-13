#include "RData.h"

#include <string.h>
#include "RLog.h"
#include "RFile.h"

namespace rgaa 
{

    Data::Data(const char* src, int size) {
        this->data = (char *) malloc(size);
        if (!this->data) { return; }
        memset(this->data, 0, size);
        if (src) {
            memcpy(this->data, src, size);
        }
        this->size = size;
    }

    std::shared_ptr<Data> Data::From(const std::string& data) {
        return std::make_shared<Data>((char*)data.data(), (int)data.size());
    }

    Data::~Data() {
        if (this->data) {
            free(this->data);
        }
		//auto kb_size = size / 1024;
		//if (kb_size >= 12) {
		//    Log("Release Data : %d KB", kb_size);
		//}
    }

    const char *Data::CStr() {
        return this->data;
    }

    std::string Data::AsString() {
        std::string val;
        val.resize(size);
        memcpy(val.data(), this->data, size);
        return val;
    }

    void Data::ConvertToStr(std::string& out) {
        out.resize(size);
        memcpy((char*)out.data(), this->data, size);
    }

    int Data::Size() {
        return this->size;
    }

    std::shared_ptr<Data> Data::Make(const char *data, int size) {
        return std::make_shared<Data>(data, size);
    }

    char* Data::DataAddr() {
        return this->data;
    }

    std::shared_ptr<Data> Data::Dup() {
        return Data::Make(this->data, this->size);
    }

    void Data::Save(const std::string& path) {
		auto file = File::OpenForWriteB(path);
		file->Write(0, data, size);
		file->Close();
    }

}
