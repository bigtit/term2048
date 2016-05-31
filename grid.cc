#include <fstream>
#include <iostream>
#include "grid.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include <climits>
using std::cout;

grid::grid(int s=4): size(s), sc(0), bst(-1) {
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
  return (a+b)*2-(a>b?a-b:b-a);
}
int empty(const line& data) {
  int a = 0;
  for(auto d : data) {
    if(1==d) ++a;
  }
  return a<<1;
}
int eval(const line& d, int s) {
   return sono(d,s)+empty(d);
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
int grid::minimax(int depth, int min, int max, bool t) {
  if(!depth || dead()) return eval(data, size);
  // save data;
  auto d = data;
  auto c = sc;
  // human, min player
  if(t) {
    int v1 = max;
    for(int i=0; i<4; ++i) {
      if(moveme(i)) {
        int v2 = minimax(depth-1, min, v1, false);
        data = d;
        sc = c;
        if(v2<v1) {
          v1 = v2;
          bst = i;
        }
        if(v1<min){
          return min;
        }
      }
    }
    return v1;
  }
  else { // max player
    int v1 = min;
    for(int i=0; i<size*size; ++i){
      if(1<data[i]) continue;
      data[i] = 2<<(randme()%2);
      int v2 = minimax(depth-1, v1, max, true);
      data = d;
      sc = c;
      if(v2>v1) v1 = v2;
      if(v1>max) return max;
    }
    return v1;
  }
}

int grid::autorun() {
  int c = 0;
  while(!dead()) {
    // minimax for current board
    minimax(6, 0, INT_MAX, true);
    cout << bst << '\n';
    moveme(bst);
    born();
    draw();
    ++c;
  }
  return c;
}
