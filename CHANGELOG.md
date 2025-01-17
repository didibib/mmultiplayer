# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.3] - 2023-10-14

### Added
- 2 more stuff to the speedometer in the "Show Extra Info"
  - SZ = Stored Z of LastJumpLocation.Z
  - SZD = Delta of Location.Z - LastJumpLocation.Z

### Fixed
- Faith being unable to move after teleporting while grabbing or rolling 

## [2.1.2] - 2023-07-16

### Fixed
- Installer broken error handling (add additional logging)
- Windows 11 Defender support

## [2.1.1] - 2023-07-15

### Added
- Troubleshooting section in CONTRIBUTING.md

### Changed
- User can no longer configure install directory. Install is set to default install location (C:\Program Files\Mirror's Edge Multiplayer)

### Removed
- Swat Sniper and Riot Cop as selectable models (both caused crash during loading)

### Fixed
- Fixed installer not creating a Windows Security exclusion

## [2.1.0] - 2023-07-12

### Added
- Tag Addon
- Misc Addon
  - Autoroll
  - No consequtive wallruns limit 
  - No wallrun challenge
  - No wallclimb challenge
  - No health regeneration
  - Permanent reaction time
  - Permanent game speed
  - One hit knock down
- Sidestepbeamer
- Reset button next to bindable keys
- Button to reset the trainer save and checkpoint
- Button to refill reaction time energy
- Toggable option to show your top height in player info
- 4 more selectable models
  - Ropeburn
  - Riot Cop
  - Swat
  - Swat Sniper

### Fixed
- Creating a save and reloading level and teleporting would cause faith to autoroll
- Creating a save no longer saves bools about the input (elevator state)
- Dying and teleporting showed a black screen
- Arms not being reset to default postion when teleporting
- Sound not being reset when teleporting during reaction time
- Reaction time energy not being saved using trainer
- Teleporting while sliding didn't stop the sliding sound
  
### Changed
- Styling in tabs to add more space between stuff (except dolly tab)  
- Flying speed to reset after being toggled
- Player info to be more like speedometer
- Default flying speed to be 2.0f instead of 3.0f

### Removed
- Player tab
- Option to toggle the overlay in trainer
