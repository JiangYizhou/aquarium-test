#  Native Aquarium
Native Aquarium is a native implementation of [WebGL Aquarium](https://github.com/WebGLSamples/WebGLSamples.github.io). The goal of this project is to compare the performance of WebGL version to its native counterparts, such as OpenGL, D3D, Vulkan, Metal, Angle and Dawn.

# Progress
<table>
  <tr align=center>
    <td><strong>OS</td>
    <td><strong>Backend</td>
    <td><strong>Supported</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>OpenGL</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>macOS</td>
    <td>OpenGL</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>OpenGL</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>Angle</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>Dawn</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>macOS</td>
    <td>Dawn</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Windows</td>
    <td>Dawn</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>*</td>
    <td>D3D</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>*</td>
    <td>Vulkan</td>
    <td>N</td>
  </tr>
  <tr align=left>
    <td>*</td>
    <td>Metal</td>
    <td>N</td>
  </tr>
</table>


## Build Aquarium by GN

Native Aquarium uses gn to build on Linux, macOS and Windows.
```sh
# cd the repo
cd aquarium-test

# download thirdparty
gclient sync

# Build on Windows
# To build release version, --args="is_debug=false"
gn gen out/build --ide=vs
open out/build/all.sln using visual studio.

# The project aquarium is the optimized version and aquarium-direct-map is the direct map version.
build aquarium by vs

# build on Linux or macOS
gn gen out/Release --args="is_debug=false"
ninja -C out/Release aquarium
ninja -C out/Release aquarium-direct-map

# build on macOS by xcode
# To build release version, --args="is_debug=false"
gn gen out/build --ide=xcode
# The project aquarium is the optimized version and aquarium-direct-map is the direct map version.
build aquarium by xcode
```

## Build Angle version
TODO: Replace cmake build by gn

First build angle by gn, then link libs to Aquarium and build by cmake.
```sh
# cd the repo
cd aquarium-optimized

# get submodules
git submodule init && git submodule update

#build angle
cd thirdparty/angle
gclient sync
gn gen out/Debug --args="is_debug=true is_clang=false"
ninja -C out/Debug libEGL libGLESv2
gn gen out/Release --args="is_debug=false is_clang=false"
ninja -C out/Release libEGL libGLESv2
```
#build aquarium
```sh
# make build directory
mkdir build && cd build

# build on Windows
cmake -G "Visual Studio 15 2017 Win64" .. -Dangle=true -Ddawn=false
open build/Aquarium.sln using visual studio, set Aquarium as StartUp project and build
copy libEGL.dll libGLESv2.dll to folder build

# build on Linux or macOS
cmake .. -Dangle=true -Ddawn=false
make
```

# Run
```sh
# "--num-fish": specifies how many fishes will be rendered
# "--backend" : specifies running a certain backend, 'opengl', 'dawn_d3d12', 'dawn_vulkan', 'dawn_metal', 'dawn_opengl'
# running angle dynamic backend is on todo list. Currently go through angle path by option 'opengl' if angle is linked into the project
# MSAA is disabled by default. To Enable MSAA of OpenGL backend, "--enable-msaa", 4 samples.
# MSAA of angle or dawn backend is not supported now.

# run on Windows
run it in Visual Studio
or
aquarium.exe --num-fish 10000 --backend opengl --enable-msaa
aquarium.exe --num-fish 10000 --backend dawn_d3d12

# run on Linux
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_vulkan

# run on macOS
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_metal
```
