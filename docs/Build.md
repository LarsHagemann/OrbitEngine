# How to build the Orbit Game Engine
This Documentation file will guide you through building Orbit.

## Dependencies
Orbit depends on the following libraries to work:
- [NVIDIA PhysX (Version 4.1)](https://github.com/NVIDIAGameWorks/PhysX)
- [ImGUI (Version 1.84)](https://github.com/ocornut/imgui)
- [Eigen (Version 3.91)](https://eigen.tuxfamily.org/index.php?title=Main_Page)
- [Zlib](https://zlib.net/)

Depending on the renderer you are targetting, Orbit also depends on
- OpenGL
- DirectX (11 or 12)

# Building PhysX
NVIDIA has good [documentation](https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BuildingWithPhysX.html) on building PhysX online. In your build preset (in `physx/buildtools/presets/public` select your preset. I.e. for x64 and VS 2019 you want to go to `vc16win64.xml`) and change the setting `NV_USE_STATIC_WINCRT` to `TRUE`.

### PhysX Options
If you don't need the snippets or the samples, just the libraries, then you can also disable building snippets and samples via the options `PX_BUILDSNIPPETS=FALSE` and `PX_BUILDPUBLICSAMPLES=FALSE`.
This will decrease your buildtime.

## Building
Now that we've chosen all the necessary settings, we can generate our project files. Go to `phsyx/` and run either `generate_projects.bat` (or `-.sh`). The tool will ask you for your generator. Select the one, that you have changed the settings for in the first step. The project files will be written to `physx/compiler/<generator_name>/`. Now build the debug and release version with the compiler of your choice. 

# ImGUI
For ImGUI you don't need to do anything specific. Just clone or download the github repository. 

# Eigen
Eigen doesn't need to be build either. Just clone or download the necessary files and you are good to go.

# Zlib
Same story as with ImGUI and Eigen. Just clone the [repository](https://github.com/madler/zlib) or download the code from the zlib website. 

# Building Orbit
Now that all the dependencies are dealt with, we can focus on building Orbit. For that, I like to create a new folder that I call `build` in my Orbit repository. Open a terminal and cd into that folder and run `cmake ..`. This call will fail (hopefully) and ask you set some cmake variables:
- `ORBIT_RENDERER=DX11|DX12|OpenGL`
- `EIGEN_ROOT_PATH=<Path_To_Eigen>`
- `PHYSX_ROOT_PATH=<Path_To_PhysX>`
- `PHYSX_LIBRARY_PATH=<Path_To_PhysX_Lib>`
- `IMGUI_ROOT_PATH=<Path_To_ImGUI>`
- `ZLIB_ROOT_PATH=<Path_To_Zlib>`
- `ORBIT_OS=Windows|Unix`
Additionally you can set the option `BUILD_SAMPLES` to `FALSE` if you don't want to build the samples.
Some Notes on the paths:
- `<Path_To_ImGUI>`: Points to the folder containing the imgui.h, imgui.cpp, ... with the `backends/` folder in it.
- `<Path_To_Eigen>`: Points to a folder containing the `Eigen` folder with all the source code.
- `<Path_To_PhysX>`: Points to the folder with the `physx` and `pxshared` folder in it.
- `<Path_To_PhysX_Lib>`: Points to the folder with the `debug` and `release` folder in it that you have build.
- `<Path_To_Zlib>`: Points to the folder with the Zlib source code in it.

The final call could then look like
```
cmake ..
  -DORBIT_RENDERER=DX11 
  -DORBIT_OS=Windows 
  -DEIGEN_ROOT_PATH=src/to/Eigen/
  -DPHYSX_ROOT_PATH=src/to/physx/
  -DPHYSX_LIBRARY_PATH=src/to/lib/physx/
  -DIMGUI_ROOT_PATH=src/to/ImGUI/
  -DZLIB_ROOT_PATH=src/to/zlib/
```
I hope, this guide helped you build Orbit.