//
// Created by RGAA on 2023/8/10.
//

#include "RMessageQueue.h"

#include "RMessage.h"

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
        messages_cv_.notify_one();
    }

    std::shared_ptr<Message> MessageQueue::Peek() {
        std::unique_lock<std::mutex> guard(messages_mtx_);
        if (messages_.empty()) {
            messages_cv_.wait(guard);
        }
        auto msg = messages_.front();
        messages_.pop();
        return msg;
    }

    void MessageQueue::RegisterTask(int target_code, MessageTask&& task) {
        if(tasks_.find(target_code) == tasks_.end()) {
            std::vector<MessageTask> tks;
            tks.push_back(std::move(task));
            tasks_.insert(std::make_pair(target_code, tks));
        }
        else {
            auto& tks = tasks_[target_code];
            tks.push_back(std::move(task));
        }
    }

    void MessageQueue::NotifyTasks(const std::shared_ptr<Message>& msg) {
        if (tasks_.find(msg->code) != tasks_.end()) {
            auto& tks = tasks_[msg->code];
            for (const auto& tk : tks) {
                tk(msg);
            }
        }
    }

    void MessageQueue::Poll() {
        std::shared_ptr<Message> msg = nullptr;
        while ((msg = this->Peek()) != nullptr) {
            if (msg->code == kCodeExit) {
                break;
            }
            this->NotifyTasks(msg);
        }
    }

    void MessageQueue::Exit() {
        this->Queue(ExitMessage::Make());
    }

}