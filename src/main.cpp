#include "SettingsManager.h"
#include "BluetoothManager.h"
#include "WofiFrontend.h"
#include <memory>

SettingsManager settings;

int main(int argc, char** argv){
    settings.ensureSettings();

    BluetoothManager bt;
    bt.startScanner();

    std::unique_ptr<Frontend> frontend;
    if(argc>1 && std::string(argv[1])=="--gui"){
        // frontend = std::make_unique<ImGuiFrontend>(&bt); // future
    } else {
        frontend = std::make_unique<WofiFrontend>(&bt);
    }

    frontend->run();
    bt.stopScanner();

    return 0;
}

