cmake -DCMAKE_BUILD_TYPE=Debug -B build -S .
cmake --build build --config Debug -j 16
rmdir /Q /S dist
mkdir dist
mkdir dist\assets
mkdir dist\levels
mkdir dist\lua
copy /y build\engine\Debug\miniflow.exe dist
xcopy /Y /S /E assets dist\assets
xcopy /Y /S /E levels dist\levels
xcopy /Y /S /E lua dist\lua
