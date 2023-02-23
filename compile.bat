g++ -mwindows -std=c++17 main.cpp icon.o %SDL2C++% -I%SDL2IMGI% -L%SDL2IMGL% -lSDL2_image -L. -limgui -lSDL2_imgui -I%DEARIMGUI% -I%DEARIMGUII% -I%DEARIMGUIMISC% -o kelli.exe
kelli.exe