#pragma once

#include "RExit.h"

namespace rgaa
{

	ExitObject::ExitObject() : RObject() {

	}

	ExitObject::~ExitObject() {

	}

	void ExitObject::Exit() {
		exit = true;
	}

	bool ExitObject::ShouldExit() {
		return exit;
	}

}