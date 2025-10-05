#include "BluetoothUtils.h"
#include "SettingsUtils.h"
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

std::atomic<bool> stopRequested = false;

// Execute shell command and capture output
std::string execCommand(const std::string &cmd){
    std::string result;
    char buffer[256];
    FILE *pipe = popen(cmd.c_str(),"r");
    if(!pipe) return "";
    while(fgets(buffer,sizeof(buffer),pipe)) result += buffer;
    pclose(pipe);
    return result;
}

// Display menu using Wofi dmenu
std::string wofiSelect(const std::vector<std::string>& options,const std::string &prompt){
    if(options.empty()) return "";
    std::string cmd = "printf '%s\\n'";
    for(auto &o: options) cmd += " '" + o + "'";
    cmd += " | wofi --dmenu --prompt '" + prompt + "'";
    char buffer[256];
    std::string result;
    FILE *pipe = popen(cmd.c_str(),"r");
    if(!pipe) return "";
    if(fgets(buffer,sizeof(buffer),pipe)){
        result = buffer;
        if(!result.empty() && result.back()=='\n') result.pop_back();
    }
    pclose(pipe);
    return result;
}

// List Bluetooth adapters
std::vector<Adapter> listAdapters(){
    std::vector<Adapter> adapters;
    std::string out = execCommand("bluetoothctl list");
    std::istringstream iss(out); std::string line;
    while(std::getline(iss,line)){
        if(line.empty()) continue;
        size_t pos1 = line.find(" "); size_t pos2 = line.find(" ",pos1+1);
        if(pos1==std::string::npos || pos2==std::string::npos) continue;
        Adapter a; a.mac = line.substr(pos1+1,17); a.name = line.substr(pos2+1);
        adapters.push_back(a);
    }
    return adapters;
}

// List Bluetooth devices
std::vector<Device> listDevices(){
    std::vector<Device> devices;
    std::string out = execCommand("bluetoothctl devices");
    std::istringstream iss(out); std::string line;
    while(std::getline(iss,line)){
        if(line.empty()) continue;
        size_t pos1 = line.find(" "); size_t pos2 = line.find(" ",pos1+1);
        if(pos1==std::string::npos || pos2==std::string::npos) continue;
        Device d; d.mac=line.substr(pos1+1,17); d.name=line.substr(pos2+1);
        if(d.mac.find(":")==std::string::npos) continue;
        devices.push_back(d);
    }
    return devices;
}

// Start scanning
void scanOn(){ execCommand("bluetoothctl scan on &"); }
// Stop scanning
void scanOff(){ execCommand("bluetoothctl scan off"); }

// Connect device with timeout

bool connectDeviceWithTimeout(const std::string &mac, int seconds) {
    std::atomic<bool> connected = false;
    std::atomic<bool> done = false;

    std::thread t([&]() {
        execCommand("bluetoothctl connect " + mac);
        if (stopRequested) return;
        std::string info = execCommand("bluetoothctl info " + mac);
        if (info.find("Connected: yes") != std::string::npos)
            connected = true;
        done = true;
    });

    for (int i = 0; i < seconds * 10; ++i) {
        if (connected || stopRequested || done) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (t.joinable()) {
        try {
            t.detach();
        } catch (...) {}
    }

    if (stopRequested) {
        std::cerr << "Connection canceled by user\n";
        return false;
    }

    if (!connected)
        std::cerr << "Timeout connecting to device\n";

    return connected;
}

// Disconnect device
void disconnectDevice(const std::string &mac){ execCommand("bluetoothctl disconnect " + mac); }

// Check if device is connected
bool isDeviceConnected(const std::string &mac){
    std::string info = execCommand("bluetoothctl info " + mac);
    return info.find("Connected: yes")!=std::string::npos;
}

// Send notifications
void sendNotification(const std::string &title, const std::string &message) {
    if(!getBoolSetting("SHOW_NOTIFICATIONS", true)) return;
     execCommand("notify-send \"" + title + "\" \"" + message + "\"");
}
