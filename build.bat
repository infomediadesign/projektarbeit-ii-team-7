cmake -B build -S src
cmake --build build
rmdir /Q /S dist
mkdir dist
mkdir dist\assets
mkdir dist\levels
copy /y build\engine\Debug\miniflow.exe dist
xcopy /Y assets dist\assets
xcopy /Y levels dist\levels
