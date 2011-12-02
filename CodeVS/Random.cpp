#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <set>

using namespace std;
typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;
static const double EPS = 1e-9;
static const double PI = acos(-1.0);

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define FOR(i, s, n) for (int i = (s); i < (int)(n); i++)
#define FOREQ(i, s, n) for (int i = (s); i <= (int)(n); i++)
#define FORIT(it, c) for (auto it= (c).begin(); it != (c).end(); it++)
#define MEMSET(v, h) memset((v), h, sizeof(v))

struct Tower {
  int x;
  int y;
  int level;
  int type;
  Tower() {;}
  Tower(int x, int y, int level, int type) :
    x(x), y(y), level(level), type(type) {;}
};

struct Enemy {
  int x;
  int y;
  int t;
  int life;
  int speed;
  Enemy() {;}
  Enemy(int x, int y, int t, int life, int speed) :
    x(x), y(y), t(t), life(life), speed(speed) {;}
};

int stage;
int field[100][100];
int l;
int w, h;
char str[1000];

int main() {
  srand(123456789);
  scanf("%d", &stage);
  REP(i, stage) {
    scanf("%d %d", &w, &h);
    REP(y, h) {
      REP(x, w) {
        char c;
        scanf(" %c ", &c);
        field[y][x] = c;
      }
    }
    scanf("%d", &l);
    scanf("%s", str);   //END
    assert(str[0] == 'E' && str[1] == 'N' && str[2] == 'D');
    REP(j, l) {
      int life, money, T, E;
      scanf("%d %d %d %d", &life, &money, &T, &E);
      vector<Tower> tower;
      vector<Enemy> enemy;
      REP(k, T) {
        int x, y, a, c;
        scanf("%d %d %d %d", &x, &y, &a, &c);
        tower.push_back(Tower(x, y, a, c));
        field[y][x] = 10000 + k;
      }
      REP(k, E) {
        int x, y, t, l, s;
        scanf("%d %d %d %d %d", &x, &y, &t, &l, &s);
        enemy.push_back(Enemy(x, y, t, l, s));
        field[y][x] = 20000 + k;
      }
      scanf("%s", str);   //END
      assert(str[0] == 'E' && str[1] == 'N' && str[2] == 'D');
      int r = 3;
      vector<Tower> output;
      if (tower.empty()) {
        output.push_back(Tower(2, 2, 0, 0));
        output.push_back(Tower(2, 3, 0, 0));
        output.push_back(Tower(2, 4, 0, 0));
        output.push_back(Tower(2, 5, 0, 0));
        output.push_back(Tower(4, 2, 0, 0));
        output.push_back(Tower(4, 3, 0, 0));
        output.push_back(Tower(4, 4, 0, 0));
        output.push_back(Tower(5, 2, 0, 0));
      } else {
      r = min(3, money / 100);
      int cnt = 0;
        REP(iter, r) {
          cnt++;
          if (cnt >= 1000) { break; }
          int x = rand() % w;
          int y = rand() % h;
          if (field[y][x] == '0') {
            field[y][x] = '1';
            output.push_back(Tower(x, y, 1, 0));
          } else {
            iter--;
            continue;
          }
        }
      }
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      if (output.size() == 0) { puts(""); }
      fflush(stdout);
    }
  }
}
