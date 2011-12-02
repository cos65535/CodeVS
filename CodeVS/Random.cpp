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

int stageCnt;
int field[100][100];
int l;
int w, h;
char str[1000];
bool visit[60][60];
int dist[60][60];
const int dx[4] = { 1, 0, -1, 0 };
const int dy[4] = { 0, 1, 0, -1 };

void PrintField() {
  REP(y, h) {
    REP(x, w) {
      printf("%c", field[y][x]);
    }
    puts("");
  }
  puts("");
}

bool dfs(int x, int y) {
  if (visit[y][x]) { return false; }
  visit[y][x] = true;
  bool ret = field[y][x] == 'g';
  REP(dir, 4) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if (field[ny][nx] != 's' && field[ny][nx] != '0' && field[ny][nx] != 'g') { continue; }
    ret |= dfs(nx, ny);
  }
  return ret;
}

bool OK(int bx, int by) {
  if (field[by][bx] != '0') { return false; }
  MEMSET(visit, false);
  field[by][bx] = 'P';
  REP(y, h) {
    REP(x, w) {
      if (visit[y][x]) { continue; }
      if (field[y][x] == 's') {
        if (!dfs(x, y)) { goto ng; }
      }
    }
  }
  field[by][bx] = '0';
  return true;
ng:;
  field[by][bx] = '0';
  return false;
}

void CalcDist() {
  struct Point {
    int x, y, cost;
    Point() {;}
    Point(int x, int y, int cost) : x(x), y(y), cost(cost) {;}
  };
  queue<Point> que;
  MEMSET(dist, 0x0f);
  MEMSET(visit, false);
  REP(y, h) {
    REP(x, w) {
      if (field[y][x] == 's') {
        que.push(Point(y, x, 0));
        visit[y][x] = true;
      }
    }
  }
  while (!que.empty()) {
    Point p = que.front();
    que.pop();
    dist[p.y][p.x] = p.cost;
    REP(dir, 4) {
      int nx = p.x + dx[dir];
      int ny = p.y + dy[dir];
      if (nx < 0 || nx >= w || ny < 0 || ny >= h) { continue; }
      if (visit[ny][nx]) { continue; }
      visit[ny][nx] = true;
      que.push(Point(nx, ny, p.cost + 1));
    }
  }
}

int main() {
  srand(123456789);
  scanf("%d", &stageCnt);
  REP(stage, stageCnt) {
    scanf("%d %d", &w, &h);
    REP(y, h) {
      REP(x, w) {
        char c;
        scanf(" %c ", &c);
        field[y][x] = c;
      }
    }
    CalcDist();
    scanf("%d", &l);
    scanf("%s", str);   //END
    assert(str[0] == 'E' && str[1] == 'N' && str[2] == 'D');
    int stageFirstMoney = 0;
    REP(j, l) {
      int life, money, T, E;
      scanf("%d %d %d %d", &life, &money, &T, &E);
      if (stageFirstMoney == 0) { stageFirstMoney = money; }
      vector<Tower> tower;
      vector<Enemy> enemy;
      REP(k, T) {
        int x, y, a, c;
        scanf("%d %d %d %d", &x, &y, &a, &c);
        tower.push_back(Tower(x, y, a, c));
        field[y][x] = 'P';
      }
      REP(k, E) {
        int x, y, t, l, s;
        scanf("%d %d %d %d %d", &x, &y, &t, &l, &s);
        enemy.push_back(Enemy(x, y, t, l, s));
      }
      scanf("%s", str);   //END
      assert(str[0] == 'E' && str[1] == 'N' && str[2] == 'D');
      vector<Tower> output;
//      money = 1000000;
      if (tower.empty()) {
        if (stage == 0) {
          output.push_back(Tower(2, 2, 0, 0));
          output.push_back(Tower(2, 3, 0, 0));
          output.push_back(Tower(2, 4, 0, 0));
          output.push_back(Tower(2, 5, 0, 0));
          output.push_back(Tower(4, 2, 0, 1));
          output.push_back(Tower(4, 3, 0, 1));
          output.push_back(Tower(4, 4, 0, 2));
          output.push_back(Tower(5, 2, 0, 0));
        } else {
          int cnt = 0;
          int use = 0;
          //PrintField();
          int IterCnt = 80;
          if (stage >= 15) { IterCnt = 400; }
          if (stage >= 25) { IterCnt = 600; }
          REP(iter, IterCnt) {
            int x = rand() % (w - 2) + 1;
            int y = rand() % (h - 2) + 1;
            if (OK(x, y)) {
              int type = rand() % 10;
              if (type < 2) { type = 0; }
              else if (type < 6) { type = 1; }
              else { type = 2; }
              if (iter > 150) { type = 0; }
              //type = 0;
              if (use + 10 + 5 * type >= stageFirstMoney / 2) { break; }
              field[y][x] = 'P';
              int level = 0;
              if (type != 2 && money - use >= 1000 && iter <= 200) {
                level = 2;
                //if (dist[y][x] <= 2) { level = 4; }
                if (type == 1) { level = 3; }
                //if (dist[y][x] <= 2) {
                //  level = 3;
                //} else if (dist[y][x] <= 4) {
                //  level = 2;
                //} else if (dist[y][x] <= 6) {
                //  level = 1;
                //}
              }
              use += (10 + 5 * type) * (level + 1) * (level + 2) / 2;
              output.push_back(Tower(x, y, level, type));
            } else {
              continue;
            }
          }
        }
      } else if (stageFirstMoney < money) {
        set<int> levelup;
        REP(iter, 5) {
          int m = tower.size();
          int r = rand() % m;
          if (levelup.count(r) || tower[r].level == 4) { continue; }
          if (dist[tower[r].y][tower[r].x] >= rand() % 10) { continue; }
          levelup.insert(r);
          int need = (10 + tower[r].type * 5) * (2 + tower[r].level);
          money -= need;
          if (stageFirstMoney * 2 >= money) { break; }
          output.push_back(Tower(tower[r].x, tower[r].y, tower[r].level + 1, tower[r].type));
        }
      }
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      //if (output.size() == 0) { puts(""); }
      fflush(stdout);
    }
  }
}
