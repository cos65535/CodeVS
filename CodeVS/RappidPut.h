#pragma once

#include "Base.h"
#include "Structure.h"
#include "Field.h"

struct Point {
  int x, y;
  double cost;
  Point() {;}
  Point(int x, int y, double cost) : x(x), y(y), cost(cost) {;}
  bool operator<(const Point &rhs) const {
    return cost > rhs.cost;
  }
};


vector<TowerInfo> RappidPut(Field field, int stage, int level, vector<TowerInfo> &tower, int money) {
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
      while (!que.empty()) {
        Point p = que.top();
        que.pop();
        if (p.cost >= 5) { break; }
        if (field.field[p.y][p.x] == 'P') {
          cnt++;
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

      //if (cnt >= 2) { continue; }
      que = priority_queue<Point>();
      que.push(Point(sx, sy, 0));
      MEMSET(visit, false);
      visit[sy][sx] = true;
      cnt = 0;
      while (!que.empty()) {
        Point p = que.top();
        que.pop();
        if (field.OK(mask, p.x, p.y)) {
          int level = 4;
          //if (cnt == 1) { level = 0; }
          int type = 0;
//          if ((int)cnt % 4 == 2) { type = 2; }
          ret.push_back(TowerInfo(p.x, p.y, level, type));
          cnt++;
          field.field[p.y][p.x] = 'P';
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