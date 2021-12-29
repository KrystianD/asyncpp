#pragma once

static int moves = 0;
static int copies = 0;

class Obj {
 public:
  int v;

  Obj() {}
  Obj(int v) : v(v) {}

  Obj(const Obj& o) {
    v = o.v;
    copies++;
  }
  Obj& operator=(const Obj& o) {
    copies++;
    return *this;
  }

  Obj(Obj&& o) {
    v = o.v;
    moves++;
  }
  Obj& operator=(Obj&& o) {
    v = o.v;
    moves++;
    return *this;
  }
};
