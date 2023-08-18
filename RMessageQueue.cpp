//
// Created by RGAA on 2023/8/10.
//

#include "RMessageQueue.h"
#include "RMessage.h"
#include "RLog.h"

namespace rgaa {

    MessageQueue::MessageQueue(int max) {
        max_messages_ = max;
    }

    MessageQueue::~MessageQueue() {

    }

    void MessageQueue::Queue(const std::shared_ptr<Message>& msg) {
        std::lock_guard<std::mutex> guard(messages_mtx_);
        messages_.push(msg);
        if (messages_.size() > max_messages_) {
            messages_.pop();
        }
        messages_cv_.notify_all();
    }

    std::shared_ptr<Message> MessageQueue::Peek() {
        std::unique_lock<std::mutex> guard(messages_mtx_);
        messages_cv_.wait(guard, [this]() -> bool {
            return !messages_.empty();
        });

        auto msg = messages_.front();
        messages_.pop();
        return msg;
    }

    int MessageQueue::RegisterTask(const MessageTaskPtr& task) {
        int task_id = ++msg_task_idx_;
        task->task_id_ = task_id;
        tasks_.push_back(task);
        return task_id;
    }

    void MessageQueue::RemoveTask(int task_id) {
        auto it = tasks_.begin();
        while (it != tasks_.end()) {
            auto task = *it;
            if (task->task_id_ == task_id) {
                it = tasks_.erase(it);
            }
            else {
                it++;
            }
        }
    }

    void MessageQueue::NotifyTasks(const std::shared_ptr<Message>& msg) {
        for (const auto& task : tasks_) {
            if (task && task->action_code_ == msg->code && task->action_exec_) {
                task->action_exec_(msg);
            }
        }
    }

    void MessageQueue::PollBlocked() {
        while (!exit_) {
            auto msg = Peek();
            if (msg->code == kCodeExit) {
                break;
            }
            this->NotifyTasks(msg);
        }
    }

    void MessageQueue::Exit() {
        exit_ = true;
        this->Queue(ExitMessage::Make());
    }

}