#include <vector>
using line = std::vector<int>;

class grid {
  int size;
  line data;
  int sc;
  void T(); // transpose
  void R(); // reverse
  void sav();
  bool load();
  int max(int, int, int);
  int min(int, int, int);
public:
  grid(int s);
  ~grid();
  bool left();
  bool right();
  bool up();
  bool down();
  bool moveme(int);
  void born();
  void draw();
  bool dead();
  void gg();

  int minimax(int);
  int autorun();
};

