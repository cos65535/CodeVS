#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Simulator.h"
#include "Mask.h"
#include "Misc.h"

namespace Final {
  const int dx[4] = { 1, 0, -1, 0 };
  const int dy[4] = { 0, 1, 0, -1 };

  int Put(const Field &field, int mask[51][51], int x, int y, int &use) {
    if (field.field[y][x] != '0') { return use; }
    if (mask[y][x] != 0) { return use; }
    mask[y][x] = 11;
    use++;
    return use;
  }

  int ExpandMask(const Field &field, int bestMask[51][51], int useCnt) {
    const int h = field.h;
    const int w = field.w;
    int use = CalcUse(field, bestMask);
    int bestDist = field.CalcDist(bestMask);
    bool ban[51][51];
    MEMSET(ban, false);
    REP(iter, 300) {
      if (use >= useCnt) { break; }
      int tx = -1;
      int ty = -1;
      REP(y, h) {
        REP(x, w) {
          if (ban[y][x]) { continue; }
          ban[y][x] = true;
          if (!field.OK(bestMask, x, y)) { continue; }
          ban[y][x] = false;
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


  int CalcMask(int iter, const Field &field, int mask[51][51], int useCnt) {
    int dist = 0;
    int use = 0;
    int target = rand() % field.gs.size();
    memset(mask, 0, sizeof(int) * 51 * 51);
    //target以外を埋める
    REP(i, field.gs.size()) {
      if (i == target) { continue; }
      REP(dir, 4) {
        int nx = field.gs[i].x + dx[dir];
        int ny = field.gs[i].y + dy[dir];
        Put(field, mask, nx, ny, use);
      }
    }
    if (!field.OK2(mask)) { return CalcMask(iter, field, mask, useCnt); }

    int px = field.gs[target].x;
    int py = field.gs[target].y;
    int ppx = px;
    int ppy = py;
    int dir = rand() % 4;
    int checked = 0;
    REP(iter2, 2000) {
      if (use > useCnt) { break; }
      if (iter < 3600 / 3) {
        dir = (dir + 3) % 4;
      } else if (iter < 3600 / 3 * 2) {
        dir = (dir + 1) % 4;
      } else {
      }
      if (rand() % 100 < (iter % (3600 / 3)) * 100 / (3600 / 3) + 5) {
        dir = rand() % 4;
      }
      if (checked & (1 << dir)) { continue; }
      checked |= 1 << dir;
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
        if (!ng) {
          ng |= !field.OK(mask, nnx, nny);
        }
        Put(field, mask, nnx, nny, use);
      }
      if (ng) {
        REP(d, 4) {
          int nnx = px + dx[d];
          int nny = py + dy[d];
          if (mask[nny][nnx]) {
            use--;
            mask[nny][nnx] = 0;
          }
        }
        if (checked != 15) { goto next; }
      }
      ppx = px;
      ppy = py;
      px = nx;
      py = ny;
      checked = 0;
next:;
    }

    //EraseUneedTower(field, mask);
    //PrintMask(field, bestMask);
    //ExpandMask(field, mask, useCnt);
    dist = field.CalcDist(mask);
    return dist;
  }

  int CalcBestMask(const Field &field, int bestMask[51][51], int useCnt) {
    int bestDist = -1;
    memset(bestMask, 0x0f, sizeof(int) * 51 * 51);
    REP(iter, 3600) {
      int mask[51][51];
      //int dist = CalcMask(rand() % 3600, field, mask, useCnt);
      int dist = CalcMask(rand() % 2400, field, mask, useCnt);
      // bestを更新
      if (dist > bestDist) {
        memcpy(bestMask, mask, sizeof(int) * 51 * 51);
        bestDist = dist;
      }
    }
    return bestDist;
  }

  struct Point {
    int x, y;
    int cost;
    Point() {;}
    Point(int x, int y, int cost) : x(x), y(y), cost(cost) {;}
    bool operator<(const Point &rhs) const {
      return cost > rhs.cost;
    }
  };

  void SetFrozenTower(const Field &field, int bestMask[51][51], int frozenCnt) {
    if (frozenCnt == 0) { return; }
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
        if (level > 4) { break; }
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

  void Simulation(const MapInfo &mapInfo, const int map, const int level, int mask[51][51]) {
    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.resize(40);
    simulator.stages.push_back(mapInfo);

    int sums[51][51];
    MEMSET(sums, 0);
    Field field(mapInfo.field, w, h);
    field.PutTower(mapInfo.levels[level].tower);
    field.CalcSum(mask, sums);

    while (true) {
      vector<TowerInfo> tower = MaskToTower(field, mask, 1 << 20);
      pair<int, int> ans = simulator.LevelSimulation(40, level, tower);
      if (ans.first == 0) { break; }
      //PrintMask(field, mask);

      REP(iter, ans.first) {
        int bestSum = -1;
        int tx = -1;
        int ty = -1;
        REP(sy, h) {
          REP(sx, w) {
            if (mask[sy][sx] >= 15) { continue; }
            if (mask[sy][sx] == 0 && field.field[sy][sx] != 1100 && !field.OK(mask, sx, sy)) { continue; }
            int sum = sums[sy][sx];
            if (sum > bestSum) {
              bestSum = sum;
              tx = sx;
              ty = sy;
            }
          }
        }
        if (bestSum == -1) { break; }
        mask[ty][tx] = 15;
      }
      //PrintMask(field, mask);
    }
  }

  vector<TowerInfo> AI(const MapInfo &mapInfo, const int map, const int level) {
    Field field(mapInfo);
    field.PutTower(mapInfo.levels[level].tower);
    if (level != 0) {
      int mask[51][51];
      MEMSET(mask, 0);
      Simulation(mapInfo, map, level, mask);
      return MaskToTower(field, mask, mapInfo.levels[level].money);
    }

    vector<pair<int, vector<TowerInfo> > > ans;
    const int ITER_CNT = 3000;
    ans.resize(ITER_CNT);
    int bestMask[51][51];
    //int t1 = timeGetTime();
    CalcBestMask(field, bestMask, 150);
    //PrintMask(field, bestMask);
    //int t2 = timeGetTime();
    //PrintMask(field, bestMask);
    //int t3 = timeGetTime();
    ExpandMask(field, bestMask, 150);
    //PrintMask(field, bestMask);
    //int t4 = timeGetTime();
    //EraseUneedTower(field, bestMask);
    //PrintMask(field, bestMask);
    //int t5 = timeGetTime();
    SetFrozenTower(field, bestMask, 10);
    //PrintMask(field, bestMask);
    //int t6 = timeGetTime();
    Simulation(mapInfo, map, level, bestMask);
    PrintMask(field, bestMask);
    //int t7 = timeGetTime();
    //fprintf(stderr, "%d %d %d %d %d %d¥n", t2 - t1, t3 - t2, t4 - t3, t5 - t4, t6 - t5, t7 - t6);
    //PrintMask(field, bestMask);

    return MaskToTower(field, bestMask, mapInfo.levels[level].money);
  }
};
