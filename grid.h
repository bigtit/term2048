#include <vector>

class grid {
  int size;
  std::vector<int> data;
  int max;
  int sc;
  void T(); // transpose
  void R(); // reverse
  void sav();
  bool load();
public:
  grid(int s);
  ~grid();
  bool left();
  bool right();
  bool up();
  bool down();
  void born();
  void draw();
  bool dead();
  void gg();
};
