#include <fstream>
#include <iostream>
#include "grid.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

grid::grid(int s=4): size(s), sc(0) {
  if(!load()) {
    for(int i=0; i<size*size; ++i) data.push_back(0);
  }
}
grid::~grid() {}

bool grid::dead() {
  for(auto d : data) {
    if(!d) return false;
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
    int tmp = 0, idx = 0;
    for(int j=0; j<size; ++j) {
      if(data[size*i+j]) {
        if(tmp==data[size*i+j]) {
          data[size*i+idx++] = tmp<<1;
          sc += tmp;
          tmp = 0;
        }
        else if(!tmp) tmp = data[size*i+j];
        else {
          data[size*i+idx++] = tmp;
          tmp = data[size*i+j];
        }
      }
    }
    data[size*i+idx++] = tmp;
    for(auto k=idx; k<size; ++k) data[size*i+k] = 0;
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

void grid::born() {
  int fd = open("/dev/urandom", O_RDONLY);
  unsigned int buf = 0, c = 0;
  read(fd, &buf, 4);
  int r2 = 2<<(buf%2);
  read(fd, &buf, 4);
  close(fd);
  for(auto d : data) {
    if(!d) ++c;
  }
  if(!c) return;
  int r = buf%c;
  c = 0;
  for(int i=0; i<data.size(); ++i) {
    if(!data[i] && c++==r) {
      data[i] = r2;
      break;
    }
  }
}

void grid::draw() {
  std::cout << "Current status with score (" << sc << "):" << std::endl;
  for(int i=0; i<size; ++i) {
    for(int j=0; j<size; ++j) {
      std::cout << data[size*i+j] << "\t";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
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
  else std::cout << "unable to save progress";
}
void grid::gg() {
  sav();
  std::cout << "your score: " << sc << std::endl;
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
