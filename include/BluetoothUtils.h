#pragma once
#include <string>
#include <vector>
#include <atomic>
extern std::atomic<bool> stopRequested;

struct Adapter{ std::string name, mac; };
struct Device{ std::string name, mac; };

std::vector<Adapter> listAdapters();
std::vector<Device> listDevices();
void scanOn();
void scanOff();
bool connectDeviceWithTimeout(const std::string &mac, int seconds);
void disconnectDevice(const std::string &mac);
bool isDeviceConnected(const std::string &mac);
std::string wofiSelect(const std::vector<std::string>& options,const std::string &prompt);
std::string execCommand(const std::string &cmd);
std::string getDeviceInfo(const std::string &mac);
void sendNotification(const std::string &title, const std::string &message);
