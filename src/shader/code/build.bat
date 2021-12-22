@echo off

echo Beginning compilation. > log.txt

echo Building DEBUG --VERTEX_SHADER
fxc /E vs_default /T vs_5_1 /Od /Zi /Fo ../vs_default_d.fxc default.hlsl >> log.txt 2>&1
if errorlevel 1 goto vs_debug_failed
:after_vertex_debug
echo Building DEBUG --PIXEL_SHADER
fxc /E ps_default /T ps_5_1 /Od /Zi /Fo ../ps_default_d.fxc default.hlsl >> log.txt 2>&1
if errorlevel 1 goto ps_debug_failed
:after_pixel_debug

echo Building RELEASE --VERTEX_SHADER
fxc /E vs_default /T vs_5_1 /O3 /Fo ../vs_default.fxc default.hlsl >> log.txt 2>&1
if errorlevel 1 goto vs_release_failed
:after_vertex_release
echo Building RELEASE --PIXEL_SHADER
fxc /E ps_default /T ps_5_1 /O3 /Fo ../ps_default.fxc default.hlsl >> log.txt 2>&1
if errorlevel 1 goto ps_release_failed
:after_pixel_release
echo Building root signature object
fxc /T rootsig_1_1 rs.hlsli /E OrbitDefaultRS /Fo ../rs_default.fxo >> log.txt 2>&1
if errorlevel 1 echo Failed to compile Root Signature. See log.txt for more information.
exit /B

:vs_debug_failed
echo Failed to compile DEBUG --VERTEX_SHADER. See log.txt for more information.
goto after_vertex_debug
:ps_debug_failed
echo Failed to compile DEBUG --PIXEL_SHADER. See log.txt for more information.
goto after_pixel_debug
:vs_release_failed
echo Failed to compile RELEASE --VERTEX_SHADER. See log.txt for more information.
goto after_vertex_release
:ps_release_failed
echo Failed to compile RELEASE --PIXEL_SHADER. See log.txt for more information.
goto after_pixel_release
