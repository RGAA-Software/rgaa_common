//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_MESSAGEQUEUE_H
#define SAILFISH_SERVER_MESSAGEQUEUE_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <atomic>

namespace rgaa {

    class Message;

    using MessageTaskExec = std::function<void(const std::shared_ptr<Message>&)>;

    class MessageTask {
    public:

        static std::shared_ptr<MessageTask> Make(int code, MessageTaskExec&& exec) {
            auto task = std::make_shared<MessageTask>();
            task->action_code_ = code;
            task->action_name_ = "";
            task->action_exec_ = std::move(exec);
            return task;
        }

        int task_id_ = {0};
        int action_code_ {0};
        std::string action_name_ {};
        MessageTaskExec action_exec_;

    };
    using MessageTaskPtr = std::shared_ptr<MessageTask>;

    class MessageQueue {
    public:

        explicit MessageQueue(int max = 1024);
        ~MessageQueue();

        void Queue(const std::shared_ptr<Message>& msg);
        std::shared_ptr<Message> Peek();
        void PollBlocked();
        void Exit();

        int RegisterTask(const MessageTaskPtr& task);
        void RemoveTask(int task_id);

    private:

        void NotifyTasks(const std::shared_ptr<Message>& msg);

    private:

        int max_messages_;
        std::mutex messages_mtx_;
        std::condition_variable messages_cv_;
        std::queue<std::shared_ptr<Message>> messages_;
        std::vector<MessageTaskPtr> tasks_;

        std::atomic_int msg_task_idx_ = 0;

        bool exit_ = false;

    };

}

#endif //SAILFISH_SERVER_MESSAGEQUEUE_H
