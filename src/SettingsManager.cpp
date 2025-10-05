#include "SettingsManager.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>

SettingsManager::SettingsManager()
    : configDir(std::string(getenv("HOME")) + "/.config/bluerice/"),
      settingsFile(configDir + "settings.conf")
{}

std::map<std::string,std::string> SettingsManager::loadSettings() {
    std::map<std::string,std::string> settings;
    std::ifstream f(settingsFile);
    if(!f) return settings;
    std::string line;
    while(std::getline(f,line)){
        if(line.empty() || line[0]=='#') continue;
        size_t eq = line.find('=');
        if(eq != std::string::npos)
            settings[line.substr(0,eq)] = line.substr(eq+1);
    }
    return settings;
}

void SettingsManager::ensureSettings(){
    mkdir(configDir.c_str(),0700);
    std::ifstream f(settingsFile);
    if(f.good()) return;

    std::ofstream ofs(settingsFile);
    ofs << "# BlueRice default settings\n";
    ofs << "LAST_ADAPTER=\n";
    ofs << "LAST_DEVICE=\n";
    ofs << "AUTO_CONNECT_LAST_DEVICE=false\n";
    ofs << "SHOW_MAC=false\n";
    ofs << "SKIP_MENU_ON_SUCCESS=true\n";
    ofs << "CONNECT_TIMEOUT=10\n";
    ofs << "SHOW_NOTIFICATIONS=true\n";
}

std::string SettingsManager::getString(const std::string &key){
    auto s = loadSettings();
    if(s.find(key)!=s.end()) return s[key];
    return "";
}

bool SettingsManager::getBool(const std::string &key, bool defaultValue){
    auto val = getString(key);
    if(val.empty()) return defaultValue;
    return val=="true" || val=="1";
}

int SettingsManager::getInt(const std::string &key, int defaultValue){
    auto val = getString(key);
    if(val.empty()) return defaultValue;
    try { return std::stoi(val); } catch(...){ return defaultValue; }
}

void SettingsManager::setString(const std::string &key, const std::string &value){
    auto s = loadSettings();
    s[key] = value;
    std::ofstream ofs(settingsFile);
    for(auto &p: s) ofs << p.first << "=" << p.second << "\n";
}

