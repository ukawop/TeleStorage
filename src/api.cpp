#include "api.h"
#include "logger.h"
#include "paths.h"
#include <stdio.h>

Api::Api(TgBot::Bot& bot, Env& env) : bot(bot), env(env){
    chatId = env.getEnvVar("CHAT_ID");
    bot.getEvents().onAnyMessage([&bot, this](TgBot::Message::Ptr message) {
        anyMessage(message);
    });
}

void Api::anyMessage(TgBot::Message::Ptr message) {
    if (chatId.empty() || chatId == "0") {
        chatId = std::to_string(message->chat->id);
        env.setEnvVar("CHAT_ID", chatId);
        bot.getApi().sendMessage(chatId, "Hi!, your id has been successfully added");
        LOG("New chat id has been added");
        return;
    }

    bot.getApi().sendMessage(chatId, "Your id has been successfully added, if you want to change it, then change the .env file");
}

TgBot::Message::Ptr Api::sendFile(const std::string& documentPath) {
    return bot.getApi().sendDocument(chatId, TgBot::InputFile::fromFile(documentPath, ""));
}

std::string Api::getFile(const std::string& fileId, const std::string& outputFilePath) {
    auto filePtr = bot.getApi().getFile(fileId);

    std::string fileContent = bot.getApi().downloadFile(filePtr->filePath);
    
    std::string fileName = filePtr->filePath.substr(filePtr->filePath.find_last_of('/') + 1);
    std::string filePath = outputFilePath + fileName;

    std::ofstream outputFile(filePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::string errmsg = "Unable to open file for writing with file id = " + std::string(fileId) + " : " + filePath;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    outputFile.write(fileContent.data(), fileContent.size());
    outputFile.close();

    return filePath;
}
