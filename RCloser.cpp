#include "RCloser.h"
#include "RTime.h"

namespace rgaa
{
	// Closer

	std::shared_ptr<Closer> Closer::Make(std::function<void()> f) {
		return std::make_shared<Closer>(f);
	}

	Closer::Closer(std::function<void()> f) {
		func = f;
	}

	Closer::~Closer() {
		func();
	}

	// RegionTimeCount

	std::shared_ptr<RegionTimeCount> RegionTimeCount::Make(std::function<void(uint64_t count)> f) {
		return std::make_shared<RegionTimeCount>(f);
	}

	RegionTimeCount::RegionTimeCount(std::function<void(uint64_t count)> f) {
		func = f;
		begin_time = GetCurrentTimestamp();
	}

	RegionTimeCount::~RegionTimeCount() {
		auto end_time = GetCurrentTimestamp();
		func(end_time - begin_time);
	}
}