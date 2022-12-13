mingw:
	g++ src/cpu.cpp src/main.cpp -o chip8.exe -I SFML-2.5.1/include -lsfml-graphics -lsfml-window -lsfml-system