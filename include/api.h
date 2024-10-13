#ifndef API_H
#define API_H

#include "env.h"
#include <tgbot/tgbot.h>

class Api {
public:
    Api(TgBot::Bot& bot, Env& env);

    void anyMessage(TgBot::Message::Ptr message);

    TgBot::Message::Ptr sendFile(const std::string& documentPath);

    std::string getFile(const std::string& fileId, const std::string& outputFilePath);

private:
    TgBot::Bot& bot;
    Env& env;
    std::string chatId;
};

#endif
