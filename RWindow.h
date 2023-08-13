#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rgaa
{

	class WinInfo {
	public:

		static bool GetWindowPositionByHwnd(HWND hwnd, RECT& rect);
		
	};

}
#endif
