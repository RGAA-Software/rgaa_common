#pragma once
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <locale>
#include <string>
#include <codecvt>
#include <cctype>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace rgaa 
{
    typedef enum {
        GBK,
        UTF8,
        UNKOWN
    } CODING;

	static std::string UTF8toGBK(const std::string& str) {
		return boost::locale::conv::between(str, "GBK", "UTF-8");
	}

	static std::string GBKtoUTF8(const std::string& str) {
		return boost::locale::conv::between(str, "UTF-8", "GBK");
	}

	static std::wstring GBKtoUNICODE(const std::string& str) {
		return boost::locale::conv::to_utf<wchar_t>(str, "GBK");
	}

	static std::string UNICODEtoGBK(std::wstring wstr) {
		return boost::locale::conv::from_utf(wstr, "GBK");
	}

	static std::string UNICODEtoUTF8(const std::wstring& wstr) {
		return boost::locale::conv::from_utf(wstr, "UTF-8");
	}

	static std::wstring UTF8toUNICODE(const std::string& str) {
		return boost::locale::conv::utf_to_utf<wchar_t>(str);
	}

    static inline std::wstring ToWstring(const std::string& src) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(src);
    }

    static inline std::string ToUTF8(const std::wstring& src) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(src);
    }

	static void Split(const std::string& s, std::vector<std::string>& sv, const char delim = ' ') {
		sv.clear();
		std::istringstream iss(s);
		std::string temp;
		while (std::getline(iss, temp, delim)) {
			sv.emplace_back(std::move(temp));
		}
	}

	static void Split(const std::string& s, std::vector<std::string>& res, const std::string& delimiter) {
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}
		res.push_back(s.substr(pos_start));
	}

	static void ToLower(std::string& data) {
		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	static std::string ToLowerCpy(const std::string& data) {
		return boost::algorithm::to_lower_copy(data);
	}

	static bool StartWith(const std::string& input, const std::string& find) {
		if (input.rfind(find, 0) == 0) {
			return true;
		}
		return false;
	}

	static std::string CopyStr(const std::string& origin) {
		std::string copy;
		copy.resize(origin.size());
		memcpy(copy.data(), origin.data(), origin.size());
		return std::move(copy);
	}

	static void Replace(std::string& origin, const std::string& from, const std::string& to) {
		boost::replace_all(origin, from, to);
	}

#ifdef WIN32
    static inline std::string utf8_to_gbk(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
        std::wstring tmp_wstr = conv.from_bytes(str);

        //GBK locale name in windows
        const char* GBK_LOCALE_NAME = ".936";
        std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
        return convert.to_bytes(tmp_wstr);
    }
#endif

    static int _PreNum(unsigned char byte) {
        unsigned char mask = 0x80;
        int num = 0;
        for (int i = 0; i < 8; i++) {
            if ((byte & mask) == mask) {
                mask = mask >> 1;
                num++;
            }
            else {
                break;
            }
        }
        return num;
    }

    static bool IsUTF8(unsigned char* data, int len) {
        int num = 0;
        int i = 0;
        while (i < len) {
            if ((data[i] & 0x80) == 0x00) {
                // 0XXX_XXXX
                i++;
                continue;
            }
            else if ((num = _PreNum(data[i])) > 2) {
                // 110X_XXXX 10XX_XXXX
                // 1110_XXXX 10XX_XXXX 10XX_XXXX
                // 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                // 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                // 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                // preNUm()        
                i++;
                for (int j = 0; j < num - 1; j++) {
                    //10 start
                    if ((data[i] & 0xc0) != 0x80) {
                        return false;
                    }
                    i++;
                }
            }
            else {
                //
                return false;
            }
        }
        return true;
    }

    static bool IsGBK(unsigned char* data, int len) {
        int i = 0;
        while (i < len) {
            if (data[i] <= 0x7f) {
                //ascii
                i++;
                continue;
            }
            else {
                //
                if (data[i] >= 0x81 &&
                    data[i] <= 0xfe &&
                    data[i + 1] >= 0x40 &&
                    data[i + 1] <= 0xfe &&
                    data[i + 1] != 0xf7) {
                    i += 2;
                    continue;
                }
                else {
                    return false;
                }
            }
        }
        return true;
    }

    // must 1.isUtf8 2.isGBK
    static CODING GetCoding(unsigned char* data, int len) {
        CODING coding;
        if (IsUTF8(data, len) == true) {
            coding = UTF8;
        }
        else if (IsGBK(data, len) == true) {
            coding = GBK;
        }
        else {
            coding = UNKOWN;
        }
        return coding;
    }

    // trim from start (in place)
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
}
