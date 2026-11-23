#!/bin/bash

source ./scripts/bash-ini-parser.sh

# Remove the tmp folder first to be sure we start with a clean sheet
rm -rf ./tmp

# Setup the folder structure
mkdir -p tmp
mkdir -p tmp/os_mac_64/UserPlugins
mkdir -p tmp/os_win_x64/UserPlugins
mkdir -p tmp/os_win_32/UserPlugins
mkdir -p tmp/os_linux_x86_64/UserPlugins
mkdir -p tmp/UserPlugins/ReaSonus

source ./scripts/changelog.sh

# Copy localizations file, changelog and reaper-package-info.txt
cp ./src/i18n/locales/en-US.ini ./tmp/UserPlugins/ReaSonus
cp ./src/changelog.ini ./tmp/UserPlugins/ReaSonus
cp ./scripts/reaper-package-info.txt ./tmp

# Have to copy the assets generated to the correct folders
cp ./artifacts/reasonus-native-macos-latest-arm64\;x86_64/reaper_ReasonusNative-arm64-x86_64.dylib ./tmp/os_mac_64/UserPlugins
cp ./artifacts/reasonus-native-windows-2022-x64/reaper_ReasonusNative-x64.dll ./tmp/os_win_x64/UserPlugins
cp ./artifacts/reasonus-native-windows-2022-x86/reaper_ReasonusNative-x86.dll ./tmp/os_win_32/UserPlugins
cp ./artifacts/reasonus-native-ubuntu-latest-x86_64/reaper_ReasonusNative-x86_64.so ./tmp/os_linux_x86_64/UserPlugins

# Read the file and set the latest version number
file="./tmp/reaper-package-info.txt"
version_placeholder=%version%
sed -i -r "s|$version_placeholder|$1|g" $file

# Zip all the contents
cd tmp || exit
zip -X9r ../reasonusNative.ReaperPkg *
cd .. || exit

