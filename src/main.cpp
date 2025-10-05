#include "BluetoothUtils.h"
#include "SettingsUtils.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>

int main() {
    // --- Ensure default settings exist ---
    ensureSettings();

    int connectTimeout = getIntSetting("CONNECT_TIMEOUT", 10);
    bool skipMenuOnSuccess = getBoolSetting("SKIP_MENU_ON_SUCCESS");
    bool showMac = getBoolSetting("SHOW_MAC");

    // --- Adapter selection ---
    std::string lastAdapter = getSetting("LAST_ADAPTER");
    auto adapters = listAdapters();
    std::vector<Adapter> realAdapters;

    // Filter out invalid adapters
    for (auto &a : adapters) 
        if (a.mac.find(":") != std::string::npos) realAdapters.push_back(a);

    if (realAdapters.empty()) { 
        std::cerr << "No valid adapters\n"; 
        return 1; 
    }

    std::string selectedAdapter;
    bool found = false;

    // Use last adapter if still available
    if (!lastAdapter.empty()) {
        for (auto &a : realAdapters) 
            if (a.mac == lastAdapter) { selectedAdapter = a.mac; found = true; break; }
    }

    if (!found) {
        // Let user select adapter if multiple are present
        std::vector<std::string> adapterOptions;
        for (auto &a : realAdapters) adapterOptions.push_back(a.name);

        if (adapterOptions.size() == 1) selectedAdapter = realAdapters[0].mac;
        else {
            std::string choice = wofiSelect(adapterOptions, "Select Adapter:");
            if (choice.empty()) return 0;
            for (auto &a : realAdapters) if (a.name == choice) { selectedAdapter = a.mac; break; }
        }
        setSetting("LAST_ADAPTER", selectedAdapter);
    }

    // --- Auto-connect last device ---
    if (getBoolSetting("AUTO_CONNECT_LAST_DEVICE")) {
        std::string lastDeviceMac = getSetting("LAST_DEVICE");
        if (!lastDeviceMac.empty() && connectDeviceWithTimeout(lastDeviceMac, connectTimeout))
            std::cout << "Connected to last device\n";
    }

    // --- Live scanning setup ---
    std::atomic<bool> scanning{ true };
    std::vector<Device> discoveredDevices;
    std::mutex devicesMutex;

    std::thread scanner([&]() {
        scanOn();
        while (scanning) {
            auto devices = listDevices();
            {
                std::lock_guard<std::mutex> lock(devicesMutex);
                discoveredDevices = devices;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        scanOff();
    });

    // Wait for initial scan results (max 5 seconds)
    {
        int retries = 0;
        while (discoveredDevices.empty() && retries < 5) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            retries++;
        }
    }

    // --- Main device menu loop ---
    while (true) {
        std::vector<std::pair<std::string, std::string>> deviceMenu;
        std::vector<std::string> menuText;

        {
            std::lock_guard<std::mutex> lock(devicesMutex);
            if (discoveredDevices.empty()) {
                menuText.push_back("No devices found yet...");
            }
            else {
                // Build menu entries from discovered devices
                for (auto &d : discoveredDevices) {
                    std::string entry = d.name;
                    if (isDeviceConnected(d.mac)) entry = "[Connected] " + entry;
                    if (showMac) entry += " [" + d.mac + "]";
                    deviceMenu.push_back({ entry, d.mac });
                    menuText.push_back(entry);
                }
            }
        }

        // Show device selection menu
        std::string selected = wofiSelect(menuText, "Select Device:");
        if (selected.empty()) {
            stopRequested = true;
            break; // user closed menu
        }

        // Find the MAC address for the selected device safely
        std::string mac;
        for (auto &p : deviceMenu) {
            if (p.first == selected) {
                mac = p.second;
                break;
            }
        }

        if (mac.empty()) { 
            std::cerr << "MAC not found for selected device. Skipping...\n"; 
            continue; // safely skip this iteration
        }

        bool success = false;

        if (isDeviceConnected(mac)) {
            // --- Disconnect the device ---
            disconnectDevice(mac);
            success = !isDeviceConnected(mac); // success = true if disconnected
            if (success) {
                std::cout << "Disconnected: " << selected << "\n";
                sendNotification("BlueRice", "Disconnected from " + selected);
            }
        } 
        else {
            // --- Connect to the device ---
            success = connectDeviceWithTimeout(mac, connectTimeout);
            if (success) {
                std::cout << "Connected: " << selected << "\n";
                sendNotification("BlueRice", "Connected to " + selected);
                setSetting("LAST_DEVICE", mac); // only save if connect succeeded
            } else {
                std::cout << "Failed to connect: " << selected << "\n";
                sendNotification("BlueRice", "Failed to connect to " + selected);
                continue; // skip skipMenuOnSuccess check to avoid segfault
            }
        }

        // Close menu if the operation succeeded and setting allows
        if (skipMenuOnSuccess && success) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // --- Cleanup scanner ---
    scanning = false;
    if (scanner.joinable()) scanner.join();

    return 0;
}

