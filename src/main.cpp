#include "api.h"
#include "database.h"
#include "env.h"
#include "file_manger.h"
#include "interface.h"
#include "logger.h"
#include "paths.h"
#include <cstdio>
#include <stdio.h>
#include <tgbot/tgbot.h>

int main() {
    try {
        Env env(ENV_PATH);
        env.loadEnv();

        const std::string TOKEN = env.getEnvVar("TOKEN");

        if (TOKEN.empty()) {
            printf("please add your bot TOKEN at .env\n");
            return 0;
        }

        TgBot::Bot bot(TOKEN);
        Api api(bot, env);

        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::string chatId = env.getEnvVar("CHAT_ID");
            if (chatId.empty() || chatId == "0"){
                printf("please write to your bot to add your id\nYou can always change it in .env\n\n");
                longPoll.start();
            } else{
                printf("your id has been added\nYou can always change it in .env\n");
                break;
            }
        }

        Database db(DB_PATH);
        FileManager fileManager(api, db);
        Interface Interface(fileManager);

        Interface.run();

    } catch (TgBot::TgException& e) {
        std::string errmsg = "TgBot error: " + std::string(e.what());
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::string errmsg = "Runtime error: " + std::string(e.what());
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
        return 1;
    } catch (...) {
        std::string errmsg = "An uncaught exception has been thrown";
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
        throw;
    }
    
    return 0;
}
