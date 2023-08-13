#pragma once

#include "RObject.h"

namespace rgaa
{

	class ExitObject : public RObject {
	public:
		ExitObject();
		~ExitObject();

		virtual void Exit();
		virtual bool ShouldExit();

	private:

		bool exit = false;

	};

}