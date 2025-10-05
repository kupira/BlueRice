# BlueRice

**Version:** 0.3.0-beta  

BlueRice is a modular, menu-driven Bluetooth device manager for Linux. It allows connecting, and disconnecting Bluetooth devices via Wofi or future GUIs. Designed for Linux users who like lightweight, configurable interfaces.

---

## Features

- Connect and disconnect devices based on current status
- Highlight connected devices in the menu
- Notifications
- Configurable settings
- Modular architecture for future GUI frontends

---

## Installing

**Dependencies:**

- `g++` (C++17)
- `wofi`
- `bluetoothctl`
- `cmake`
- `make`

You can install `BlueRice` on Arch using AUR: (WARINING: may be temporarily unavailable)

```
yay -S bluerice
```
or manually:
```
git clone https://github.com/kupira/BlueRice.git
cd bluerice
mkdir build && cd build
cmake ..
make
```
The executable will appear as:
```./bluerice```

---
## Configuration (`~/.config/bluerice/settings.conf`)

BlueRice automatically creates a configuration directory at first launch. You can modify settings such as:

```
AUTO_CONNECT_LAST_DEVICE
CONNECT_TIMEOUT
SHOW_MAC
SHOW_NOTIFICATIONS
SKIP_MENU_ON_SUCCESS
```
