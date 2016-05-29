#include <iostream>
#include "grid.h"
#include <signal.h>

bool gameon = true;

void sighdl(int sn) {
  gameon = false;
  // send return;
}

int main() {
  bool b;
  grid g(4);
  g.born();
  g.born();
  g.draw();
  char c;
  signal(SIGINT, sighdl);
  while(gameon) {
    std::cin.get(c);
    switch(c) {
      case 'h': b = g.left(); break;
      case 'j': b = g.down(); break;
      case 'k': b = g.up(); break;
      case 'l': b = g.right(); break;
      case 'q': gameon = false; break;
      default: continue;
    }
    if(b) g.born();
    g.draw();
    if(g.dead()) {
      std::cout << "you died" << std::endl;
      break;
    }
  }
  g.gg();
  return 0;
}
