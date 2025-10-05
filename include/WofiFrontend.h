#pragma once
#include "BluetoothManager.h"

class Frontend {
public:
    virtual ~Frontend() {}
    virtual void run() = 0;
};

class WofiFrontend : public Frontend {
public:
    WofiFrontend(BluetoothManager* bt) : bt(bt) {}
    void run() override;

private:
    BluetoothManager* bt;
    std::string wofiSelect(const std::vector<std::string>& options,const std::string &prompt);
};

