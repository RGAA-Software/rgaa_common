#include "RWindow.h"

#ifdef WIN32
#include <iostream>

namespace rgaa
{

	bool WinInfo::GetWindowPositionByHwnd(HWND hwnd, RECT& rect) {
		if (!GetClientRect(hwnd, &rect)) {
			return false;
		}

		{
			POINT point{ rect.left, rect.top };
			if (!ClientToScreen(hwnd, &point)) {
				return false;
			}
			rect.left = point.x;
			rect.top = point.y;
		}
		{
			POINT point{ rect.right, rect.bottom};
			if (!ClientToScreen(hwnd, &point)) {
				return false;
			}
			rect.right = point.x;
			rect.bottom = point.y;
		}

		return true;
	}

}
#endif
