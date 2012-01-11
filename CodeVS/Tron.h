#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Simulator.h"
#include "Mask.h"
#include "Misc.h"

namespace SecondHalf {
  const int dx[4] = { 1, 0, -1, 0 };
  const int dy[4] = { 0, 1, 0, -1 };

  int Put(const Field &field, int mask[FS][FS], int x, int y, int &use, int probability) {
    if (field.field[y][x] != '0') { return use; }
    if (mask[y][x] != 0) { return use; }
    mask[y][x] = 11;
    if (rand() % 101 < probability) {
      mask[y][x] = 31;
    }
    use++;
    return use;
  }


  int EraseUneedTower(const Field &field, int bestMask[FS][FS]) {
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

  int ExpandMask(const Field &field, int bestMask[FS][FS], int useCnt) {
    const int h = field.h;
    const int w = field.w;
    int use = CalcUse(field, bestMask);
    int bestDist = field.CalcDist(bestMask);
    bool ban[FS][FS];
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
            //} else {
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


  int CalcMask(int iter, const Field &field, int mask[FS][FS], int useCnt, int probability) {
    int dist = 0;
    int use = 0;
    int target = rand() % field.gs.size();
    memset(mask, 0, sizeof(int) * FS * FS);
    //targetˆÈŠO‚ð–„‚ß‚é
    REP(i, field.gs.size()) {
      if (i == target) { continue; }
      REP(dir, 4) {
        int nx = field.gs[i].x + dx[dir];
        int ny = field.gs[i].y + dy[dir];
        Put(field, mask, nx, ny, use, 0);
      }
    }
    if (!field.OK2(mask)) { return CalcMask(iter, field, mask, useCnt, probability); }

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
        Put(field, mask, nnx, nny, use, probability);
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

  int CalcBestMask(const Field &field, int bestMask[FS][FS], int useCnt, int probability) {
    int bestDist = -1;
    memset(bestMask, 0x0f, sizeof(int) * FS * FS);
    REP(iter, 200) {
      int mask[FS][FS];
      int dist = CalcMask(rand() % 3600, field, mask, useCnt, probability);
      // best‚ðXV
      if (dist > bestDist) {
        memcpy(bestMask, mask, sizeof(int) * FS * FS);
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

  int Simulation(const MapInfo &mapInfo, const int map, int mask[FS][FS], int best) {
    const int w = mapInfo.w;
    const int h = mapInfo.h;
    Simulator simulator;
    simulator.stages.push_back(mapInfo);

    int iniMask[FS][FS];
    memcpy(iniMask, mask, sizeof(int) * FS * FS);

    int sums[FS][FS];
    MEMSET(sums, 0);
    Field field(mapInfo.field, w, h);
    field.CalcSum(mask, sums);

    int ret = 0;
    int lastLevel = 0;
    while (true) {
      vector<TowerInfo> tower = MaskToTower(field, mask, mapInfo.levels[0].money);
      pair<int, int> ans = simulator.LevelSimulation(false, 0, lastLevel, tower);
      if ((lastLevel == 24 && ans.first == 0) || ret + ans.second < best) { break; }
      if (ans.first == 0) {
        lastLevel++;
        ret += ans.second;
        field.PutTower(tower);
        continue;
      }

      int bestSum = -1;
      int tx = -1;
      int ty = -1;
      REP(iter, ans.first) {
        REP(sy, h) {
          REP(sx, w) {
            if (mask[sy][sx] >= 15) { continue; }
            if (mask[sy][sx] == 0 && !field.OK(mask, sx, sy)) { continue; }
            int sum = sums[sy][sx];
            if (sum > bestSum) {
              bestSum = sum;
              tx = sx;
              ty = sy;
            }
          }
        }
        if (bestSum == -1) { goto end; }
        mask[ty][tx] = 15;
      }
      //PrintMask(field, mask);
    }
end:
    field = Field(mapInfo.field, w, h);
    vector<TowerInfo> tower = MaskToTower(field, mask, mapInfo.levels[0].money);
    pair<int, int> ans = simulator.MapSimulation(false, 0, tower);
    ans.second += -ans.first * 5000;
    return ans.second;
  }

  vector<TowerInfo> TronAI(const MapInfo &mapInfo, const int map, const int level, bool save, int useCnt = -1, int useFrozenCnt = -1) {
    if (level != 0) { return vector<TowerInfo>(); }
    int mapUse[80];
    REP(i, 80) { mapUse[i] = 150; }
    mapUse[40]= 24;
    mapUse[41]= 42;
    mapUse[42]= 51;
    mapUse[43]= 82;
    mapUse[44]= 49;
    mapUse[45]= 78;
    mapUse[46]= 48;
    mapUse[47]=142;
    mapUse[48]= 59;
    mapUse[49]=110;
    mapUse[50]= 69;
    mapUse[51]= 98;

    if (useCnt != -1) {
      mapUse[map] = useCnt;
    }

    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
    int best = -1500000;
    vector<pair<int, vector<TowerInfo> > > ans;
    const int ITER_CNT = 100;
    ans.resize(ITER_CNT);
#pragma omp parallel for
    REP(iter, ITER_CNT) {
      int useCnt = mapUse[map] + rand() % 151 - 50;
      int bestMask[FS][FS];
      //int t1 = timeGetTime();
      CalcBestMask(field, bestMask, useCnt, 0);
      //PrintMask(field, bestMask);
      //int t2 = timeGetTime();
      EraseUneedTower(field, bestMask);
      //PrintMask(field, bestMask);
      //int t3 = timeGetTime();
      ExpandMask(field, bestMask, useCnt);
      //PrintMask(field, bestMask);
      //int t4 = timeGetTime();
      EraseUneedTower(field, bestMask);
      //PrintMask(field, bestMask);
      //int t5 = timeGetTime();
      SetFrozenTower(field, bestMask, (iter & 1) * (10 + rand() % 5));
      //PrintMask(field, bestMask);
      //int t6 = timeGetTime();
      int money = Simulation(mapInfo, map, bestMask, best);
      //int t7 = timeGetTime();
      //fprintf(stderr, "%d %d %d %d %d %d\n", t2 - t1, t3 - t2, t4 - t3, t5 - t4, t6 - t5, t7 - t6);
      //PrintMask(field, bestMask);
      ans[iter] = make_pair(money, MaskToTower(field, bestMask, mapInfo.levels[0].money));
      best = max(best, money);
    }
    sort(ans.rbegin(), ans.rend());
    if (save) {
      REP(i, min((int)ans.size(), 10)) {
        char filename[100];
        sprintf(filename, "replay/%02d.txt", map);
        SaveMask(filename, mapInfo, ans[i].second, ans[i].first, true);
      }
    }

    return ans[0].second;
  }

  vector<TowerInfo> LifeToMoney(const MapInfo &mapInfo, const int map, const vector<TowerInfo> iniTowers, int iniMoney) {
    bool ok[80];
    MEMSET(ok, false);
    ok[57] = true;
    ok[59] = true;
    ok[60] = true;
    ok[61] = true;
    ok[62] = true;
    ok[66] = true;
    ok[67] = true;
    ok[68] = true;
    ok[70] = true;
    ok[71] = true;
    ok[72] = true;
    ok[73] = true;
    ok[75] = true;
    ok[77] = true;
    ok[78] = true;
    const int last = 78;

    if (mapInfo.levels[0].life == 1 || !ok[map]) { return LevelDown(mapInfo, map, iniTowers, 0); }
    vector<int> iniTarget;
    REP(i, iniTowers.size()) {
      if (iniTowers[i].level >= 1 && iniTowers[i].type == 0) {
        iniTarget.push_back(i);
      }
    }
    const int threshold = 400;
    int best = iniMoney + threshold;
    if (map == last) { best = iniMoney; }
    int ITER_CNT = 500;
    vector<pair<int, vector<TowerInfo> > > ans(ITER_CNT);
    FORIT(it, ans) { *it = make_pair(best, iniTowers); }
    ans.push_back(make_pair(best, iniTowers));

    vector<bool> end(ITER_CNT);
    vector<int> moneys(ITER_CNT, iniMoney);
    vector<vector<TowerInfo> > towers(ITER_CNT, iniTowers);
    vector<vector<int> > target(ITER_CNT, iniTarget);
    REP(iter, ITER_CNT) {
      random_shuffle(target[iter].begin(), target[iter].end());
    }
    int bestDamage = 0;
#pragma omp parallel for
    REP(iter, ITER_CNT) {
      REP(i, target[0].size()) {
        //if (i >= 4 && best == iniMoney + threshold) { continue; }
        if (i >= 20) { continue; }
        //if (end[iter]) { continue; }
        moneys[iter] += towers[iter][target[iter][i]].Money();
        towers[iter][target[iter][i]].level = 0;
        moneys[iter] -= towers[iter][target[iter][i]].Money();
        if (moneys[iter] > best) {
          pair<int, int> result = Simulator::MapSimulation(false, mapInfo, 0, towers[iter], mapInfo.levels[0].life);
          //pair<int, int> result = Simulator::MapSimulation(false, mapInfo, 0, towers[iter]);
          //if (result.first >= 10) { break; }
          if (result.first >= mapInfo.levels[0].life) {
            end[iter] = true;
            break;
          }
          if (result.second > best && (result.second > iniMoney + result.first * threshold || map == last)) {
            best = result.second;
            bestDamage = result.first;
            ans[iter] = make_pair(result.second, towers[iter]);
          }
        }
      }
    }
    sort(ans.rbegin(), ans.rend());
    return ans[0].second = LevelDown(mapInfo, map, ans[0].second, bestDamage);
  }

  vector<TowerInfo> ReplayAttack(const MapInfo &mapInfo, const int map, const int level) {
    if (level != 0) { return vector<TowerInfo>(); }
    Field field(mapInfo);
    char filename[100];
    sprintf(filename, "replay/%02d.txt", map);
    vector<MaskInfo> answer = LoadMasks(filename, mapInfo);
    if (answer.size() == 0) { return TronAI(mapInfo, map, level, false); }
    //fprintf(stderr, "Size: %d\n", answer.size());
    int best = -15000;
    int upper = (int)answer.size();
    vector<pair<int, vector<TowerInfo> > > ans(upper);
    REP(i, upper) { ans[i].first = -150000; }
#pragma omp parallel for
    for (int i = 0; i < upper; i++) {
      if (answer[i].money <= best - 50) { continue; }
      MaskInfo temp = answer[i];
      int money = Simulation(mapInfo, map, answer[i].mask, best);
      best = max(best, money);
      ans[i] = make_pair(money, MaskToTower(field, answer[i].mask, mapInfo.levels[0].money));
    }
    sort(ans.rbegin(), ans.rend());
    field.PutTower(ans[0].second);
    int mask[FS][FS];
    MEMSET(mask, 0);
    //PrintMask(field, mask);
    //ans[0].second = LifeToMoney(mapInfo, map, ans[0].second, ans[0].first);
    //ans[0].second = LevelDown(mapInfo, map, ans[0].second, 0);
    //ans[0].first = -CalcMoney(ans[0].second);

    //return ans[0].second;
    return ans[0].second;
  }
};
