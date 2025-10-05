#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

struct Adapter { std::string mac, name; };
struct Device  { std::string mac, name; };

class BluetoothManager {
public:
    BluetoothManager();
    ~BluetoothManager();

    void startScanner();
    void stopScanner();
    std::vector<Device> getDevices();

    std::vector<Adapter> getAdapters();
    bool connectDeviceWithTimeout(const std::string &mac, int seconds);
    void disconnectDevice(const std::string &mac);
    bool isDeviceConnected(const std::string &mac);
    void sendNotification(const std::string &title, const std::string &message);

private:
    std::atomic<bool> scanning;
    std::vector<Device> discoveredDevices;
    std::mutex devicesMutex;

    std::string execCommand(const std::string &cmd);
    void scanLoop();
    std::thread scannerThread;
};

