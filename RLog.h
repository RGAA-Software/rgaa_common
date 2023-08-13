#ifndef __SK_LOG__
#define __SK_LOG__

#include <stdio.h>
#include <stdarg.h>

#define SPDLOG_NAME         "spd.log"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>

#define LOG_FILE 1

namespace rgaa 
{

#define LOGD_ SPDLOG_LOGGER_DEBUG
#define LOGI_ SPDLOG_LOGGER_INFO
#define LOGW_ SPDLOG_LOGGER_WARN
#define LOGE_ SPDLOG_LOGGER_ERROR

	class Logger {
	public:

		static bool Init(const std::string& path, bool save_to_file)
		{
			if (save_to_file) {
				std::string strFilePath = path;
				try {
					std::shared_ptr<spdlog::logger> logger;
					logger = spdlog::rotating_logger_mt(SPDLOG_NAME, strFilePath, 1024 * 1024 * 30, 5, false);
					logger->flush_on(spdlog::level::info);;
					spdlog::set_default_logger(logger);
					logger->set_level(spdlog::level::debug);
					// %@ 是全路径名+行数，这里用 %s:%#，不使用全路径
					logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%s:%#,%!][%l] : %v");
				}
				catch (const spdlog::spdlog_ex& ex) {
					std::cerr << "Log path : " << path << std::endl;
					std::cout << "Log initialization failed: " << ex.what() << std::endl;
					return false;
				}
			}
			else {
				//spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%s:%#,%!][%l] : %v");
				spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%s:%#][%l] : %v");
			}
			return true;
		}
	};

#define LOGI(...) LOGI_(spdlog::default_logger_raw(), __VA_ARGS__)
#define LOGD(...) LOGD_(spdlog::default_logger_raw(), __VA_ARGS__)
#define LOGW(...) LOGW_(spdlog::default_logger_raw(), __VA_ARGS__) 
#define LOGE(...) LOGE_(spdlog::default_logger_raw(), __VA_ARGS__) 
	 
#define LOG_INFO(...)  
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#define LOG_WARN(...)

}


#endif
