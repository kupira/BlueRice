# Changelog
All notable changes to **BlueRice** will be documented here.

## [0.3.0-beta] - 2025-10-05
### Added
- `BluetoothManager` class for handling scanning, connecting, and disconnecting devices.
- `Device` and `Adapter` structs for better modularity.
- Settings handling separated into `SettingsManager` (modularized).
- Migrated from make to CMake

### Changed
- Refactored project structure for modular code.
- Improved thread-safety for scanning devices.
- Notifications now use centralized settings check.

## [0.2.1-beta] - 2025-10-05
### Fixed
- Notifications enabled by default

## [0.2.0-beta] - 2025-10-05
### Added
- Notifications
- Notification toggle (`SHOW_NOTIFICATIONS`)
- Proper handling when user closes Wofi

### Fixed
- Hanging process when connection fails or menu closed
- Improved thread cleanup during connection attempts

## [0.1.0-beta] - 2025-10-04
### Added
- Initial release
- Device connection via Wofi
- Adapter auto-selection and settings persistence

