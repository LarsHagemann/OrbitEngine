# Orbit Game Engine
using DirectX 12

## TODO:
- Finish translating the old DirectX 11 Orbit engine to DirectX 12
- Write some sample code
- Write a wiki
- Rewrite the README 
- Publish

## Installation:
Preperations:
Orbit depends on Eigen for vector algebra. Luckily, Eigen is a header-only library, so everything you have
to do is, copy the Eigen header files into a /dep/ directory in the same directory as src and inc. 
Your /dep/ directory should look like this now:
dep/
	Eigen/
		<Eigen Source Code>

For building the Orbit Game Engine you need cmake (at least version 3.12.4).
When using the cmake GUI copy source_directory into the text box labeled "Where the source code is"
and build_directory into the text box labeled "Where to build the binaries".
Click on "Configure" and select the compiler and the compiler options that you want to use
(for example Visual Studio 16 2019 and Win32). When no errors occured click on generate and you're done
generating the project. 
Go into your build directory open the project and compile it (most likely you want the debug and the
release builds). If everything has worked, you should find your libraries in the /Debug/ or /Release/ folder.