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
    //int pattern[9][51][51];
    //memset(pattern, 0, 9 * sizeof(int) * 51 * 51);
    const int h = field.h;
    const int w = field.w;
    int use = CalcUse(field, bestMask);
    ll bestDist = field.CalcDist3(bestMask);
    bool ban[51][51];
    MEMSET(ban, false);
    REP(iter, 10000) {
      //if (use >= useCnt) { break; }
      int tx = -1;
      int ty = -1;
      REP(y, h) {
        REP(x, w) {
          if (ban[y][x]) { continue; }
          ban[y][x] = true;
          if (!field.OK(bestMask, x, y)) { continue; }
          ban[y][x] = false;
          bestMask[y][x] = 1;
          ll dist = field.CalcDist3(bestMask);
          bestMask[y][x] = 0;
          if (dist > bestDist) {
            bestDist = dist;
            tx = x;
            ty = y;
            bestMask[ty][tx] = 11;
          }
        }
      }
      if (tx == -1) { break; }
      use++;
    }
    return use;
  }


  ll CalcMask(const Field &field, int mask[51][51], int useCnt) {
    ll dist = 0;
    int use = 0;
    int target = rand() % field.gs.size();
    //target以外を埋める
    REP(i, field.gs.size()) {
      if (i == target) { continue; }
      REP(dir, 4) {
        int nx = field.gs[i].x + dx[dir];
        int ny = field.gs[i].y + dy[dir];
        Put(field, mask, nx, ny, use);
      }
    }
    if (!field.OK2(mask)) { return -1; }

    bool ban[51][51];
    MEMSET(ban, false);
    int px = field.gs[target].x;
    int py = field.gs[target].y;
    int ppx = px;
    int ppy = py;
    int dir = rand() % 4;
    int checked = 0;
    REP(iter2, 100) {
      if (use > useCnt) { break; }
      ban[py][px] = true;
      dir = rand() % 4;
      if (checked & (1 << dir)) { continue; }
      checked |= 1 << dir;
      int nx = px + dx[dir];
      int ny = py + dy[dir];
      if (field.field[ny][nx] != '0' || mask[ny][nx] != 0) { continue; }
      if (nx == ppx && ny == ppy) { continue; }
      bool ng = false;
      bool end = false;
      bool first = true;
      int pmask[4] = { 0, 0, 0, 0 };
      REP(d, 4) {
        int nnx = px + dx[d];
        int nny = py + dy[d];
        pmask[d] = mask[nny][nnx];
        if (field.field[nny][nnx] == 's') { end = true; }
        if (field.field[nny][nnx] != '0' || mask[nny][nnx] != 0) { continue; }
        if (nnx == ppx && nny == ppy) { continue; }
        if (dir == d) { continue; }
        if (!ng) {
          ng |= ban[nny][nnx] || !field.OK(mask, nnx, nny);
          if (first && ng) { ban[nny][nnx] = true; }
        }
        if (mask[nny][nnx] == 0) {
          first = false;
          Put(field, mask, nnx, nny, use);
        }
      }
      if (ng) {
        REP(d, 4) {
          int nnx = px + dx[d];
          int nny = py + dy[d];
          if (mask[nny][nnx] != pmask[d]) {
            use--;
            mask[nny][nnx] = 0;
          }
        }
        if (checked != 15) {
          goto next;
        }
      }
      ppx = px;
      ppy = py;
      px = nx;
      py = ny;
      checked = 0;
next:;
     if (end) { break; }
    }
    /*
    REP(i, 1) {
      REP(y, field.h) {
        REP(x, field.w) {
          int lx = x % 2;
          int ly = y % 2;
          int tx = (i >> 0) & 1;
          int ty = (i >> 1) & 1;
          if (i < 4 && x % 4 >= 2) { ty = (ty + 1) % 2; }
          if (i >= 4 && y % 4 >= 2) { tx = (tx + 1) % 2; }
          if (lx == tx && ly == ty && field.OK(mask, x, y)) {
            mask[y][x] = 11;
          }
        }
      }
      //PrintMask(field, pattern[i]);
    }
    */

    //EraseUneedTower(field, mask);
    //PrintMask(field, bestMask);
    //ExpandMask(field, mask, useCnt);
    dist = field.CalcDist(mask);
    return dist;
  }

  ll CalcBestMask(const Field &field, int bestMask[51][51], int useCnt) {
    ll bestDist = -1;
    memset(bestMask, 0x0f, sizeof(int) * 51 * 51);
    REP(iter, 360) {
      int mask[51][51];
      MEMSET(mask, 0);
      ll dist = CalcMask(field, mask, useCnt);
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

  int Simulation(const MapInfo &mapInfo, const int map, const int level, int mask[51][51]) {

    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.resize(40);
    simulator.stages.push_back(mapInfo);

    int ret = -150000;
    int sums[51][51];
    MEMSET(sums, 0);
    Field field(mapInfo.field, w, h);
    field.PutTower(mapInfo.levels[level].tower);
    field.CalcSum(mask, sums);

    while (true) {
      vector<TowerInfo> tower = MaskToTower(field, mask, 1 << 20);
      //PrintMask(field, mask);
      pair<int, int> ans = simulator.LevelSimulation(40, level, tower);
      if (ans.first == 0) {
        ret = ans.second;
        break;
      }

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
        if (bestSum == -1) { return ret; }
        mask[ty][tx] = 15;
      }
      //PrintMask(field, mask);
    }
    return ret;
  }
  int Simulation2(const MapInfo &mapInfo, const int map, const int level, int mask[51][51], int best) {

    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.resize(40);
    simulator.stages.push_back(mapInfo);

    int ret = -150000;
    int sums[51][51];
    MEMSET(sums, 0);
    Field field(mapInfo.field, w, h);
    field.PutTower(mapInfo.levels[level].tower);
    field.CalcSum(mask, sums);

    if (level != 0) {
      vector<TowerInfo> tower;
      pair<int, int> ans = simulator.LevelSimulation(40, level, tower, 1);
      if (ans.first == 0) { return 0; }
    }
        

    int bestMask[51][51];
    memcpy(bestMask, mask, sizeof(int) * 51 * 51);
    int cnt = 0;
    vector<bool> usable;
    vector<pair<int, int> > candidate;
    {
      REP(sy, h) {
        REP(sx, w) {
          if (mask[sy][sx] >= 15 || (field.field[sy][sx] != 1100 && field.field[sy][sx] != '0')) { continue; }
          int sum = sums[sy][sx];
          candidate.push_back(make_pair(sum, (sy << 10) + sx));
          usable.push_back(true);
        }
      }
      sort(candidate.rbegin(), candidate.rend());
      int lmask[51][51];
      memcpy(lmask, mask, sizeof(int) * 51 * 51);
      REP(i, candidate.size()) {
        int x = candidate[i].second & 1023;
        int y = candidate[i].second >> 10;
        if (lmask[y][x] != 11 && field.field[y][x] != 1100 && !field.OK(lmask, x, y)) { usable[i] = false; continue; }
        cnt++;
        lmask[y][x] = 15;
      }
    }

    int lo = 0;
    int hi = min(cnt, 254);
    while (lo != hi) {
      int mid = (lo + hi) / 2;
      int lmask[51][51];
      memcpy(lmask, mask, sizeof(int) * 51 * 51);
      int offset = 0;
      REP(i, mid + offset) {
        if (!usable[i]) { offset++; continue; }
        int x = candidate[i].second & 1023;
        int y = candidate[i].second >> 10;
        lmask[y][x] = 15;
      }

      vector<TowerInfo> tower = MaskToTower(field, lmask, 1 << 20);
      //PrintMask(field, mask);
      pair<int, int> ans = simulator.LevelSimulation(40, level, tower, 1);
      //cout << mid << " " << ans.first << " " << ans.second << endl;
      if (ans.first == 0) {
        ret = max(ret, ans.second);
        memcpy(bestMask, lmask, sizeof(int) * 51 * 51);
        hi = mid;
      } else {
        lo = mid + 1;
      }

      //PrintMask(field, mask);
    }
    memcpy(mask, bestMask, sizeof(int) * 51 * 51);
    return ret;
  }

  vector<TowerInfo> AI(const MapInfo &mapInfo, MapInfo pmapInfo, const int map, const int level) {
    Field field(mapInfo);
    field.PutTower(mapInfo.levels[level].tower);
    if (level != 0) {
      int mask[51][51];
      MEMSET(mask, 0);
      Simulation2(mapInfo, map, level, mask, -150000);
      return MaskToTower(field, mask, mapInfo.levels[level].money);
    }

    memcpy(pmapInfo.field, mapInfo.field, sizeof(int) * 51 * 51);
    pmapInfo.w = mapInfo.w;
    pmapInfo.h = mapInfo.h;
    pmapInfo.levels[0].money = mapInfo.levels[level].money;
    pmapInfo.levels[0].life = mapInfo.levels[level].life;
    pmapInfo.levels[0].tower.clear();
    pmapInfo.levels[0].enemy = pmapInfo.levels.back().enemy;
    FORIT(it, pmapInfo.levels[0].enemy) {
      int r = rand() % field.ss.size();
      it->x = field.ss[r].x;
      it->y = field.ss[r].y;
    }
    pmapInfo.levels.resize(1);

    vector<pair<int, vector<TowerInfo> > > ans;
    int best = -1500000;
    const int ITER_CNT = 5;
    ans.resize(ITER_CNT);
    int bestMask[51][51];
    vector<MaskInfo> maskInfos(ITER_CNT);
    REP(iter, ITER_CNT) {
      int mask[51][51];
      maskInfos[iter].money = -100000;
      //int t1 = timeGetTime();
      CalcBestMask(field, mask, 150);
      //PrintMask(field, bestMask);
      //int t2 = timeGetTime();
      //PrintMask(field, bestMask);
      //int t3 = timeGetTime();
      ExpandMask(field, mask, 150);
      //PrintMask(field, bestMask);
      //int t4 = timeGetTime();
      //EraseUneedTower(field, bestMask);
      //PrintMask(field, bestMask);
      //int t5 = timeGetTime();
      SetFrozenTower(field, mask, 10);
      //PrintMask(field, bestMask);
      //int t6 = timeGetTime();
      //PrintMask(field, bestMask);
      //if (map == 0 || map == 40) {
      maskInfos[iter].money = 0;
      memcpy(maskInfos[iter].mask, mask, sizeof(int) * 51 * 51);
      maskInfos[iter].money = Simulation2(pmapInfo, map, 0, mask, best);
      //}
      //Simulation2(mapInfo, map, level, bestMask);
    }
    sort(maskInfos.rbegin(), maskInfos.rend());
    memcpy(bestMask, maskInfos[0].mask, sizeof(int) * 51 * 51);
    Simulation2(mapInfo, map, level, bestMask, -15000000);
    //PrintMask(field, bestMask);
    //int t7 = timeGetTime();
    //fprintf(stderr, "%d %d %d %d %d %d¥n", t2 - t1, t3 - t2, t4 - t3, t5 - t4, t6 - t5, t7 - t6);
    //PrintMask(field, bestMask);

    return MaskToTower(field, bestMask, mapInfo.levels[level].money);
  }
};
