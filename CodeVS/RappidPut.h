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


  vector<TowerInfo> RappidPut(const MapInfo &mapInfo, int stage, int level) {
    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
    vector<TowerInfo> tower = mapInfo.levels[level].tower;
    int money = mapInfo.levels[level].money;
    vector<TowerInfo> ret;
    if (stage == 0) {
      if (level == 0) {
        ret.push_back(TowerInfo(2, 3, 3, 0));
      } else if (level == 5) {
        ret.push_back(TowerInfo(2, 3, 4, 0));
      } else if (level == 10) {
        ret.push_back(TowerInfo(2, 2, 0, 0));
        ret.push_back(TowerInfo(2, 4, 0, 0));
      }
      return ret;
    }
    if (stage == 1) {
      if (level == 1) {
        ret.push_back(TowerInfo(3, 1, 0, 0));
      }
    }
    if (!tower.empty()) { return ret; }

    int mapUse[40];
    REP(i, 40) { mapUse[i] = 1; }
    //mapUse[12] = 2;
    //mapUse[18] = 2;
    //mapUse[26] = 2;
    //mapUse[35] = 2;
    //mapUse[37] = 2;
    //mapUse[38] = 2;

    int w = field.w;
    int h = field.h;
    bool visit[51][51];
    int mask[51][51];
    MEMSET(mask, 0);
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

//  vector<TowerInfo> ReplayAttack(const MapInfo &mapInfo, const int map, const int level) {
//    if (level != 0) { return vector<TowerInfo>(); }
//    Field field(mapInfo.field, mapInfo.w, mapInfo.h);
//    char filename[100];
//    sprintf(filename, "replay/%02d.txt", map);
//    vector<MaskInfo> answer = LoadMasks(filename, mapInfo);
//    if (answer.size() == 0) { return RappidPut(mapInfo, map, level, false); }
//    //fprintf(stderr, "Size: %d\n", answer.size());
//    int best = -15000;
//    int upper = (int)answer.size();
//    vector<pair<int, vector<TowerInfo> > > ans(upper * 2);
//    REP(i, upper * 2) { ans[i].first = -150000; }
//#pragma omp parallel for
//    for (int i = 0; i < upper; i++) {
//      if (answer[i].money <= best) { continue; }
//      MaskInfo temp = answer[i];
//      int money = Simulation(mapInfo, map, answer[i].mask, best);
//      best = max(best, money);
//      ans[i] = make_pair(money, MaskToTower(field, answer[i].mask, mapInfo.levels[0].money));
//
//      SetFrozenTower(field, temp.mask, 10);
//      money = Simulation(mapInfo, map, temp.mask, best);
//      ans[i + upper] = make_pair(money, MaskToTower(field, temp.mask, mapInfo.levels[0].money));
//    }
//    sort(ans.rbegin(), ans.rend());
//    ans[0].second = LevelDown(mapInfo, map, ans[0].second);
//    ans[0].first = CalcMoney(ans[0].second);
//
//    //return ans[0].second;
//    return LifeToMoney(mapInfo, map, ans[0].second, ans[0].first);
//  }
}