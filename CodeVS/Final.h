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

  int Put(const Field &field, int mask[FS][FS], int x, int y, int &use) {
    if (field.field[y][x] != '0') { return use; }
    if (mask[y][x] != 0) { return use; }
    mask[y][x] = 11;
    use++;
    return use;
  }

  int ExpandMask(const Field &field, int bestMask[FS][FS], int useCnt) {
    //int pattern[9][FS][FS];
    //memset(pattern, 0, 9 * sizeof(int) * FS * FS);
    const int h = field.h;
    const int w = field.w;
    int use = CalcUse(field, bestMask);
    ll bestDist = field.CalcDist3(bestMask);
    bool ban[FS][FS];
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


  int CalcMask(const Field &field, int mask[FS][FS], int useCnt) {
    int dist = 0;
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

    bool ban[FS][FS];
    MEMSET(ban, false);
    int px = field.gs[target].x;
    int py = field.gs[target].y;
    int ppx = px;
    int ppy = py;
    int dir = rand() % 4;
    int checked = 0;
    REP(iter2, 150) {
      if (use > useCnt) { break; }
      ban[py][px] = true;
      dir = rand() % 4;
      if (checked & (1 << dir)) { continue; }
      checked |= 1 << dir;
      int nx = px + dx[dir];
      int ny = py + dy[dir];
      if (field.field[ny][nx] != '0' || mask[ny][nx] != 0 || (nx == ppx && ny == ppy)) { iter2--; continue; }
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
        ng |= ban[nny][nnx] || !field.OK(mask, nnx, nny);
        if (first && ng) { ban[nny][nnx] = true; }
        if (ng) { break; }
        first = false;
        use++;
        mask[nny][nnx] = 11;
        //Put(field, mask, nnx, nny, use);
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

    //REP(i, 1) {
    //  REP(y, field.h) {
    //    REP(x, field.w) {
    //      int lx = x % 2;
    //      int ly = y % 2;
    //      int tx = (i >> 0) & 1;
    //      int ty = (i >> 1) & 1;
    //      if (i < 4 && x % 4 >= 2) { ty = (ty + 1) % 2; }
    //      if (i >= 4 && y % 4 >= 2) { tx = (tx + 1) % 2; }
    //      if (lx == tx && ly == ty && field.OK(mask, x, y)) {
    //        mask[y][x] = 11;
    //      }
    //    }
    //  }
    //  //PrintMask(field, pattern[i]);f
    //}

    //EraseUneedTower(field, mask);
    //PrintMask(field, bestMask);
    //ExpandMask(field, mask, useCnt);
    dist = field.CalcDist(mask);

    //int sums[FS][FS];
    //field.CalcSum(mask, sums);
    //int s[FS * FS];
    //int cnt = 0;
    //REP(y, field.h) {
    //  REP(x, field.w) {
    //    if (mask[y][x] == 11) { s[cnt++] = sums[y][x]; }
    //  }
    //}
    //sort(s, s + cnt);
    //reverse(s, s + cnt);
    //int v = 0;
    //REP(i, 30) {
    //  v += s[i];
    //}

    return dist;
  }

  vector<MaskInfo> CalcBestMask(const Field &field, int useCnt) {
    ll bestDist = -1;
    int ITER_CNT = 720;
    vector<MaskInfo> maskInfo(ITER_CNT);
    REP(iter, ITER_CNT) {
      MEMSET(maskInfo[iter].mask, 0);
      maskInfo[iter].money = CalcMask(field, maskInfo[iter].mask, useCnt);
    }
    sort(maskInfo.rbegin(), maskInfo.rend());
    return maskInfo;
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

  void SetFrozenTower(const Field &field, int bestMask[FS][FS], int frozenCnt) {
    if (frozenCnt == 0) { return; }
    const int w = field.w;
    const int h = field.h;
    int route[FS][FS];
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
    bool visit[FS][FS];
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

  bool calcCandidate;
  int Simulation2(const MapInfo &mapInfo, const int map, const int level, int mask[FS][FS]) {

    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.push_back(mapInfo);

    int ret = -15000000;
    int sums[FS][FS];
    MEMSET(sums, 0);
    Field field(mapInfo.field, w, h);
    field.PutTower(mapInfo.levels[level].tower);
    field.CalcSum(mask, sums);

    if (level != 0) {
      vector<TowerInfo> tower;
      pair<int, int> ans = simulator.LevelSimulation(false, 0, level, tower, 1);
      if (ans.first == 0) { return 0; }
    }
        

    int bestMask[FS][FS];
    memcpy(bestMask, mask, sizeof(int) * FS * FS);
    int cnt = 0;
    static vector<bool> usable;
    static vector<pair<int, int> > candidate;
    if (!calcCandidate) {
      usable.clear();
      candidate.clear();
      REP(sy, h) {
        REP(sx, w) {
          if (mask[sy][sx] >= 15 || (field.field[sy][sx] != 1100 && field.field[sy][sx] != '0')) { continue; }
          int sum = sums[sy][sx];
          candidate.push_back(make_pair(sum, (sy << 10) + sx));
          usable.push_back(true);
        }
      }
      sort(candidate.rbegin(), candidate.rend());
      int lmask[FS][FS];
      memcpy(lmask, mask, sizeof(int) * FS * FS);
      REP(i, candidate.size()) {
        int x = candidate[i].second & 1023;
        int y = candidate[i].second >> 10;
        if (lmask[y][x] != 11 && field.field[y][x] != 1100 && !field.OK(lmask, x, y)) { usable[i] = false; continue; }
        cnt++;
        lmask[y][x] = 15;
      }
      calcCandidate = true;
    } else {
      REP(i, candidate.size()) {
        int x = candidate[i].second & 1023;
        int y = candidate[i].second >> 10;
        if (field.field[y][x] == 1500) { usable[i] = false; }
        cnt += usable[i] ? 1 : 0;
      }
    }

    int best = -1500000;
    int lo = 0;
    int hi = min(cnt, 254);
    while (true) {
      int mid = (lo + hi) / 2;
      int lmask[FS][FS];
      memcpy(lmask, mask, sizeof(int) * FS * FS);
      int offset = 0;
      REP(i, min((int)usable.size(), mid + offset)) {
        if (!usable[i]) { offset++; continue; }
        int x = candidate[i].second & 1023;
        int y = candidate[i].second >> 10;
        lmask[y][x] = 15;
      }

      //PrintMask(field, lmask);
      vector<TowerInfo> tower = MaskToTower(field, lmask, 1 << 20);
      pair<int, int> ans = simulator.LevelSimulation(false, 0, level, tower, 1);
      ans.second += -ans.first * 50000;
      //cout << mid << " " << ans.first << " " << ans.second << endl;
      if (ans.second > best || best < -50000) {
        best = ans.second;
        ret = max(ret, ans.second);
        memcpy(bestMask, lmask, sizeof(int) * FS * FS);
      }
      if (lo == hi) { break; }
      if (ans.first == 0) {
        hi = mid;
      } else {
        lo = mid + 1;
      }
    }
    memcpy(mask, bestMask, sizeof(int) * FS * FS);
    return ret;
  }

  vector<TowerInfo> AI(const MapInfo &mapInfo, const int map, const int level) {
    Field field(mapInfo);
    field.PutTower(mapInfo.levels[level].tower);
    if (level != 0) {
      int mask[FS][FS];
      MEMSET(mask, 0);
      Simulation2(mapInfo, map, level, mask);
      return MaskToTower(field, mask, mapInfo.levels[level].money);
    }

    MapInfo mousou(mapInfo);
    mousou.levels[0].enemy.clear();
    {
      int cnt = (int)(sqrt((double)(map + 1) * 25) * 2 + 11);
      REP(i, cnt) {
        EnemyInfo enemy;
        int r = rand() % field.ss.size();
        enemy.x = field.ss[r].x;
        enemy.y = field.ss[r].y;
        enemy.life = (int)(1 + exp(square((double)rand() / (double)RAND_MAX) * log(2500.0 * (map + 1))));
        enemy.speed = (int)(3 + exp(square((double)rand() / (double)RAND_MAX) * log(101.0 - map - 1)));
        enemy.t = rand() % 30 + 1;
        mousou.levels[0].enemy.push_back(enemy);
      }
    }

    vector<pair<int, vector<TowerInfo> > > ans;
    int best = -1500000;
    const int ITER_CNT = 5;
    ans.resize(ITER_CNT);
    int bestMask[FS][FS];
    int expandT = 0;
    int simulatorT = 0;
    int t1 = timeGetTime();
    vector<MaskInfo> maskInfos = CalcBestMask(field, 150);
    int t2 = timeGetTime();
    maskInfos.resize(ITER_CNT);
    REP(iter, ITER_CNT) {
      calcCandidate = false;
      maskInfos[iter].money = -100000;
      int t3 = timeGetTime();
      ExpandMask(field, maskInfos[iter].mask, 150);
      int t4 = timeGetTime();
      //EraseUneedTower(field, bestMask);
      if (map > 10) {
        SetFrozenTower(field, maskInfos[iter].mask, 10);
      }
      int t5 = timeGetTime();
      maskInfos[iter].money = Simulation2(mousou, map, 0, maskInfos[iter].mask);
      int t6 = timeGetTime();
      expandT += t4 - t3;
      simulatorT += t6 - t5;
    }
    fprintf(stdout, "%d %d %d \n", t2 - t1, expandT, simulatorT);
    calcCandidate = false;
    sort(maskInfos.rbegin(), maskInfos.rend());
    memcpy(bestMask, maskInfos[0].mask, sizeof(int) * FS * FS);
    Simulation2(mapInfo, map, level, bestMask);
    //PrintMask(field, bestMask);
    //int t7 = timeGetTime();
    //PrintMask(field, bestMask);

    return MaskToTower(field, bestMask, mapInfo.levels[level].money);
  }
};
