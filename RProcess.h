#pragma once

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace bp = ::boost::process;

namespace rgaa
{

	bool ProcessNamesToProcessIds(const std::vector<std::wstring>& process_names, std::vector<DWORD>& pid_array);

	class ProcessInfo {
	public:
		std::string name;
		DWORD pid;
	};

	class WindowInfo {
	public:

		std::pair<int, int> GetWindowSize() {
			if (!win_handle) {
				return std::make_pair(0, 0);
			}
			RECT rect;
			GetWindowRect(win_handle, &rect);
			return std::make_pair((rect.right - rect.left), (rect.bottom - rect.top));
		}

	public:
		DWORD pid;
		HWND win_handle = nullptr;
		std::wstring title;
		std::wstring exe_name;
		std::wstring exe_path;
		std::string claxx;
	};

	class WindowInfos {
	public:

		int pid = 0;
		int filter_window_size = -1;
		std::vector<WindowInfo> infos;

	};

	class RProcess {
	public:

		static std::shared_ptr<RProcess> Make(const std::string& exec, const std::vector<std::string>& args);

		static DWORD GetProcessIdByName(const std::string& name);
		static std::vector<DWORD> GetProcessIdsByName(const std::string& name);
		static bool TraverseProcesses(std::map<DWORD, ProcessInfo>& processes);
		// if window size < filter_window_size , ignore
		static WindowInfos GetWindowInfoByPid(DWORD pid, int filter_window_size = -1);

		static std::vector<DWORD> GetProceeIDsfromParentID(DWORD dwParentProcessId);
		static void Kill(uint64_t pid);

		RProcess(const std::string& exec, const std::vector<std::string>& args);
		void Start();

	private:

		std::string exec_path;
		std::vector<std::string> exec_args;
		bp::child child;
	};

}

#endif
