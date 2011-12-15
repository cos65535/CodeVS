#include "Base.h"
#include "Field.h"
#include "Simulator.h"
#include "Structure.h"

int stageCnt;
int field[51][51];
int l;
int w, h;
char str[1000];
bool visit[60][60];
int dist[60][60];
const int dx[8] = { 1, 0, -1, 0, 1, 1, -1, -1 };
const int dy[8] = { 0, 1, 0, -1, 1, -1, -1, 1 };

struct Point {
  int x, y;
  double cost;
  Point() {;}
  Point(int x, int y, double cost) : x(x), y(y), cost(cost) {;}
  bool operator<(const Point &rhs) const {
    return cost > rhs.cost;
  }
};

void CalcDist() {
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
    dist[p.y][p.x] = (int)p.cost;
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

inline int CalcCost(int type, int plevel, int level) {
  return (10 + 5 * type) * ((level + 1) * (level + 2) / 2 - (plevel + 1) * (plevel + 2) / 2);
}


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


vector<TowerInfo> RappidPut(int stage, vector<TowerInfo> &tower, int stageFirstMoney, int money, int turn) {
  vector<TowerInfo> ret;
  if (stage == 0) {
    if (turn == 0) {
      ret.push_back(TowerInfo(2, 3, 3, 0));
    } else if (turn == 5) {
      ret.push_back(TowerInfo(2, 3, 4, 0));
    } else if (turn == 10) {
      ret.push_back(TowerInfo(2, 2, 0, 0));
      ret.push_back(TowerInfo(2, 4, 0, 0));
    }
    return ret;
  }
  if (stage == 1) {
    if (turn == 1) {
      ret.push_back(TowerInfo(3, 1, 0, 0));
    }
  }
  if (!tower.empty()) { return ret; }
  bool visit[60][60];
  REP(sy, h) {
    REP(sx, w) {
      if (field[sy][sx] != 's') { continue; }
      priority_queue<Point> que;
      que.push(Point(sx, sy, 0));
      MEMSET(visit, false);
      visit[sy][sx] = true;
      double cnt = 0;
      while (!que.empty()) {
        Point p = que.top();
        que.pop();
        if (p.cost >= 5) { break; }
        if (field[p.y][p.x] == 'P') {
          cnt++;
        }
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          if (nx < 0 || nx >= w || ny < 0 || ny >= h) { continue; }
          if (visit[ny][nx]) { continue; }
          visit[ny][nx] = true;
          double ncost = p.cost + 1;
          if (dir > 4) { ncost = p.cost + 1.41421356; }
          que.push(Point(nx, ny, ncost));
        }
      }

      //if (cnt >= 2) { continue; }
      que = priority_queue<Point>();
      que.push(Point(sx, sy, 0));
      MEMSET(visit, false);
      visit[sy][sx] = true;
      cnt = 0;
      while (!que.empty()) {
        Point p = que.top();
        que.pop();
        if (OK(p.x, p.y)) {
          int level = 4;
          //if (cnt == 1) { level = 0; }
          int type = 0;
//          if ((int)cnt % 4 == 2) { type = 2; }
          ret.push_back(TowerInfo(p.x, p.y, level, type));
          cnt++;
          field[p.y][p.x] = 'P';
          if (stage < 10 && cnt > 0) { break; }
          if (stage >= 10 && stage < 40 && cnt > 1) { break; }
          if (stage == 40 && cnt > 20) { break; }
          if (stage >= 41 && stage <= 46&& cnt > 60) { break; }
          if (stage == 47 && cnt > 50) { break; }
          if (stage == 48 && cnt > 35) { break; }
          if (stage >= 49 && cnt > 70) { break; }
//          if (cnt > 0) { break; }
        }
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          if (nx < 0 || nx >= w || ny < 0 || ny >= h) { continue; }
          if (visit[ny][nx]) { continue; }
          visit[ny][nx] = true;
          double ncost = p.cost + 1;
          if (dir > 4) { ncost = p.cost + 1.41421356; }
          que.push(Point(nx, ny, ncost));
        }
      }
    }
  }
  return ret;
}


int main() {
  srand(123456789);
//  Simulator simulator("input.txt");
  scanf("%d", &stageCnt);
  int plife = 10;
  int pmoney = 100;
  pair<int, int> ans(0, 0);
  REP(stage, stageCnt) {
    StageData stageData;
    stageData.LoadHeader(stdin);
    w = stageData.w;
    h = stageData.h;
    memcpy(field, stageData.field, sizeof(stageData.field));
    CalcDist();
    REP(j, stageData.levelCnt) {
      stageData.LoadMap(stdin);
//      fprintf(stderr, "Stage:%d-%d\n", stage + 1, j + 1);
//      fprintf(stderr, "Ans:%d %d\n", plife - stageData.infos.back().life, stageData.infos.back().money - pmoney);
//      if (stage != 40 && j == 0) {
//        assert(plife - ans.first == stageData.infos.back().life);
//        assert(pmoney + ans.second == stageData.infos.back().money);
//      }
      plife = stageData.infos.back().life;
      pmoney = stageData.infos.back().money;

      vector<TowerInfo> output;
      output = RappidPut(stage, stageData.infos.back().tower, 0, stageData.infos.back().money, j);

      // Print
      printf("%d\n", (int)output.size());
      FORIT(it, output) {
        printf("%d %d %d %d\n", it->x, it->y, it->level, it->type);
      }
      fflush(stdout);
//      ans = simulator.OneMapSimulation(stage, j, output, stageData.infos[j].tower);
//      fprintf(stderr, "Simulator:%d %d\n", ans.first, ans.second);
    }
  }
}
