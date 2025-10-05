#include "BluetoothManager.h"
#include "SettingsManager.h"
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <thread>

extern SettingsManager settings; // shared global

BluetoothManager::BluetoothManager() : scanning(false) {}

BluetoothManager::~BluetoothManager() { stopScanner(); }

// --- Helper: run shell command ---
std::string BluetoothManager::execCommand(const std::string &cmd) {
    std::string result;
    char buffer[256];
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buffer, sizeof(buffer), pipe)) result += buffer;
    pclose(pipe);
    return result;
}

// --- Get Bluetooth adapters ---
std::vector<Adapter> BluetoothManager::getAdapters() {
    std::vector<Adapter> adapters;
    std::string out = execCommand("bluetoothctl list");
    std::istringstream iss(out);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        size_t pos1 = line.find(" ");
        size_t pos2 = line.find(" ", pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) continue;
        adapters.push_back({ line.substr(pos1 + 1, 17), line.substr(pos2 + 1) });
    }
    return adapters;
}

// --- Get devices (safe) ---
std::vector<Device> BluetoothManager::getDevices() {
    std::vector<Device> devices;
    std::string out = execCommand("bluetoothctl devices");
    std::istringstream iss(out);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        size_t pos1 = line.find(" ");
        size_t pos2 = line.find(" ", pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) continue;

        Device d{ line.substr(pos1 + 1, 17), line.substr(pos2 + 1) };
        if (d.mac.find(":") == std::string::npos) continue;
        devices.push_back(d);
    }
    return devices;
}

// --- Background scanner ---
void BluetoothManager::scanLoop() {
    execCommand("bluetoothctl scan on &");
    while (scanning) {
        auto devices = getDevices();
        {
            std::lock_guard<std::mutex> lock(devicesMutex);
            discoveredDevices = devices;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    execCommand("bluetoothctl scan off");
}

void BluetoothManager::startScanner() {
    scanning = true;
    scannerThread = std::thread(&BluetoothManager::scanLoop, this);
}

void BluetoothManager::stopScanner() {
    scanning = false;
    if (scannerThread.joinable()) scannerThread.join();
}

// --- Device operations ---
bool BluetoothManager::connectDeviceWithTimeout(const std::string &mac, int seconds) {
    std::atomic<bool> connected = false, done = false;

    std::thread t([&]() {
        execCommand("bluetoothctl connect " + mac);
        if (execCommand("bluetoothctl info " + mac).find("Connected: yes") != std::string::npos)
            connected = true;
        done = true;
    });

    for (int i = 0; i < seconds * 10; i++) {
        if (connected || done) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (t.joinable()) t.detach();
    return connected;
}

void BluetoothManager::disconnectDevice(const std::string &mac) {
    execCommand("bluetoothctl disconnect " + mac);
}

bool BluetoothManager::isDeviceConnected(const std::string &mac) {
    return execCommand("bluetoothctl info " + mac).find("Connected: yes") != std::string::npos;
}

// --- System notification ---
void BluetoothManager::sendNotification(const std::string &title, const std::string &message) {
    if (!settings.getBool("SHOW_NOTIFICATIONS", true)) return;
    execCommand("notify-send \"" + title + "\" \"" + message + "\"");
}

