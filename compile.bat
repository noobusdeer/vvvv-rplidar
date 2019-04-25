cmake -G "Visual Studio 14 2015 Win64" -Blib-rplidar\build -Hlib-rplidar
MSbuild lib-rplidar\build\lib-rplidar.sln /m /p:PlatformToolset=v140 /p:Configuration=Debug;Platform=x64
XCOPY lib-rplidar\build\Debug\lib-rplidar.dll out /m /e /y
MSbuild vvvv-rplidar\vvvv-rplidar.sln /m /p:Configuration=Debug;Platform="Any CPU"
XCOPY vvvv-rplidar\build\vvvv-rplidar.dll out /m /e /y