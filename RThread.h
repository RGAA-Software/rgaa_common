#ifndef COMM_THREAD_H
#define COMM_THREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <list>

namespace rgaa {

	template<typename T>
	using sp = std::shared_ptr<T>;

	typedef std::function<void()> VoidFunc;

	class ThreadTask {
	public:
		ThreadTask() = default;
		virtual ~ThreadTask() = default;
		virtual void Run() = 0;
	private:

	};

	class SimpleThreadTask : public ThreadTask {
	public:

		static sp<SimpleThreadTask> Make(VoidFunc ef, VoidFunc cbk) {
			return std::make_shared<SimpleThreadTask>(ef, cbk);
		}

		static sp<SimpleThreadTask> Make(VoidFunc ef) {
			VoidFunc cbk = []() {};
			return std::make_shared<SimpleThreadTask>(ef, cbk);
		}

		SimpleThreadTask(VoidFunc ef, VoidFunc cbk) {
			exec_func = ef;
			callback = cbk;
		}

		void Run() override {
			if (exec_func) {
				exec_func();
			}
			if (callback) {
				callback();
			}
		}

	private:
		VoidFunc exec_func;
		VoidFunc callback;

	};

	template <class ExecFunc, class CallbackFunc>
	class ReturnThreadTask : public ThreadTask {
	public:

		static sp<ReturnThreadTask> Make(ExecFunc ef, CallbackFunc cbk) {
			return std::make_shared<ReturnThreadTask>(ef, cbk);
		}

		ReturnThreadTask(ExecFunc ef, CallbackFunc cbk) {
			exec_func = ef;
			callback = cbk;
		}

		void Run() override {
			if (exec_func) {
				auto ret = exec_func();
				if (callback) {
					callback(ret);
				}
			}
		}

	private:
		ExecFunc exec_func;
		CallbackFunc callback;

	};


	typedef std::shared_ptr<ThreadTask> ThreadTaskPtr;
	typedef std::function<void()>       OnceTask;

	class Thread
	{
	public:
		static std::shared_ptr<Thread> Make(const std::string& name, int max_task);


		Thread() = delete;
		explicit Thread(const std::string& name, int max_task = -1);
		Thread(OnceTask&& task, const std::string& name, bool join = true);
		virtual ~Thread();

		void Poll();

		void Post(const ThreadTaskPtr& task);
		void Post(ThreadTaskPtr&& task);
		bool HasTask();
		int TaskSize();
		int MaxTaskSize();

		void Exit();

		bool IsExit();
		bool IsLastTaskReturned();
		bool IsJoinable();
		void Join();
		void Detach();

		unsigned long ExecCount();

		std::string GetId();

	private:

		void TaskLoop();

		std::mutex                init_mtx;
		bool                      init{ false };

		std::mutex                task_mtx;
		std::list<ThreadTaskPtr>  tasks;

		std::shared_ptr<std::thread>  t;

		std::condition_variable   take_var;

		bool                      exit{ false };
		bool                      exit_loop{ false };

		int                       max_tasks = -1;
		bool                      last_task_returned{ false };
		unsigned long             task_exec_count{ 0 };

		std::string               name;
	};


	typedef std::shared_ptr<Thread>  ThreadPtr;


}

#endif // THREAD_H
