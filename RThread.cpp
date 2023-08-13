#include "RThread.h"

#include <iostream>
#include <sstream>

namespace rgaa {

	ThreadPtr Thread::Make(const std::string& name, int max_task) {
		return std::make_shared<Thread>(name, max_task);
	}

	Thread::Thread(const std::string& name, int max_task) : max_tasks(max_task) {
		this->name = name;
	}

	Thread::Thread(OnceTask&& task, const std::string& name, bool join) {
		this->name = name;
		t = std::make_shared<std::thread>(task);
		if (join) {
			t->join();
		}
		else {
			//t->detach();
		}
	}

	Thread::~Thread() {
		Exit();
	}

	void Thread::Detach() {
		if (t) {
			t->detach();
		}
	}

	void Thread::Poll() {
		{
			std::lock_guard<std::mutex> guard(init_mtx);
			if (init) {
				return;
			}
			init = true;
		}

		t = std::make_shared<std::thread>([this]() {
			TaskLoop();
		});
		t->detach();
	}


	void Thread::TaskLoop() {
		for (;;) {
			{
				std::unique_lock<std::mutex> lk(task_mtx);
				take_var.wait(lk, [this]() {
					return !tasks.empty() || exit;
				});
			}
			if (exit) {
				break;
			}

			ThreadTaskPtr task = nullptr;
			{
				std::lock_guard<std::mutex> guard(task_mtx);
				if (!tasks.empty()) {
					task = tasks.front();
				}
				if (task) {
					tasks.remove(task);
				}
			}
			if (task) {
				task->Run();
				task_exec_count++;
			}
		}

		exit_loop = true;
	}


	void Thread::Post(const ThreadTaskPtr& task) {
		std::lock_guard<std::mutex> guard(task_mtx);
		if (max_tasks > 0 && (int)tasks.size() >= max_tasks) {
			tasks.pop_front();
		}
		tasks.push_back(task);
		take_var.notify_all();
	}

	void Thread::Post(ThreadTaskPtr&& task) {
		std::lock_guard<std::mutex> guard(task_mtx);
		if (max_tasks > 0 && (int)tasks.size() >= max_tasks) {
			tasks.pop_front();
		}
		tasks.push_back(std::move(task));
		take_var.notify_all();
	}

	bool Thread::IsExit() {
		return exit_loop;
	}

	bool Thread::IsLastTaskReturned() {
		return last_task_returned;
	}

	void Thread::Exit() {
		exit = true;
		take_var.notify_all();
		if (t && t->joinable()) {
			t->join();
		}
	}

	bool Thread::HasTask() {
		return TaskSize() > 0;
	}

	int Thread::TaskSize() {
		std::lock_guard<std::mutex> guard(task_mtx);
		return tasks.size();
	}

	bool Thread::IsJoinable() {
		return t && t->joinable();
	}

	void Thread::Join() {
		if (t) {
			t->join();
		}
	}

	unsigned long Thread::ExecCount() {
		return task_exec_count;
	}

	int Thread::MaxTaskSize() {
		return max_tasks;
	}

	std::string Thread::GetId() {
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}

}
