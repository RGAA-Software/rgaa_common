#pragma once

#include <chrono>
#include <string>
#include <iomanip>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

namespace rgaa 
{

	static uint64_t GetCurrentTimestamp() {
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp
			= std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		std::time_t timestamp = tp.time_since_epoch().count();
		return timestamp;
	}

}