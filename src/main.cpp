/*Nathan L. 2020
This file is part of C8-emu

C8-emuis free software: you can redistribute it and/or modify
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

#include <iostream>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include "cpu.h"

using namespace std;

int main(int argc, char *argv[]) {
  sf::RenderWindow window(sf::VideoMode(640, 320),"CHIP-8");
  sf::Image image;
  sf::Texture texture;
  sf::Sprite sprite;
  if(!argv[1]){
    return -1;
  }
  CPU cpu;
  cpu.init(argv[1]);
  while(window.isOpen()){
    sf::Event event;
    sf::Uint8 pixels[8192];
    std::vector<int> pixelvector;
    while (window.pollEvent(event)){
      if (event.type == sf::Event::Closed){
         window.close();
      }
      if (event.type == sf::Event::EventType::KeyPressed){
        cpu.keyenable = true;
        std::cout << "key pressed" << std::endl;
        switch(event.key.code){
          case sf::Keyboard::Num1: cpu.keycode = 0x1;
          case sf::Keyboard::Num2: cpu.keycode = 0x2;
          case sf::Keyboard::Num3: cpu.keycode = 0x3;
          case sf::Keyboard::Num4: cpu.keycode = 0xC;
          case sf::Keyboard::Q: cpu.keycode = 0x4;
          case sf::Keyboard::W: cpu.keycode = 0x5;
          case sf::Keyboard::E: cpu.keycode = 0x6;
          case sf::Keyboard::R: cpu.keycode = 0xD;
          case sf::Keyboard::A: cpu.keycode = 0x7;
          case sf::Keyboard::S: cpu.keycode = 0x8;
          case sf::Keyboard::D: cpu.keycode = 0x9;
          case sf::Keyboard::F: cpu.keycode = 0xE;
          case sf::Keyboard::Z: cpu.keycode = 0xA;
          case sf::Keyboard::X: cpu.keycode = 0x0;
          case sf::Keyboard::C: cpu.keycode = 0xB;
          case sf::Keyboard::V: cpu.keycode = 0xF;
        }
      } 
    }

    sf::sleep(sf::milliseconds(60/60));

    if(cpu.delaytimer != 0){
      cpu.delaytimer = cpu.delaytimer - 1;
    }
    else{
      cpu.delaytimer = 60;
    }
    cpu.cycle();

    for(int y = 0; y < 32; y++){
      for(int x = 0; x < 64; x++){
        if(cpu.display[x][y] == 1){
          pixelvector.push_back(0);
          pixelvector.push_back(255);
          pixelvector.push_back(0);
          pixelvector.push_back(255);
        }
        else{
          pixelvector.push_back(0);
          pixelvector.push_back(0);
          pixelvector.push_back(0);
          pixelvector.push_back(255);
        }
      }
    }
    cpu.keyenable = false;
    cpu.keycode = 0;
    std::copy(pixelvector.begin(), pixelvector.end(), pixels);
    image.create(64, 32, pixels);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    sprite.setScale(10,10);
    window.draw(sprite);
    window.display();
  }
  return 0;
}