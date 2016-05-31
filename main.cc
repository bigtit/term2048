#include <iostream>
#include "grid.h"

int main() {
  grid g(4);
  g.born();
  g.born();
  g.draw();
  int c = g.autorun();
  g.gg();
  std::cout << c << " steps\n";
  return 0;
}
