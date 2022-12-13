#include <stack>
#include <fstream>

class CPU {
  public:
    unsigned char memory[4096];
    bool display[64][32];
    unsigned int pc;
    uint16_t ireg;
    uint16_t opcode;
    uint16_t instruction;
    unsigned char x;
    unsigned char y;
    unsigned char n;
    unsigned char nn;
    uint16_t nnn;
    unsigned char delaytimer;
    unsigned char soundtimer;
    unsigned char registers[16];
    char keycode;
    bool keyenable;
    std::stack<uint16_t> chipstack;
    void init(std::string path);
    void cycle();
};
