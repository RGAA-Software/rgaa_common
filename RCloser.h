#pragma once

#include <memory>
#include <functional>

namespace rgaa 
{
	class Closer {
	public:

		static std::shared_ptr<Closer> Make(std::function<void()> f);

		Closer(std::function<void()> f);
		~Closer();

	private:

		std::function<void()> func;
	};

	class RegionTimeCount {
	public:

		static std::shared_ptr<RegionTimeCount> Make(std::function<void(uint64_t count)> f);

		RegionTimeCount(std::function<void(uint64_t count)> f);
		~RegionTimeCount();

	private:
		std::function<void(uint64_t count)> func;

		uint64_t begin_time = 0;
	};
}