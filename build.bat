cmake -DCMAKE_BUILD_TYPE=Debug -B build -S src
cmake --build build --config Debug
rmdir /Q /S dist
mkdir dist
mkdir dist\assets
mkdir dist\levels
copy /y build\engine\Debug\miniflow.exe dist
xcopy /Y /S /E assets dist\assets
xcopy /Y /S /E levels dist\levels
