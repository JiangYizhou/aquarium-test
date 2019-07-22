#  Native Aquarium
Native Aquarium is a native implementation of [WebGL Aquarium](https://github.com/WebGLSamples/WebGLSamples.github.io). The goal of this project is to compare the performance of WebGL version to its native counterparts, such as OpenGL, D3D, Vulkan, Metal, ANGLE and Dawn.

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
    <td>ANGLE</td>
    <td>Y</td>
  </tr>
  <tr align=left>
    <td>Linux</td>
    <td>Dawn</td>
    <td>N</td>
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

# Required Tools
Configure [depot_tools](http://dev.chromium.org/developers/how-tos/install-depot-tools) on all platforms.
Windows sdk version is required to be over [10.0.17134.0](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk) to support D3D12 backend.
The OpenGL version is required to > 4.5 on Windows and Linux  and > 4.1 on MacOS.

## Build Aquarium by GN (OpenGL, D3D12, DAWN)

Native Aquarium uses gn to build on Linux, macOS and Windows.

```sh
# Aquarium contains two projects, aquarium-direct-map is a direct map version to WebGL repo, and only has OpenGL backend.  The aquarium
# project contains many backends.

# cd the repo
cd aquarium-test

# Download thirdparty
gclient sync

# Build on aquarium by ninja on Windows, Linux and MacOS.
# On windows, opengl, d3d12 and dawn backends are enabled by default.
# On linux and macOS, opengl and dawn are enabled by default.
# Enable or disable a specific platform, you can add 'enable_opengl', 'enable_d3d12', and 'enable_dawn' to gn args.
# To build a release version, specify 'is_debug=false'.
gn gen out/Release --args="is_debug=false"
ninja -C out/Release aquarium
ninja -C out/Release aquarium-direct-map

# Build on Windows by vs
gn gen out/build --ide=vs
open out/build/all.sln using visual studio.
build aquarium by vs

# Build on macOS by xcode
gn gen out/build --ide=xcode
build aquarium by xcode
```

## Build standalone ANGLE version

Because ANGLE headers have conflicts with other backends, it can only build individually. To build ANGLE version on Windows， please refer to the following steps (ANGLE backend is only supported on Windows now).
```sh
# cd the repo
cd aquarium-test

# download thirdparty
gclient sync

#Build aquarium ninja
gn gen out/Debug --args="enable_angle=true"
ninja -C out/aquarium
gn gen out/Release --args="enable_angle=true"
ninja -C out/Release aquarium

# Build on Windows by vs
gn gen out/Debug --ide=vs --args="enable_angle=true"
open out/Debug/all.sln using visual studio.
build aquarium by vs
```

# Run
```sh
# "--num-fish" : specifies how many fishes will be rendered
# "--backend" : specifies running a certain backend, 'opengl', 'dawn_d3d12', 'dawn_vulkan', 'dawn_metal', 'dawn_opengl', 'angle'
# "--enable-full-screen-mode" : specifies rendering a full screen mode
# Running angle dynamic backend is on todo list.

# run on Windows
aquarium.exe --num-fish 10000 --backend dawn_d3d12
aquarium.exe --num-fish 10000 --backend dawn_vulkan
aquarium.exe --num-fish 10000 --backend angle

# run on Linux
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_vulkan

# run on macOS
./aquarium  --num-fish 10000 --backend opengl
./aquarium.exe --num-fish 10000 --backend dawn_metal

# "--enable-instanced-draws" : specifies rendering fishes by instanced draw. By default fishes
# are rendered by individual draw. Instanced rendering is only supported on dawn and d3d12 backend now.

aquarium.exe --num-fish 10000 --backend dawn_d3d12 --enable-instanced-draws
aquarium.exe --num-fish 10000 --backend dawn_vulkan --enable-instanced-draws
aquarium.exe --num-fish 10000 --backend d3d12 --enable-instanced-draws

# MSAA is disabled by default. To Enable MSAA, "--enable-msaa", 4 samples.
# MSAA of ANGLE is not supported now.

aquarium.exe --num-fish 10000 --backend opengl --enable-msaa


# “--disable-dynamic-buffer-offset” ：The path is to test individual draw by creating many binding groups on dawn backend.
# By default, dynamic buffer offset is enabled. This arg is only supported on dawn backend.

aquarium.exe --num-fish 10000 --backend dawn_d3d12 --disable-dynamic-buffer-offset
aquarium.exe --num-fish 10000 --backend dawn_vulkan --disable-dynamic-buffer-offset

# "--integrated-gpu", "--discrete-gpu": Specifies which gpu to render the application. The two args are exclusive.
# This is an optional arg. By default, a default adapter will be created.
# The option is only supported on dawn and d3d12 backend.
aquarium.exe --num-fish 10000 --backend dawn_d3d12 --integrated-gpu
aquarium.exe --num-fish 10000 --backend dawn_vulkan --discrete-gpu

# aquarium-direct-map only has OpenGL backend
# Enable MSAA
./aquarium-direct-map  --num-fish 10000 --backend opengl --enable-msaa

```

# TODOList
1. Dawn Vulkan backend doesn't work now. We need to implement recreate swap chain in Dawn.
2. Debug mode of Dawn Metal backend has some issues to be fixed.
3. Extra semicolon in dawn/third_party/shaderc throws warning when build the project. This should be fixed in Dawn.
4. Texture without VK_IMAGE_USAGE_TRANSFER_DST_BIT should not be cleared. This is a warning of Dawn Vulkan backend.
5. Enable dynamic choosing backend for ANGLE backend.
6. Enable MSAA mode for ANGLE backend.
7. Support Imgui for Direct Map Aquarium.
8. Check and create the highest OpenGL version supported on each of the platforms.
9. Check if some feature is available for D3D12 backend.
