cmake -DCMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build --config Release -j 16
rmdir /Q /S dist
mkdir dist
mkdir dist\assets
mkdir dist\levels
copy /y build\engine\Release\miniflow.exe dist
xcopy /Y /S /E assets dist\assets
xcopy /Y /S /E levels dist\levels
