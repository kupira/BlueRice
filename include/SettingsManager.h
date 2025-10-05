#pragma once
#include <string>
#include <map>

class SettingsManager {
public:
    SettingsManager();
    void ensureSettings();

    std::string getString(const std::string &key);
    bool getBool(const std::string &key, bool defaultValue=false);
    int getInt(const std::string &key, int defaultValue=0);
    void setString(const std::string &key, const std::string &value);

private:
    std::map<std::string,std::string> loadSettings();
    const std::string configDir;
    const std::string settingsFile;
};

