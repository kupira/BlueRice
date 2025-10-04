#include "SettingsUtils.h"
#include <fstream>
#include <sstream>
#include <map>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>

const std::string CONFIG_DIR = std::string(getenv("HOME")) + "/.config/bluerice/";
const std::string SETTINGS_FILE = CONFIG_DIR + "settings.conf";

// Load settings from file into a map
static std::map<std::string,std::string> loadSettings() {
    std::map<std::string,std::string> settings;
    std::ifstream f(SETTINGS_FILE);
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

// Ensure default settings file exists
void ensureSettings(){
    mkdir(CONFIG_DIR.c_str(),0700);
    std::ifstream f(SETTINGS_FILE);
    if(f.good()) return;

    std::ofstream ofs(SETTINGS_FILE);
    ofs << "# bt-rice default settings\n";
    ofs << "LAST_ADAPTER=\n";
    ofs << "LAST_DEVICE=\n";
    ofs << "AUTO_CONNECT_LAST_DEVICE=false\n";
    ofs << "SHOW_MAC=false\n";
    ofs << "SKIP_MENU_ON_SUCCESS=false\n";
    ofs << "CONNECT_TIMEOUT=10\n";
}

// Get string setting
std::string getSetting(const std::string &key){
    auto s = loadSettings();
    if(s.find(key)!=s.end()) return s[key];
    return "";
}

// Get boolean setting
bool getBoolSetting(const std::string &key){
    auto val = getSetting(key);
    return val=="true" || val=="1";
}

// Get integer setting with default
int getIntSetting(const std::string &key, int defaultValue){
    auto val = getSetting(key);
    if(val.empty()) return defaultValue;
    try { return std::stoi(val); } catch(...){ return defaultValue; }
}

// Set/update a setting
void setSetting(const std::string &key, const std::string &value){
    auto s = loadSettings();
    s[key] = value;
    std::ofstream ofs(SETTINGS_FILE);
    for(auto &p: s) ofs << p.first << "=" << p.second << "\n";
}

