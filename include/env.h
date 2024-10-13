#ifndef ENV_H
#define ENV_H

#include <string>

class Env {
public:
    Env(const std::string& path);

    void loadEnv();

    void setEnvVar(const std::string &key, const std::string &value);

    const std::string getEnvVar(const std::string &key);

private:
    std::string path;

    void updateEnvFile(const std::string &key, const std::string &value);
};

#endif
