mac:
	g++ src/main.cpp src/cpu.cpp -o chip8 -std=c++11 -lsfml-graphics  -lsfml-window -lsfml-system
mingw:
	g++ src/cpu.cpp src/main.cpp -o chip8.exe -I SFML-2.5.1/include -lsfml-graphics -lsfml-window -lsfml-system
