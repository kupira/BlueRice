# BlueRice

**Version:** 0.1-beta  

BlueRice is a modular, menu-driven Bluetooth device manager for Linux. It allows connecting, and disconnecting Bluetooth devices via Wofi or future GUIs. Designed for Linux users who like lightweight, configurable interfaces.

---

## Features

- Connect and disconnect devices based on current status
- Highlight connected devices in the menu
- Configurable settings:
  - Show/hide MAC addresses
  - Auto-connect last device
  - Connection timeout
  - Skip menu on successful operation
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
## Configuration (`~/.config/bt-rice/`)

BlueRice automatically creates a configuration directory at first launch:

~/.config/bt-rice/
