#pragma once

#include <windows.h>
#include <string>

namespace rgaa
{

	ATOM RegisterSelfHotKey(UINT modifier, UINT key, const std::wstring& name = L"THE HOT KEY\n") {
		ATOM atom = GlobalAddAtomW((LPWSTR)name.c_str()) - 0xc000;
		RegisterHotKey(NULL, atom, modifier, key);
		return atom;
	}

}