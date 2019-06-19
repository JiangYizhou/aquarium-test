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
    <td>Windows</td>
    <td>D3D12</td>
    <td>Y</td>
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
# To build release version, --args="is_debug=false"
# Enable different backend, by default dawn and d3d12 backend is enabled.
# To change other backends, --args="enable_d3d12=false enable_dawn=false enable_angle=true", if angle backend is enabled, the opengl
# backend will go through angle.

# cd the repo
cd aquarium-test

# download thirdparty
gclient sync

# Build on Windows by vs
gn gen out/build --ide=vs
open out/build/all.sln using visual studio.

# The project aquarium is the optimized version and aquarium-direct-map is the direct map version.
build aquarium by vs

# build on Linux, macOS and windows by ninja, disable d3d12 backend on linux and mac
gn gen out/Release --args="is_debug=false enable_d3d12=false"
ninja -C out/Release aquarium
ninja -C out/Release aquarium-direct-map

# build on macOS by xcode
# To build release version, --args="is_debug=false"
gn gen out/build --ide=xcode
# The project aquarium is the optimized version and aquarium-direct-map is the direct map version.
build aquarium by xcode
```

## Build Angle version
```sh

#build angle
cd thirdparty/angle
gclient sync
gn gen out/Debug --args="is_debug=true enable_angle=true angle_enable_vulkan=false"
ninja -C out/Debug libEGL libGLESv2
gn gen out/Release --args="is_debug=false enable_angle=true angle_enable_vulkan=false"
ninja -C out/Release libEGL libGLESv2
```
#build aquarium
```sh
# Build on Windows by vs
gn gen out/Debug --ide=vs --args="enable_angle=true enable_dawn=false enable_d3d12=false"
open out/Debug/all.sln using visual studio.

# build on Windows
build aquarium by vs
```

# Run
```sh
# "--num-fish" : specifies how many fishes will be rendered
# "--backend" : specifies running a certain backend, 'opengl', 'dawn_d3d12', 'dawn_vulkan', 'dawn_metal', 'dawn_opengl'
# "--allow-instanced-draws" : specifies rendering fishes by instanced draw. By default fishes
# are rendered by individual draw. Instanced rendering is only supported on dawn backend now.
# running angle dynamic backend is on todo list. Currently go through angle path by option 'opengl' if angle is linked into the project
# MSAA is disabled by default. To Enable MSAA of OpenGL backend, "--enable-msaa", 4 samples.
# MSAA of angle is not supported now.
# “--disable-dynamic-buffer-offset” ：The path is to test individual draw by creating many binding groups on dawn backend.
# By default, dynamic buffer offset is enabled. The arg is only supported on dawn backend.

# run on Windows
run it in Visual Studio
or
aquarium.exe --num-fish 10000 --backend opengl --enable-msaa
aquarium.exe --num-fish 10000 --backend dawn_d3d12
aquarium.exe --num-fish 10000 --backend angle

# run on Linux
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_vulkan

# run on macOS
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_metal
```
