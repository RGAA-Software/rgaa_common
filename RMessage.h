//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_MESSAGE_H
#define SAILFISH_SERVER_MESSAGE_H

#include <memory>

namespace rgaa {

    constexpr int kCodeExit = -1;

    class Message {
    public:

        explicit Message(int c) : code(c) {

        }

        int code;


    };

    class ExitMessage : public Message {
    public:

        explicit ExitMessage(int c) : Message(c) {}

        static std::shared_ptr<ExitMessage> Make() {
            return std::make_shared<ExitMessage>(kCodeExit);
        }

    };

}

#endif //SAILFISH_SERVER_MESSAGE_H