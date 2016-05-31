#include <vector>
using line = std::vector<int>;

class grid {
  int size;
  line data;
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
  bool moveme(int);
  void born();
  void draw();
  bool dead();
  void gg();

  int minimax(int, int, int, bool);
  int autorun();
private:
  int bst;
};

