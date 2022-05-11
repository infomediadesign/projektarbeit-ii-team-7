cmake -DCMAKE_BUILD_TYPE=Release -B build -S src
cmake --build build --config Release
rmdir /Q /S dist
mkdir dist
mkdir dist\assets
mkdir dist\levels
copy /y build\engine\Release\miniflow.exe dist
xcopy /Y assets dist\assets
xcopy /Y levels dist\levels
