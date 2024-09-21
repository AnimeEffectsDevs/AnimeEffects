# AnimeEffects

🇯🇵 (Outdated) [日本語](https://github.com/AnimeEffectsDevs/AnimeEffects/blob/master/README-jp.md)

A 2D animation tool which doesn't require a carefully thought-out plan, it simplifies animation by providing various functions based on the deformation of polygon meshes.<br>
Originally developed by hidefuku, it is now being developed and maintained by its community.

* Official Website:<br>
  * https://animeeffectsdevs.github.io/

* Official socials:<br>
  * Discord: <a href='https://discord.gg/sKp8Srm'>AnimeEffects Community Server</a> (courtesy of @Jose-Moreno)<br>
  * Twitter: <a href='https://twitter.com/anime_effects'>AnimeEffects</a> (maintained by @p_yukusai)<br>
  * [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/V7V04YLC3)

Note: For the present there may be incompatible changes made, these will be made known in the release affected should they occur.<br>
***If you have any issues or wish to suggest new features, feel reach out to us on our socials!***

## Downloads
* The latest AnimeEffects release is available [here](https://github.com/AnimeEffectsDevs/AnimeEffects/releases), to use it just extract its contents to a folder of your choosing, and then simply run the executable.<br>

## Requirements
* Windows/Linux/Mac
  * See compatible versions below.
* Processor: A 64 bit CPU
* RAM: 4GB
* Graphics card: Intel UHD Graphics/AMD Vega Graphics or any card that supports OpenGL 4.0 or higher
* Display: 1360x720 (The GUI has been made for displays with this resolution or higher)
* [FFmpeg](https://ffmpeg.org/download.html) (Necessary for video exporting, you can place it on your path or copy it to the "/tools" folder.)

## OS Targets
#### This is what we are compiling and testing the software on, it may work on older versions but this is discouraged.
* Windows 10 or newer.
* Ubuntu LTS or comparable distro.
* MacOS Big Sur or newer.

## Development requirements
* Qt 6.6.X
* Vulkan Headers
* CMake 3.16 or later.
* MSVC/GCC/CLang (64-bit)

## Linux (Debian)
### Compilation and AppImage creation
* Most of these dependencies are unnecessary as they come with your distro so check against your own packages:
```
sudo apt update && sudo apt upgrade -y
sudo apt install -y software-properties-common g++ make cmake ninja-build wget rsync build-essential libglib2.0-0 
sudo apt install -y libgl1-mesa-dev file libvulkan-dev openssl python3 python3-pip libxcb-cursor0 libxrandr2 wget
pip install -U pip
pip install aqtinstall
aqt install-qt linux desktop 6.6.2 gcc_64 -m qtimageformats qtmultimedia qt5compat
git clone https://github.com/AnimeEffectsDevs/AnimeEffects
cd AnimeEffects
cmake -S . -B build -G "Ninja Multi-Config"
cmake --build build --config Release
cd build/src/gui/Release 
mkdir -p appdir
cp AnimeEffects appdir
cp -R ../data appdir/data
cp -R ../../../../dist appdir/dist
cp ../../../../dist/AnimeEffects.png appdir
find appdir/
export APPIMAGE_EXTRACT_AND_RUN=1
wget -c -nv "https://github.com/p-yukusai/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/dist/AnimeEffects.desktop -extra-plugins=imageformats,multimedia,core5compat -appimage -verbose=2       
chmod a+x AnimeEffects-x86_64.AppImage
```

## Windows
### Compilation and folder creation
* The installation steps assume you have installed all the requirements through their installers and you have them in your path
```powershell
git clone https://github.com/AnimeEffectsDevs/AnimeEffects
cd AnimeEffects
cmake -S . -B build -G "Ninja Multi-Config"
cmake --build build --config Release
cd build/src/gui/Release
mkdir .\AnimeEffects-Windows-x64
windeployqt --dir .\AnimeEffects-Windows-x64 .\AnimeEffects.exe
Copy-Item -Path "..\data" -Destination ".\AnimeEffects-Windows-x64\" -recurse -Force
Copy-Item ".\AnimeEffects.exe" ".\AnimeEffects-Windows-x64\"
```

## MacOS
### Compilation and .app creation
* These steps assume xcode, brew, wget, python 3 and pip are installed on your system
```bash
brew install cmake ninja vulkan-headers
pip install -U pip
pip install aqtinstall
aqt install-qt mac desktop 6.6.2 clang_64 -m qtimageformats qtmultimedia qt5compat
git clone https://github.com/AnimeEffectsDevs/AnimeEffects
cd AnimeEffects
cmake -S . -B build -G "Ninja Multi-Config"
cmake --build build --config Release
cd build/src/gui/Release 
mkdir -p appdir/usr/lib
cp -R AnimeEffects.app appdir/AnimeEffects.app
cp -R ../data appdir/data
cp -R ../../../../dist appdir/dist
find appdir/
cd appdir
macdeployqt AnimeEffects.app
wget https://raw.githubusercontent.com/OpenZWave/ozw-admin/master/scripts/macdeployqtfix.py && chmod a+x macdeployqtfix.py
./macdeployqtfix.py AnimeEffects.app /usr/local/Cellar/qt/*/
```
