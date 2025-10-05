#include "WofiFrontend.h"
#include "SettingsManager.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <thread>

extern SettingsManager settings;

std::string WofiFrontend::wofiSelect(const std::vector<std::string>& options,const std::string &prompt){
    if(options.empty()) return "";
    std::string cmd = "printf '%s\\n'";
    for(auto &o: options) cmd += " '" + o + "'";
    cmd += " | wofi --dmenu --prompt '" + prompt + "'";
    char buffer[256]; std::string result;
    FILE *pipe = popen(cmd.c_str(),"r");
    if(!pipe) return "";
    if(fgets(buffer,sizeof(buffer),pipe)){
        result=buffer;
        if(!result.empty() && result.back()=='\n') result.pop_back();
    }
    pclose(pipe);
    return result;
}

void WofiFrontend::run(){
    int connectTimeout = settings.getInt("CONNECT_TIMEOUT", 10);
    bool skipMenu = settings.getBool("SKIP_MENU_ON_SUCCESS");
    bool showMac  = settings.getBool("SHOW_MAC");

    while(true){
        auto devices = bt->getDevices();
        std::vector<std::pair<std::string,std::string>> deviceMenu;
        std::vector<std::string> menuText;

        if(devices.empty()) menuText.push_back("No devices found yet...");
        else{
            for(auto &d: devices){
                std::string entry = d.name;
                if(bt->isDeviceConnected(d.mac)) entry = "[Connected] "+entry;
                if(showMac) entry += " ["+d.mac+"]";
                deviceMenu.push_back({entry,d.mac});
                menuText.push_back(entry);
            }
        }

        std::string selected = wofiSelect(menuText,"Select Device:");
        if(selected.empty()) break;

        std::string mac;
        for(auto &p: deviceMenu) if(p.first==selected) { mac=p.second; break; }
        if(mac.empty()) continue;

        bool success=false;
        if(bt->isDeviceConnected(mac)){
            bt->disconnectDevice(mac);
            success=!bt->isDeviceConnected(mac);
            if(success) bt->sendNotification("BlueRice","Disconnected from "+selected);
        } else{
            success=bt->connectDeviceWithTimeout(mac,connectTimeout);
            if(success){
                bt->sendNotification("BlueRice","Connected to "+selected);
                settings.setString("LAST_DEVICE",mac);
            }
        }

        if(skipMenu && success) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

