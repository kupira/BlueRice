# BlueRice

**Version:** 0.2-beta  

BlueRice is a modular, menu-driven Bluetooth device manager for Linux. It allows connecting, and disconnecting Bluetooth devices via Wofi or future GUIs. Designed for Linux users who like lightweight, configurable interfaces.

---

## Features

- Connect and disconnect devices based on current status
- Highlight connected devices in the menu
- Notifications
- Configurable settings
- Modular architecture for future GUI frontends

---

## Installation

**Dependencies:**

- `g++` (C++17)
- `make`
- `wofi`
- `bluetoothctl`

**Build:**

```bash
git clone https://github.com/kupira/BlueRice.git
cd BlueRice
make
```
---
## Configuration (`~/.config/bluerice/settings.conf`)

BlueRice automatically creates a configuration directory at first launch.
