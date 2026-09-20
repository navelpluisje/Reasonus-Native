![ReaSonus version](https://img.shields.io/github/v/release/navelpluisje/reasonus-native?style=flat&label=ReaSonus%20version)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/navelpluisje/reasonus-native/cmake-multi-platform.yml?style=flat&label=Last%20ReaSonus%20Build)
![GitHub License](https://img.shields.io/github/license/navelpluisje/reasonus-native?style=flat)

# ReaSonus Faderport Native

Documentation can be found here: [https://navelpluisje.github.io/reasonus/](https://navelpluisje.github.io/reasonus/)

## Install

Using `vcpkg` for package management

- git clone .....
- remove the build folder (You might otherwise end up in cache issues which can cause errors)
- git submodule init
- git submodule update
- Run vcpkg bootstrap
    - **Windows**: `.\vcpkg\bootstrap-vcpkg.bat -disableMetrics`
    - **Others**: `./vcpkg/bootstrap-vcpkg.sh -disableMetrics`
- Install the packages:
    - **Windows**: `.\vcpkg\vcpkg.exe install`
    - **Others**: `./vcpkg/vcpkg install`
- Add configure argument:
    - **Windows**: `-DCMAKE_TOOLCHAIN_FILE="vcpkg\scripts\buildsystem\vcpkg.cmake"`
    - **Others**: `-DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystem/vcpkg.cmake"`

## Contribute

Feel free to contribute. You can contribute inmultiple ways:

- Report an issue. If you fond an issue, please report it and provide the information needed.
- If you found an issue and are able to fix it yourself:
    - Fork the repository
    - Edit the code
    - Create a pull request

Always respect the code of conduct.

## Notes to self

## Adding Setting

- `shared/csurf_utils.cpp > readAndCreateIni`: add the setting(s)
- `shared/csurf_utils.cpp > validateReaSonusIni`: add the setting(s)
- `shared/csurf_context.cpp`: add the setting(s) with the getter(s) and setter(s)
- `csurf_faderport_x/csurf_fp_X_main.cpp`: set the current value in the context
- `csurf_faderport_x/csurf_fp_X_ui_init.cpp`: add the control
- `res.rc`: Add the setting to the init ui

FaderPort 8 & 16 only

- `csurf_faderport_8/csurf_fp_8_menu_manager.cpp`: Add the new setting
