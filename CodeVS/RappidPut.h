#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"
#include "Mask.h"
#include "Misc.h"

namespace RappidPut {
  struct Point {
    int x, y;
    double cost;
    Point() {;}
    Point(int x, int y, double cost) : x(x), y(y), cost(cost) {;}
    bool operator<(const Point &rhs) const {
      return cost > rhs.cost;
    }
  };

  vector<TowerInfo> Kimeuchi(int stage, int level) {
    vector<TowerInfo> ret;
    assert(stage == 0 || stage == 1);
    if (stage == 0) {
      if (level == 0) {
        ret.push_back(TowerInfo(2, 3, 3, 0));
      } else if (level == 2) {
        ret.push_back(TowerInfo(2, 4, 0, 0));
      } else if (level == 4) {
        ret.push_back(TowerInfo(2, 3, 4, 0));
      } else if (level == 5) {
        ret.push_back(TowerInfo(2, 4, 2, 0));
      } else if (level == 6) {
        ret.push_back(TowerInfo(2, 4, 3, 0));
      } else if (level == 7) {
        ret.push_back(TowerInfo(2, 4, 4, 0));
      }
    } else if (stage == 1) {
      if (level == 0) {
        ret.push_back(TowerInfo(2, 1, 4, 0));
        ret.push_back(TowerInfo(3, 3, 4, 0));
        ret.push_back(TowerInfo(3, 7, 4, 0));
        ret.push_back(TowerInfo(3, 4, 1, 0));
      }
    }
    return ret;
  }

  vector<TowerInfo> RappidPut(const MapInfo &mapInfo, int stage, int level, int mask[51][51]) {
    if (stage <= 1) { return Kimeuchi(stage, level); }
    Field field(mapInfo);
    vector<TowerInfo> tower = mapInfo.levels[level].tower;
    int money = mapInfo.levels[level].money;
    vector<TowerInfo> ret;

    int mapUse[40];
    REP(i, 40) { mapUse[i] = 1; }
    
    int w = field.w;
    int h = field.h;
    bool visit[51][51];
    //int mask[51][51];
    //MEMSET(mask, 0);
    REP(sy, h) {
      REP(sx, w) {
        if (field.field[sy][sx] != 's') { continue; }
        priority_queue<Point> que;
        que.push(Point(sx, sy, 0));
        MEMSET(visit, false);
        visit[sy][sx] = true;
        double cnt = 0;
        que = priority_queue<Point>();
        que.push(Point(sx, sy, 0));
        MEMSET(visit, false);
        visit[sy][sx] = true;
        while (!que.empty()) {
          Point p = que.top();
          que.pop();
          if (field.OK(mask, p.x, p.y)) {
            int level = 4;
            int type = 0;
            ret.push_back(TowerInfo(p.x, p.y, level, type));
            cnt++;
            mask[p.y][p.x] = 15;
            field.field[p.y][p.x] = 1500;
            break;
            if (cnt >= mapUse[stage]) { break; }
          }
          REP(dir, 8) {
            const int dx[8] = { 1, 0, -1, 0, 1, 1, -1, -1 };
            const int dy[8] = { 0, 1, 0, -1, 1, -1, 1, -1 };
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




  int RandomPut(const MapInfo &mapInfo, int map, int mask[51][51]) {
    Field field(mapInfo);
    while (true) {
      int x = rand() % field.w;
      int y = rand() % field.h;
      if (!field.OK(mask, x, y)) { continue; }
      mask[y][x] = 15;
      pair<int, int> result = Simulator::MapSimulation(mapInfo, map, MaskToTower(field, mask, mapInfo.levels[0].money));
      if (result.first == 0) { return result.second; }
    }
    assert(false);
    return -150000;
  }

  int SwapPut(const MapInfo &mapInfo, int map, int mask[51][51]) {
    Field field(mapInfo);
    Simulator simulator;
    simulator.stages.push_back(mapInfo);
    vector<TowerInfo> towers;
    int cnt = rand() % 11 - 3;
    {
      vector<TowerInfo> temp = RappidPut(mapInfo, map, 0, mask);
      random_shuffle(temp.begin(), temp.end());
      FORIT(it, temp) {
        if (cnt < 0) { cnt++; continue; }
        towers.push_back(*it);
        mask[it->y][it->x] = 15;
      }
    }
    REP(y, field.h) {
      REP(x, field.w) {
        if (cnt <= 0) { break; }
        if (!field.OK(mask, x, y)) { continue; }
        towers.push_back(TowerInfo(x, y, 4, 0));
        mask[y][x] = 15;
        cnt--;
      }
    }
    if ((int)towers.size() == 0) { return SwapPut(mapInfo, map, mask); }
    int best = -1500000;
    {
      pair<int, int> result = simulator.MapSimulation(mapInfo, 0, towers);
      result.second += -result.first * 5000;
      best = result.second;
    }

    int pmoney = best;
    int sums[51][51];
    field.CalcSum(mask, sums);

    vector<TowerInfo> bestTowers = towers;
    REP(iter, 10000) {
      if (towers.size() == 0) { break; }
      vector<TowerInfo> ptowers = towers;
      int pmask[51][51];
      memcpy(pmask, mask, sizeof(int) * 51 * 51);
      int r = rand() % 10;
      int money = -15000000;
      if (r <= 8) {
        // move
        int index = rand() % towers.size();
        int x = rand() % field.w;
        int y = rand() % field.h;
        if (sums[y][x] == 0) { continue; }
        if (!field.OK(mask, x, y)) { continue; }
        mask[towers[index].y][towers[index].x] = 0;
        mask[y][x] = 15;
        towers[index].x = x;
        towers[index].y = y;
        assert(field.OK2(mask));
      } else if (r <= 9) {
        // level down
        int index = rand() % towers.size();
        if (towers[index].level == 0) {
          mask[towers[index].y][towers[index].x] = 0;
          towers.erase(towers.begin() + index);
        } else {
          towers[index].level--;
          mask[towers[index].y][towers[index].x] = 11 + towers[index].level;
        }
        assert(field.OK2(mask));
      }
      pair<int, int> result = simulator.MapSimulation(mapInfo, map, MaskToTower(field, mask, mapInfo.levels[0].money), 1);
      result.second += -result.first * 5000;
      money = result.second;
      if (money > best) {
        best = money;
        bestTowers = towers;
      }
      if (money > pmoney || rand() % 300 > iter * 3 + 150) {
        pmoney = money;
      } else {
        memcpy(mask, pmask, sizeof(int) * 51 * 51);
        towers = ptowers;
      }
    }
    if (!field.OK2(mask)) { best = -1500000; }
    return best;
  }

  void Tsubusu(const MapInfo &mapInfo, int mask[51][51]) {
    memset(mask, 0, sizeof(int) * 51 * 51);
    Field field(mapInfo);
    int cnt = rand() % field.gs.size();
    if (rand() % 2) { cnt = 0; }
    vector<Field::Point> gs = field.gs;
    random_shuffle(gs.begin(), gs.end());
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    REP(i, cnt) {
      REP(dir, 4) {
        int nx = gs[i].x + dx[dir];
        int ny = gs[i].y + dy[dir];
        if (!field.OK(mask, nx, ny)) { continue; }
        mask[ny][nx] = 11;
      }
    }
    if (!field.OK2(mask)) { Tsubusu(mapInfo, mask); }
  }

  vector<TowerInfo> RappidPut2(const MapInfo &mapInfo, int stage, int level, bool save) {
    if (stage == 0 || stage == 1) { return Kimeuchi(stage, level); }
    if (level != 0) { return vector<TowerInfo>(); }
    int best = -15000;
    const int ITER_CNT = 12;
    vector<pair<int, vector<TowerInfo> > > ans(ITER_CNT, make_pair(best, vector<TowerInfo>()));
    {
      int mask[51][51];
      memset(mask, 0, sizeof(int) * 51 * 51);
      vector<TowerInfo> towers = RappidPut(mapInfo, stage, level, mask);
      pair<int, int> result = Simulator::MapSimulation(mapInfo, stage, towers);
      //printf("-1 %d %d\n", result.first, result.second);
      result.second += -result.first * 400;
      ans.push_back(make_pair(result.second, towers));
    }
#pragma omp parallel for
    REP(iter, ITER_CNT) {
      Field field(mapInfo);
      int mask[51][51];
      MEMSET(mask, 0);
      Tsubusu(mapInfo, mask);
      int money = SwapPut(mapInfo, stage, mask);
      ans[iter] = make_pair(money, MaskToTower(field, mask, mapInfo.levels[0].money));
      best = max(best, money);
      //printf("%d %d\n", iter, money);
    }

    sort(ans.rbegin(), ans.rend());
    if (save) {
      REP(i, min((int)ans.size(), 5)) {
        char filename[100];
        sprintf(filename, "replay/%02d.txt", stage);
        SaveMask(filename, mapInfo, ans[i].second, ans[i].first, false);
      }
    }
    return ans[0].second;
  }

  bool OKMask(const MapInfo &mapInfo, int mask[51][51]) {
    Field field(mapInfo);
    if (field.OK2(mask)) { return true; }
    const int dx[8] = { 1, 0, -1, 0, 1, 1, -1, -1 };
    const int dy[8] = { 0, 1, 0, -1, 1, -1, 1, -1 };
    REP(sy, field.h) {
      REP(sx, field.w) {
        if (mask[sy][sx] == 0 || field.field[sy][sx] == '0') { continue; }
        bool visit[51][51];
        MEMSET(visit, false);
        int v = mask[sy][sx];
        mask[sy][sx] = 0;
        visit[sy][sx] = true;
        queue<pair<int, int> > que;
        que.push(make_pair(sx, sy));
        while (!que.empty()) {
          int x = que.front().first;
          int y = que.front().second;
          if (field.field[y][x] == '0') {
            mask[y][x] = v;
            break;
          }
          REP(dir, 8) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx < 0 || nx > field.w || ny < 0 || ny >= field.h) { continue; }
            if (visit[ny][nx]) { continue; }
            visit[ny][nx] = true;
            que.push(make_pair(nx, ny));
          }
        }
      }
    }
    return field.OK2(mask);
  }

  vector<TowerInfo> ReplayAttack(const MapInfo &mapInfo, const int map, const int level) {
    if (map == 0 || map == 1) { return Kimeuchi(map, level); }
    if (level != 0) { return vector<TowerInfo>(); }
    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
    char filename[100];
    sprintf(filename, "replay/%02d.txt", map);
    vector<MaskInfo> answer = LoadMasks(filename, mapInfo);
    if (answer.size() == 0) { return RappidPut2(mapInfo, map, level, false); }
    //fprintf(stderr, "Size: %d\n", answer.size());
    int best = -15000;
    int upper = (int)answer.size();
    vector<pair<int, vector<TowerInfo> > > ans(upper);
    REP(i, upper) { ans[i].first = -150000; }
    {
      int mask[51][51];
      memset(mask, 0, sizeof(int) * 51 * 51);
      vector<TowerInfo> towers = RappidPut(mapInfo, map, level, mask);
      pair<int, int> result = Simulator::MapSimulation(mapInfo, map, towers, 1);
      result.second += -result.first * 500;
      ans.push_back(make_pair(result.second, towers));
      best = result.second;
    }
#pragma omp parallel for
    for (int i = 0; i < upper; i++) {
      if (answer[i].money <= best) { continue; }
      MaskInfo temp = answer[i];
      if (!OKMask(mapInfo, temp.mask)) { continue; }
      vector<TowerInfo> towers = MaskToTower(field, answer[i].mask, mapInfo.levels[0].money);
      pair<int, int> result = Simulator::MapSimulation(mapInfo, map, towers);
      result.second += -result.first * 5000;
      best = max(best, result.second);
      ans[i] = make_pair(result.second, towers);
    }
    sort(ans.rbegin(), ans.rend());
    ans[0].second = LevelDown(mapInfo, map, ans[0].second, 0);

    return ans[0].second;
  }
}