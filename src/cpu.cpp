/*Nathan L. 2020
This file is part of C8-emu

C8-emu is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

C8-emuis distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with C8-emu. If not, see <https://www.gnu.org/licenses/>.
*/

#include "cpu.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include "../include/ghc/filesystem.hpp"

unsigned char font[] = 
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//Stack
std::stack<short> stack;

#define RAND_MAX 255

void CPU::init(std::string path){
  //Load Font
  pc = 0x200;
  delaytimer = 60;
  for(int i = 0; i <= 0x04B; i++){
    memory[i] = font[i];
  }
  //Load ROM
  std::ifstream file(path, std::ios::in | std::ios::binary);
  namespace fs = ghc::filesystem;
  int file_size = fs::file_size(path);
  char buffer[file_size];
  file.read(buffer,file_size);
  for(int i = 0; i <= file_size; i++){
    memory[0x200+i] = buffer[i];
  }
  //Clear screen
  for(int y = 0; y < 32; y++){
    for(int x = 0; x < 64; x++){
      display[x][y] = 0;
    }
  }

  for(int i = 0; i <= 0xF; i++){
    registers[i] = 0;
  }
}

void CPU::cycle(){
  //std::cout << std::hex << pc << std::endl;
  opcode = 0;
  //Fetch
  opcode = memory[pc] << 8 | memory[pc + 1];
  //std::cout << std::hex << opcode << std::endl;
  //Decode
  instruction = opcode & 0xF000;
  x = (opcode & 0x0F00) >> 8;
  y = (opcode & 0x00F0) >> 4;
  n = opcode & 0x000F;
  nn = opcode & 0x00FF;
  nnn = opcode & 0x0FFF;
  //Execute
  switch(instruction){
    case 0x0000:
      switch(nn){
        case 0xE0:
          for(int y = 0; y < 32; y++){
            for(int x = 0; x < 64; x++){
              display[x][y] = 0;
            }
          }
          pc += 2;
          break;
        case 0xEE:
          pc = stack.top();
          stack.pop();
          break;
      }
      break;
      
    case 0x1000:
      pc = nnn;
      break;
    
    case 0x2000:
      pc += 2;
      stack.push(pc);
      pc += 2;
      pc = nnn;
      break;

    case 0x3000:
      if(registers[x] == nn){
        pc += 4;
      }
      else{
        pc += 2;
      }
      break;

    case 0x4000:
      if(registers[x] != nn){
        pc += 4;
        break;
      }
      else{
        pc += 2;
      }
      break;

    case 0x5000:
      if(registers[x] == registers[y]){
        pc += 4;
        break;
      }
      else{
        pc += 2;
      }
      break;

    case 0x6000:
      registers[x] = nn;
      pc += 2; 
      break;

    case 0x7000:
      registers[0xF] = 0;
      if(registers[x] + nn <= 0x00FF){
        registers[x] += nn;
      }
      else{
        registers[x] += nn;
        registers[0xF] = 1;
      }
      pc += 2;
      break;

    case 0x8000:
      switch(n){
        case 0x0:
          registers[x] = registers[y];
          break;
        
        case 0x1:
          registers[x] = registers[x] | registers[y];
          break;

        case 0x2:
          registers[x] = registers[x] & registers[y];
          break;

        case 0x3:
          registers[x] = registers[x] ^ registers[y];
          break;

        case 0x4:
          if(registers[x] + registers[y] > 255){
            registers[0xF] = 1;
          }
          else{
            registers[0xF] = 0;
          }
          registers[x] = registers[x] + registers[y];
          break;

        case 0x5:
          if(registers[x] > registers[y]){
            registers[0xF] = 1;
          }
          else if(registers[x] < registers[y]){
            registers[0xF] = 0;
          }
          registers[x] = registers[x] - registers[y];
          break;

        case 0x6:
          //registers[x] = registers[y];
          if((registers[x] & 0x01) == 0x01){
            registers[0xF] = 1;
          }
          else{
            registers[0xF] = 0;
          }
          registers[x] = registers[x] >> 1;
          break;

        case 0x7:
          if(registers[y] > registers[x]){
            registers[0xF] = 1;
          }
          else if(registers[y] < registers[x]){
            registers[0xF] = 0;
          }
          registers[x] = registers[y] - registers[x];
          break;

        case 0xE:
          //registers[x] = registers[y];
          if((registers[x] & 0b10000000) == 0b10000000){
            registers[0xF] = 1;
          }
          else{
            registers[0xF] = 0;
          }
          registers[x] = registers[x] << 1;
          break;
      }
      pc += 2;
      break;

    case 0x9000:
      if(registers[x] != registers[y]){
        pc += 4;
      }
      else{
        pc += 2;
      }
      break;

    case 0xA000:
      ireg = nnn;
      pc += 2;
      break;

    case 0xB000:
      pc = nnn + registers[0x0];
      break;

    case 0xC000:
      registers[x] = (rand() & nn);
      pc += 2;
      break;

    case 0xE000:
      switch(nn){
        case 0x9E:
          if(keycode == registers[x]){
            pc += 4;
          }
          else{
            pc += 2;
          }
          break;

        case 0xA1:
          if(keycode != registers[x]){
            pc += 4;
          }
          else{
            pc += 2;
          }
          break;
      }
      break;

    case 0xF000:
      switch(nn){
        case 0x0A:
          if(keyenable != true){
            break;
          }
          else{
            registers[x] = keycode;
            pc += 2;
          }
          break;

        case 0x07:
          registers[x] = delaytimer;
          break;
        
        case 0x15:
          delaytimer = registers[x];
          break;

        case 0x18:
          soundtimer = registers[x];
          break;

        case 0x1E:
          ireg += registers[x];
          break;

        case 0x29:
          ireg = registers[x] * 5;
          break;

        case 0x33:
          memory[ireg + 2] = registers[x] % 10;
          memory[ireg + 1] = (registers[x] / 10) % 10;
          memory[ireg] = ((registers[x] / 10) / 10);
          break;

        case 0x55:
          for(int i = 0; i <= x; i++){
            memory[ireg + i] = registers[i];
          }
          break;

        case 0x65:
          for(int i =0; i <= x; i++){
            registers[i] = memory[ireg + i];
          }
          break;
      }
      if(nn == 0x0A){
        break;
      }
      else{
        pc += 2;
      }
      break;

    case 0xD000:
      //std::cout << std::hex << int(registers[y]) << "\n";
      unsigned char xcord = registers[x] & 63;
      unsigned char ycord = registers[y] & 31;
      //std::cout << std::hex << int(xcord) << " " << std::hex << int(ycord) << "\n";
      registers[0xF] = 0;
      for(int rows = 0; rows < n; rows++){
        std::bitset<8> spriteroworig = memory[ireg+rows];
        auto str = spriteroworig.to_string();
        std::reverse(str.begin(), str.end());
        auto spriterow = std::bitset<8>(str);

        if(ycord+rows > 31){
          break;
        }
        else{
          for(int x = 0; x < 8; x++){
            if(x+xcord > 63){
              break;
            }
            else{
              if(display[x+xcord][ycord+rows] == spriterow[x]){
                registers[0xF] = 1;
              }
              else{
                registers[0xF] = 0;
              }
              display[x+xcord][ycord+rows] ^= spriterow[x];
            }
          }
        }
      }
      pc += 2;
      break;
  }
  if(delaytimer > 0){
    delaytimer--;
  }
}
