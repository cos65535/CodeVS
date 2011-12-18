#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Simulator.h"


namespace Tron {
  const int dx[4] = { 1, 0, -1, 0 };
  const int dy[4] = { 0, 1, 0, -1 };

  int CalcUse(const Field &field, const int mask[51][51]) {
    int ret = 0;
    REP(y, field.h) {
      REP(x, field.w) {
        ret += mask[y][x] != 0;
      }
    }
    return ret;
  }

  void PrintMask(const Field &field, const int mask[51][51]) {
    printf("%d %d\n", field.CalcDist(mask), CalcUse(field, mask));
    REP(y, field.h) {
      REP(x, field.w) {
        if (field.field[y][x] != '0') {
          int v = field.field[y][x];
          if (v == 1100) { v = '2'; }
          if (v == 1500) { v = '3'; }
          printf("%c", v);
        } else if (mask[y][x] != 0) {
          int v = '4';
          if (mask[y][x] == 15) { v = '5'; }
          printf("%c", v);
        } else {
          printf(".");
        }
      }
      puts("");
    }
  }

  vector<TowerInfo> MaskToTower(const Field &field, const int mask[51][51], int money) {
    vector<TowerInfo> ret;
    REP(y, field.h) {
      REP(x, field.w) {
        int type = mask[y][x] / 10 - 1;
        int level = mask[y][x] % 10 - 1;
        if (type >= 0) {
          if (money - CalcCost(type, -1, level) < 0) { continue; }
          money -= CalcCost(type, -1, level);
          ret.push_back(TowerInfo(x, y, level, type));
        }
      }
    }
    return ret;
  }

  int Put(const Field &field, int mask[51][51], int x, int y, int &use) {
    if (field.field[y][x] != '0') { return use; }
    if (mask[y][x] != 0) { return use; }
    mask[y][x] = 11;
    use++;
    return use;
  }

  int CalcMask(int iter, const Field &field, int mask[51][51], int useCnt) {
    int dist = 0;
    int use = 0;
    int target = rand() % field.gs.size();
    memset(mask, 0, sizeof(int) * 51 * 51);
    //targetˆÈŠO‚ð–„‚ß‚é
    REP(i, field.gs.size()) {
      if (i == target) { continue; }
      REP(dir, 4) {
        int nx = field.gs[i].x + dx[dir];
        int ny = field.gs[i].y + dy[dir];
        Put(field, mask, nx, ny, use);
      }
    }
    if (field.CalcDist(mask) == -1) { return CalcMask(iter, field, mask, useCnt); }

    int px = field.gs[target].x;
    int py = field.gs[target].y;
    int ppx = px;
    int ppy = py;
    int dir = rand() % 4;
    int ngCnt = 0;
    REP(iter2, 2000) {
      if (use > useCnt) { break; }
      if (iter < 1200) {
        dir = (dir + 3) % 4;
      } else if (iter < 2400) {
        dir = (dir + 1) % 4;
      } else {
      }
      if (rand() % 100 < (iter % 1200) / 12 + 5) {
        dir = rand() % 4;
      }
      int nx = px + dx[dir];
      int ny = py + dy[dir];
      if (field.field[ny][nx] != '0' || mask[ny][nx] != 0) { continue; }
      if (nx == ppx && ny == ppy) { continue; }
      bool ng = false;
      REP(d, 4) {
        int nnx = px + dx[d];
        int nny = py + dy[d];
        if (field.field[nny][nnx] != '0' || mask[nny][nnx] != 0) { continue; }
        if (nnx == ppx && nny == ppy) { continue; }
        if (dir == d) { continue; }
        ng |= !field.OK(mask, nnx, nny);
        Put(field, mask, nnx, nny, use);
      }
      if (ng) {
        ngCnt++;
        REP(d, 4) {
          int nnx = px + dx[d];
          int nny = py + dy[d];
          if (mask[nny][nnx]) {
            use--;
            mask[nny][nnx] = 0;
          }
        }
        if (ngCnt <= 10) { goto next; }
      }
      ngCnt = 0;
      ppx = px;
      ppy = py;
      px = nx;
      py = ny;
next:;
    }

    dist = field.CalcDist(mask);
    //printf("%d %d\n", dist, use);
    //REP(y, field.h) {
    //  REP(x, field.w) {
    //    if (field.field[y][x] != '0') {
    //      printf("%c", field.field[y][x]);
    //    } else if (mask[y][x] != 0) {
    //      printf("2");
    //    } else {
    //      printf(".");
    //    }
    //  }
    //  puts(""); 
    //}
    return dist;
  }

  int CalcBestMask(const Field &field, int bestMask[51][51], int useCnt) {
    int bestDist = -1;
    memset(bestMask, 0x0f, sizeof(int) * 51 * 51);
    REP(iter, 3600) {
      int mask[51][51];
      int dist = CalcMask(iter, field, mask, useCnt);
      // best‚ðXV
      if (dist > bestDist) {
        memcpy(bestMask, mask, sizeof(int) * 51 * 51);
        bestDist = dist;
      }
    }
    return bestDist;
  }

  int EraseUneedTower(const Field &field, int bestMask[51][51]) {
    int cnt = 0;
    int bestDist = field.CalcDist(bestMask);
    REP(y, field.h) {
      REP(x, field.w) {
        if (bestMask[y][x]) {
          int v = bestMask[y][x];
          bestMask[y][x]= 0;
          if (bestDist == field.CalcDist(bestMask)) {
            continue;
          }
          bestMask[y][x] = v;
          cnt++;
        }
      }
    }
    return cnt;
  }

  int ExpandMask(const Field &field, int bestMask[51][51], int useCnt) {
    const int h = field.h;
    const int w = field.w;
    int use = CalcUse(field, bestMask);
    int bestDist = field.CalcDist(bestMask);
    REP(iter, 300) {
      if (use >= useCnt) { continue; }
      int tx = -1;
      int ty = -1;
      REP(y, h) {
        REP(x, w) {
          if (!field.OK(bestMask, x, y)) { continue; }
          bestMask[y][x] = 1;
          int dist = field.CalcDist(bestMask);
          bestMask[y][x] = 0;
          if (dist > bestDist) {
            bestDist = dist;
            tx = x;
            ty = y;
          }
        }
      }
      if (tx == -1) { break; }
      bestMask[ty][tx] = 11;
      use++;
    }
    return use;
  }

  void SetFrozenTower(const Field &field, int bestMask[51][51], int frozenCnt) {
    if (frozenCnt == 0) { return; }
    struct Point {
      int x, y;
      double cost;
      Point() {;}
      Point(int x, int y, double cost) : x(x), y(y), cost(cost) {;}
      bool operator<(const Point &rhs) const {
        return cost > rhs.cost;
      }
    };
    const int w = field.w;
    const int h = field.h;
    int route[51][51];
    field.CalcEnemyRoute(bestMask, route);
    int tx = -1;
    int ty = -1;
    FORIT(it, field.gs) {
      REP(dir, 4) {
        int nx = it->x + dx[dir];
        int ny = it->y + dy[dir];
        if (route[ny][nx] >= 1) {
          tx = nx;
          ty = ny;
        }
      }
    }
    priority_queue<Point> que;
    que.push(Point(tx, ty, 0));
    int cnt = 0;
    bool visit[51][51];
    MEMSET(visit, false);
    while (!que.empty()) {
      Point p = que.top();
      que.pop();
      if (visit[p.y][p.x]) { continue; }
      visit[p.y][p.x] = true;
      if ((p.y != ty || p.x != tx) && (bestMask[p.y][p.x] || field.OK(bestMask, p.x, p.y))) {
        int level = 0;
        while (p.cost > square(2 + level)) { level++; }
        if (level > 1) { break; }
        bestMask[p.y][p.x] = 31 + level;
        cnt++;
        if (cnt == frozenCnt) { break; }
      }
      REP(dir, 4) {
        int nx = p.x + dx[dir];
        int ny = p.y + dy[dir];
        int ncost = square(nx - tx) + square(ny - ty);
        if (nx < 0 || nx >= w || ny < 0 || ny >= h) { continue; }
        if (visit[ny][nx]) { continue; }
        que.push(Point(nx, ny, ncost));
      }
    }
  }

  void Simulation(MapInfo mapInfo, const int map, const int level, int mask[51][51]) {
    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.push_back(mapInfo);

    int iniMask[51][51];
    memcpy(iniMask, mask, sizeof(int) * 51 * 51);

    while (true) {
      Field field(mapInfo.field, w, h);
      vector<TowerInfo> tower = MaskToTower(field, mask, mapInfo.levels[level].money);
      pair<int, int> ans = simulator.LevelSimulation(0, level, tower);
      if (ans.first == 0) { break; }

      if (level != 0) {
        field.PutTower(mapInfo.levels[level].tower);
      }
      int route[51][51];
      field.CalcEnemyRoute(mask, route);
      int bestSum = -1;
      int tx = -1;
      int ty = -1;
      REP(sy, h) {
        REP(sx, w) {
          if (mask[sy][sx] >= 15 || field.field[sy][sx] == 1500) { continue; }
          if (field.field[sy][sx] != 1100 && mask[sy][sx] == 0 && !field.OK(mask, sx, sy)) { continue; }
          int sum = 0;
          REP(y, h) {
            REP(x, w) {
              int d = square(sy - y) + square(sx - x);
              if (d > square(4 + 4)) { continue; }
              sum += route[y][x];
            }
          }
          if (sum > bestSum) {
            bestSum = sum;
            tx = sx;
            ty = sy;
          }
        }
      }
      if (bestSum == -1) { break; }
      mask[ty][tx] = 15;
      //PrintMask(field, mask);
    }
  }

  vector<TowerInfo> TronAI(const MapInfo &mapInfo, const int map, const int level) {
    if (level != 0) {
      Field field(mapInfo.field, mapInfo.w, mapInfo.h);
      int mask[51][51];
      MEMSET(mask, 0);
      Simulation(mapInfo, map, level, mask);
      return MaskToTower(field, mask, mapInfo.levels[level].money);
    }
    int mapUse[80];
    REP(i, 80) { mapUse[i] = 70; }
    mapUse[40] = 10;
    mapUse[41] = 10;
    mapUse[42] = 10;
    mapUse[43] = 20;
    mapUse[44] = 20;
    mapUse[45] = 20;
    mapUse[46] = 20;
    mapUse[47] = 33; //!
    mapUse[48] = 30; //!
    mapUse[49] = 30; //!
    mapUse[50] = 40; //!

    mapUse[51] = 30;
    mapUse[52] = 40;
    mapUse[53] = 32;
    mapUse[54] = 42;
    mapUse[55] = 42; //!
    mapUse[56] = 45; //!
    mapUse[57] = 51; //!!
    mapUse[58] = 45;
    mapUse[59] = 70; //!!
    mapUse[60] = 51;

    mapUse[61] = 71; //!!
    mapUse[62] = 65;
    mapUse[63] = 50;
    mapUse[64] = 58;
    mapUse[65] = 60;
    mapUse[66] = 65;
    mapUse[67] = 50;
    mapUse[68] = 43;
    mapUse[69] = 65;
    mapUse[70] = 70;

    mapUse[71] = 80;
    mapUse[72] = 70;

    static const int offset = 15;
    const int h = mapInfo.h;
    const int w = mapInfo.w;
    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
    int bestMask[51][51];
    int bestDist = CalcBestMask(field, bestMask, mapUse[map] * 2 + offset);
    //PrintMask(field, bestMask);
    EraseUneedTower(field, bestMask);
    //PrintMask(field, bestMask);
    ExpandMask(field, bestMask, mapUse[map] * 2 + offset);
    //PrintMask(field, bestMask);
    EraseUneedTower(field, bestMask);
    //PrintMask(field, bestMask);
    if (map > 55) {
      SetFrozenTower(field, bestMask, 12);
    }
    //PrintMask(field, bestMask);
    Simulation(mapInfo, map, level, bestMask);
    //PrintMask(field, bestMask);

    return MaskToTower(field, bestMask, mapInfo.levels[level].money);
  }
}