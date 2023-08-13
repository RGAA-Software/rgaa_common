#include "RProcess.h"

#ifdef WIN32

#include <iostream>
#include <psapi.h>
#include <Tlhelp32.h>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <boost/process/windows.hpp>
#include <Poco/Process.h>

#include "RLog.h"
#include "RString.h"

namespace rgaa
{

	template <typename T>
	static bool AddToArray(std::vector<T>& _array, T& item) {
		for (const auto& e : _array) {
			if (e == item) {
				return false;
			}
		}
		_array.emplace_back(item);
		return true;
	}

	static inline bool IsInArray(const std::vector<std::wstring>& _array,
		const std::wstring& item) {
		for (auto& e : _array) {
			if (0 == _wcsicmp(e.data(), item.data())) {
				return true;
			}
		}
		return false;
	}

	bool ProcessNamesToProcessIds(const std::vector<std::wstring>& process_names, std::vector<DWORD>& pid_array) {
		HANDLE process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == process_snap) {
			return false;
		}

		bool result = false;
		PROCESSENTRY32 pe32 = {};
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(process_snap, &pe32)) {
			do {
				if (pe32.th32ProcessID && IsInArray(process_names, std::wstring(pe32.szExeFile))) {
					AddToArray(pid_array, pe32.th32ProcessID);
					result = true;
				}
			} while (Process32Next(process_snap, &pe32));
		}
		CloseHandle(process_snap);
		return result;
	}

	bool RProcess::TraverseProcesses(std::map<DWORD, ProcessInfo>& processes)
	{
#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
			return false;
		}

		BOOL bResult = Process32First(hProcessSnap, &pe32);
		int num(0);
		while (bResult) {
			std::string name = pe32.szExeFile;
			DWORD id = pe32.th32ProcessID;
			//std::cout << "[" << ++num << "] : " << "Process Name:" << name << "  " << "ProcessID:" << id << std::endl;
			processes.insert(std::make_pair(id, ProcessInfo{name, id}));
			bResult = Process32Next(hProcessSnap, &pe32);
		}

		CloseHandle(hProcessSnap);
		return true;
	}

	DWORD RProcess::GetProcessIdByName(const std::string& name) {
		std::map<DWORD, ProcessInfo> name_pid_pairs;
		bool result = TraverseProcesses(name_pid_pairs);
		if (!result) {
			return 0;
		}
		for (auto& [pid, info] : name_pid_pairs) {
			if (info.name == name) {
				return pid;
			}
		}
		return 0;
	}

	std::vector<DWORD> RProcess::GetProcessIdsByName(const std::string& name) {
		std::vector<DWORD> ids;
		std::map<DWORD, ProcessInfo> name_pid_pairs;
		bool result = TraverseProcesses(name_pid_pairs);
		if (!result) {
			return ids;
		}
		for (auto& [pid, info] : name_pid_pairs) {
			if (info.name == name) {
				ids.push_back(pid);
			}
		}
		return ids;
	}

	static bool GetProcessNameByPid(uint32_t pid, std::wstring& exe_name, std::wstring& exe_path)
	{
		HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (processHandle == NULL) {
			return false;
		}

		exe_name.resize(MAX_PATH);
		auto len = GetModuleBaseNameW(processHandle, NULL, exe_name.data(), MAX_PATH);
		if (len == 0) {
			LOG_ERROR("Get base name failed, err: %u", GetLastError());
			return false;
		}

		exe_path.resize(MAX_PATH);
		GetModuleFileNameExW(processHandle, NULL, exe_path.data(), MAX_PATH);
		
		//GetProcessImageFileNameA(processHandle, tempProcName, MAX_PATH);
		CloseHandle(processHandle);
		return true;
	}

	WindowInfos RProcess::GetWindowInfoByPid(DWORD pid, int filter_window_size) {
		WindowInfos infos;
		infos.pid = pid;
		infos.filter_window_size = filter_window_size;

		EnumWindows([](HWND hwnd, LPARAM lParam) {
			auto infos = reinterpret_cast<WindowInfos*>(lParam);

			DWORD find_pid;
			auto result = GetWindowThreadProcessId(hwnd, &find_pid);
			if (find_pid == infos->pid) {

				WindowInfo window_info;

				auto title_length = GetWindowTextLengthW(hwnd);
				if (title_length > 0) {
					title_length++;
				}
				std::wstring title(title_length, 0);
				GetWindowTextW(hwnd, title.data(), title_length);

				//std::string class_name;
				//class_name.resize(256);
				//memset((char*)class_name.data(), 0, class_name.size());
				char path[256] = {0};
				memset(path, 0, 256);
				GetClassNameA(hwnd, path, 256);
				//GetClassNameA(hwnd, (char*)class_name.data(), class_name.size());

				window_info.pid = find_pid;
				window_info.title = title;
				window_info.win_handle = hwnd;
				window_info.claxx = std::string(path);// class_name;

				// filter small window
				auto size = window_info.GetWindowSize();
				if (infos->filter_window_size > 0) {
					if (size.first <= infos->filter_window_size 
						|| size.second <= infos->filter_window_size) {
						window_info.win_handle = nullptr;
						return TRUE;
					}
				}

				if ("ConsoleWindowClass" == window_info.claxx) {
					LOG_INFO("filter the window : ConsoleWindowClass");
					return TRUE;
				}

				GetProcessNameByPid(find_pid, window_info.exe_name, window_info.exe_path);
				//LOG_INFO("pid : %d , find : %d, result : %d, title : %s, exe : %s , path : %s ", window_info.pid, find_pid, result, UNICODEtoGBK(title).c_str(), UNICODEtoGBK(window_info.exe_name).c_str(), UNICODEtoGBK(window_info.exe_path).c_str());
				infos->infos.push_back(window_info);
			}

			return TRUE;
		}, reinterpret_cast<LPARAM>(&infos));

		return infos;
	}

	std::shared_ptr<RProcess> RProcess::Make(const std::string& exec, const std::vector<std::string>& args) {
		return std::make_shared<RProcess>(exec, args);
	}

	RProcess::RProcess(const std::string& exec, const std::vector<std::string>& args) {
		exec_path = exec;
		exec_args = args;
	}

	void RProcess::Start() {
		std::error_code ec;
		child = bp::child(exec_path, exec_args, ec);
		std::cout << "start error code : " << ec << std::endl;
	}

	std::vector<DWORD> RProcess::GetProceeIDsfromParentID(DWORD dwParentProcessId) {
		std::vector<DWORD> parent_id_vec;
		DWORD dwProcessID = 0;

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return parent_id_vec;
		}

		PROCESSENTRY32 pe;
		TCHAR procID[100] = { 0 };
		pe.dwSize = sizeof(pe);
		BOOL bProcess = Process32First(hProcessSnap, &pe);
		
		while (bProcess) {
			if (pe.th32ParentProcessID == dwParentProcessId) {
				dwProcessID = pe.th32ProcessID;
				parent_id_vec.push_back(dwProcessID);
			}
			bProcess = Process32Next(hProcessSnap, &pe);
		}
		CloseHandle(hProcessSnap);
		return parent_id_vec;
	}

	void RProcess::Kill(uint64_t pid) {
		if (Poco::Process::isRunning(pid)) {
			Poco::Process::kill(pid);
		}
	}

}
#endif
