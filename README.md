# Espanso Helper

A Qt-based GUI application for managing Espanso text expansion snippets.

## Features

- Add new snippets
- Edit existing snippets
- Delete snippets
- View all snippets in a table
- Automatic YAML validation
- Debug logging
- Save changes directly to Espanso config

## Requirements

- Qt 6.x
- CMake 3.16 or higher
- yaml-cpp library
- C++17 compatible compiler

## Building

1. Install dependencies:
   ```bash
   vcpkg install yaml-cpp:x64-windows
   ```

2. Create build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```

4. Build:
   ```bash
   cmake --build . --config Release
   ```

## Usage

1. Run the application
2. The application will automatically load your Espanso snippets from the default location
3. Use the buttons to manage your snippets:
   - Add New: Create a new snippet
   - Edit: Modify the selected snippet
   - Delete: Remove the selected snippet
   - Save: Save changes to the Espanso config file
   - Reload: Refresh the snippet list from the config file

## Debugging

The application includes extensive debug logging. You can view the debug output in the console or by redirecting it to a file:

```bash
espanso_helper.exe > debug.log 2>&1
``` 