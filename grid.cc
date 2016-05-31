#include <fstream>
#include <iostream>
#include "grid.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include <climits>
using std::cout;

grid::grid(int s=4): size(s), sc(0) {
  if(!load()) {
    for(int i=0; i<size*size; ++i) data.push_back(1);
  }
}
grid::~grid() {}

bool grid::dead() {
  for(auto d : data) {
    if(1==d) return false;
    else if(2048==d) return true;
  }
  // full
  for(int i=0; i<size; ++i) {
    for(int j=0; j<size-1; ++j) {
      if(data[size*i+j]==data[size*i+j+1]) return false;
      if(data[size*j+i]==data[size*j+i+size]) return false;
    }
  }
  return true;
}

bool grid::left() {
  auto t = data;
  for(int i=0; i<size; ++i) {
    int tmp = 1, idx = 0;
    for(int j=0; j<size; ++j) {
      if(1<data[size*i+j]) {
        if(tmp==data[size*i+j]) {
          data[size*i+idx++] = tmp<<1;
          sc += tmp;
          tmp = 1;
        }
        else if(1==tmp) tmp = data[size*i+j];
        else {
          data[size*i+idx++] = tmp;
          tmp = data[size*i+j];
        }
      }
    }
    data[size*i+idx++] = tmp;
    for(auto k=idx; k<size; ++k) data[size*i+k] = 1;
  }
  return t!=data;
}

bool grid::right() {
  R();
  bool b = left();
  R();
  return b;
}
bool grid::up() {
  T();
  bool b = left();
  T();
  return b;
}
bool grid::down() {
  T();
  bool b = right();
  T();
  return b;
}

unsigned int randme() {
  int fd = open("/dev/urandom", O_RDONLY);
  unsigned int buf = 0;
  read(fd, &buf, 4);
  close(fd);
  return buf;
}

void grid::born() {
  int c = 0;
  int r2 = 2<<(randme()%2);
  for(auto d : data) {
    if(1==d) ++c;
  }
  if(!c) return;
  int r = randme()%c;
  c = 0;
  for(int i=0; i<data.size(); ++i) {
    if(1==data[i] && c++==r) {
      data[i] = r2;
      break;
    }
  }
}

void grid::draw() {
  cout << "Current status with score (" << sc << "):" << std::endl;
  for(int i=0; i<size; ++i) {
    for(int j=0; j<size; ++j) {
      auto c = data[size*i+j];
      cout << (c>1?c:0) << "\t";
    }
    cout << std::endl;
  }
  cout << std::endl;
}

void grid::T() {
  for(int i=0; i<size; ++i) {
    for(int j=1+i*(size+1); j<(i+1)*size; ++j)
      std::swap(data[j], data[size*(j%size)+i]);
  }
}
void grid::R() {
  for(int i=0; i<size; ++i) {
    for(int j=size*i; j<size*i+size/2; ++j)
      std::swap(data[j], data[size-1+2*size*i-j]);
  }
}
void grid::sav() {
  std::ofstream s;
  s.open("gridgame.sav", std::ofstream::binary);
  if(s.is_open()) {
    s.write((char*)&size, sizeof(size));
    s.write((char*)&sc, sizeof(sc));
    for(auto d : data)
      s.write((char*)&d, sizeof(d));
    s.close();
  }
  else cout << "unable to save progress";
}
void grid::gg() {
  draw();
  sav();
  cout << "your score: " << sc << std::endl;
}
bool grid::load() {
  int buf;
  std::ifstream s;
  s.open("gridgame.sav", std::ofstream::binary);
  if(s.is_open()) {
    if(s.peek()==EOF) return false;
    s.read((char*)&buf, sizeof(buf));
    size = buf;
    s.read((char*)&buf, sizeof(buf));
    sc = buf;
    for(int i=0; i<size*size; ++i) {
      s.read((char*)&buf, sizeof(buf));
      data.push_back(buf);
    }
    return true;
  }
  return false;
}

int sono(const line& d, int s) {
  int a = 0, b = 0;
  for(int i=0; i<s-1; ++i) {
    for(int j=0; j<s-1; ++j) {
      a += d[s*i+j]/d[s*i+j+1];
      a += d[s*i+j]/d[s*i+j+s];
      b += d[s*i+j+1]/d[s*i+j];
      b += d[s*i+j+s]/d[s*i+j];
    }
  }
  int c = (a+b)-(a>b?a-b:b-a);
  return c;
}
int empty(const line& d, int s) {
  int a = 0;
  for(int i=0; i<s*s; ++i) if(1==d[i]) ++a;
  return a;
}
int eval(const line& d, int s) {
  auto a = sono(d, s);
  auto b = empty(d, s);
  return a<<2+b<<4;
}

bool grid::moveme(int m) {
  switch(m) {
  case 0: return left();
  case 1: return right();
  case 2: return up();
  case 3: return down();
  default: return false;
  }
}

// output: evaluation, best path
int grid::minimax(int depth) {
  int choice = -1, best = 0;
  auto d = data;
  auto c = sc;
  for(int i=0; i<4; ++i) {
    if(!moveme(i)) continue;
    int tmp = max(depth-1, INT_MIN, INT_MAX);
    data = d; sc = c;
    if(tmp>best) {
      best = tmp;
      choice = i;
    }
  }
  return choice;
}

int grid::min(int depth, int alpha, int beta) {
  if(!depth || dead() || alpha>=beta) return eval(data, size);
  int v = beta;
  auto d = data;
  auto c = sc;
  for(int i=0; i<4; ++i) {
    if(!moveme(i)) continue;
    int v2 = max(depth-1, alpha, v);
    data = d; sc = c;
    if(v2<v) v = v2;
    if(v<alpha) return alpha;
  }
  return v;
}
int grid::max(int depth, int alpha, int beta) {
  if(!depth || dead() || alpha>=beta) return eval(data, size);
  int v = alpha;
  for(int i=0; i<size*size; ++i) {
    if(1<data[i]) continue;
    data[i] = 2<<(randme()%2);
    int v2 = min(depth-1, v, beta);
    data[i] = 1;
    if(v2>v) v = v2;
    if(v>beta) return beta;
  }
  return v;
}

int grid::autorun() {
  int c = 0;
  int dir;
  while(!dead()) {
    if(-1==(dir=minimax(9-empty(data, size)/2))) break;
    cout << dir << '\n';
    moveme(dir);
    born();
    draw();
    ++c;
  }
  return c;
}
