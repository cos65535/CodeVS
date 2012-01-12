#pragma once

#include "Base.h"
#include "Structure.h"

struct Field {
  private:
    struct DistPoint {
      int x;
      int y;
      int dist;
      int dir;
      DistPoint() {;}
      DistPoint(int x, int y, int dist, int dir) : x(x), y(y), dist(dist), dir(dir) {;}
      bool operator<(const DistPoint &rhs) const {
        if (dist != rhs.dist) { return dist > rhs.dist; }
        return dir > rhs.dir;
      }
    };
  public:

    int w;
    int h;
    int field[FS][FS];
    int move[FS][FS];
    struct Point {
      int x;
      int y;
      Point() : x(0xdeadbeaf), y(0xdeadbeaf) {;}
      Point(int x, int y) : x(x), y(y) {;}
    };
    vector<Point> ss;
    vector<Point> gs;
    Field() {;}
    Field(const Field &rhs) {
      this->w = rhs.w;
      this->h = rhs.h;
      memcpy(this->field, rhs.field, sizeof(this->field));
      memcpy(this->move, rhs.move, sizeof(this->move));
    }
    Field &operator=(const Field &rhs) {
      this->w = rhs.w;
      this->h = rhs.h;
      memcpy(this->field, rhs.field, sizeof(this->field));
      memcpy(this->move, rhs.move, sizeof(this->move));
      return *this;
    }
    Field(const int f[FS][FS], int W, int H) {
      Create(f, W, H);
    }
    Field (const MapInfo &mapInfo) {
      Create(mapInfo.field, mapInfo.w, mapInfo.h);
    }

    void Create(const int f[FS][FS], int W, int H) {
      w = W;
      h = H;
      memcpy(field, f, sizeof(field));
      MEMSET(move, 0x0f);
      ss.clear();
      gs.clear();
      REP(y, h) {
        REP(x, w) {
          if (field[y][x] == (int)'s') {
            ss.push_back(Point(x, y));
          }
          if (field[y][x] == (int)'g') {
            gs.push_back(Point(x, y));
          }
        }
      }
    }

    int PutTower(const vector<TowerInfo> &tower) {
      int ret = 0;
      FORIT(it, tower) {
        assert(0 <= it->type && it->type <= 3);
        if (field[it->y][it->x] == 's' || field[it->y][it->x] == 'g' || field[it->y][it->x] == '1') { return -1; }
        if (it->type == 3) {
          //cout << it->x << " " << it->y << " " << field[it->y][it->x] << endl;
          assert(field[it->y][it->x] >= 1100);
          field[it->y][it->x] = '0';
          continue;
        }
        ret += it->Money();
        int ptype = field[it->y][it->x] / 1000 - 1;
        int plevel = field[it->y][it->x] % 1000 / 100 - 1;
        if (ptype == it->type) {
          ret -= TowerInfo(-1, -1, plevel, ptype).Money();
        }
        field[it->y][it->x] = (it->type + 1) * 1000 + (it->level + 1) * 100;
      }
      int mask[FS][FS];
      MEMSET(mask, 0);
      CalcMove(mask, move);
      return ret;
    }

    void CalcMove(const int mask[FS][FS], int move[FS][FS]) const {
      priority_queue<DistPoint> que;
      FORIT(it, gs) {
        que.push(DistPoint(it->x, it->y, 0, 0));
      }
      bool visit[FS][FS];
      int dist[FS][FS];
      MEMSET(visit, false);
      MEMSET(dist, 0x0f);
      memset(move, 0x0f, sizeof(int) * FS * FS);
      const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
      const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
      while (!que.empty()) {
        DistPoint p = que.top();
        que.pop();
        if (visit[p.y][p.x]) { continue; }
        visit[p.y][p.x] = true;
        move[p.y][p.x] = p.dir;
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          int ndist = p.dist + 2 + dir % 2;
          if (mask[ny][nx] || (field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g')) { continue; }
          if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
          if (dir % 2 == 1) {
            if (mask[ny][p.x] || (field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g')) { continue; }
            if (mask[p.y][nx] || (field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g')) { continue; }
          }
          dist[ny][nx] = ndist;
          que.push(DistPoint(nx, ny, ndist, dir));
        }
      }
    }

    int CalcDist(const int mask[FS][FS]) const {
      bool visit[FS][FS];
      int dist[FS][FS];
      MEMSET(visit, false);
      MEMSET(dist, 0x0f);
      priority_queue<DistPoint> que;
      FORIT(it, gs) {
        que.push(DistPoint(it->x, it->y, 0, 0));
      }
      int cnt = 0;
      int ret = 1 << 28;
      const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
      const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
      while (!que.empty()) {
        DistPoint p = que.top();
        que.pop();
        if (visit[p.y][p.x]) { continue; }
        visit[p.y][p.x] = true;
        if (field[p.y][p.x] == 's') {
          return p.dist;
          ret = min(ret, p.dist);
          cnt++;
        }
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          int ndist = p.dist + 10 + (dir & 1) * 4;
          if (mask[ny][nx] != 0 || (field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g')) { continue; }
          if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
          if (dir % 2 == 1) {
            if (mask[ny][p.x] != 0 || (field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g')) { continue; }
            if (mask[p.y][nx] != 0 || (field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g')) { continue; }
          }
          dist[ny][nx] = ndist;
          que.push(DistPoint(nx, ny, ndist, dir));
        }
      }
      if (cnt != (int)ss.size()) { return 1 << 28; }
      return ret;
    }

    int CalcDist2(const int mask[FS][FS], int dist[FS][FS]) const {
      bool visit[FS][FS];
      MEMSET(visit, false);
      memset(dist, 0x0f, sizeof(int) * FS * FS);
      priority_queue<DistPoint> que;
      FORIT(it, gs) {
        que.push(DistPoint(it->x, it->y, 0, 0));
      }
      int cnt = 0;
      int ret = 1 << 28;
      const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
      const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
      while (!que.empty()) {
        DistPoint p = que.top();
        que.pop();
        if (visit[p.y][p.x]) { continue; }
        visit[p.y][p.x] = true;
        if (field[p.y][p.x] == 's') {
          return p.dist;
          ret = min(ret, p.dist);
          cnt++;
        }
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          int ndist = p.dist + 10 + (dir & 1) * 4;
          if (mask[ny][nx] != 0 || (field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g')) { continue; }
          if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
          if (dir % 2 == 1) {
            if (mask[ny][p.x] != 0 || (field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g')) { continue; }
            if (mask[p.y][nx] != 0 || (field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g')) { continue; }
          }
          dist[ny][nx] = ndist;
          que.push(DistPoint(nx, ny, ndist, dir));
        }
      }
      if (cnt != (int)ss.size()) { return 1 << 28; }
      return ret;
    }

    ll CalcDist3(const int mask[FS][FS]) const {
      bool visit[FS][FS];
      int dist[FS][FS];
      MEMSET(visit, false);
      MEMSET(dist, 0x0f);
      priority_queue<DistPoint> que;
      FORIT(it, gs) {
        que.push(DistPoint(it->x, it->y, 0, 0));
      }
      int cnt = 0;
      ll ret = 0;
      ll base = 1LL << 50;
      const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
      const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
      while (!que.empty()) {
        DistPoint p = que.top();
        que.pop();
        if (visit[p.y][p.x]) { continue; }
        visit[p.y][p.x] = true;
        if (field[p.y][p.x] == 's') {
          ret += p.dist * base;
          base /= 4000;
          cnt++;
          if (cnt == (int)ss.size() || base == 0) { break; }
        }
        REP(dir, 8) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          int ndist = p.dist + 10 + (dir & 1) * 4;
          if (mask[ny][nx] != 0 || field[ny][nx] == '1') { continue; }
          if (dist[ny][nx] < ndist) { continue; }
          if (dir & 1) {
            if (mask[ny][p.x] != 0 || field[ny][p.x] == '1') { continue; }
            if (mask[p.y][nx] != 0 || field[p.y][nx] == '1') { continue; }
          }
          dist[ny][nx] = ndist;
          que.push(DistPoint(nx, ny, ndist, dir));
        }
      }
      return ret;
    }

    bool OK2(const int mask[FS][FS]) const {
      bool visit[FS][FS];
      MEMSET(visit, false);
      const int dx[4] = { 1, 0, -1, 0 };
      const int dy[4] = { 0, 1, 0, -1 };
      REP(y, h) {
        REP(x, w) {
          if (visit[y][x]) { continue; }
          if (field[y][x] == 's') {
            queue<int> que;
            que.push((y << 10) | x);
            bool ok = false;
            visit[y][x] = true;
            while (!que.empty()) {
              int x = que.front() & 63;
              int y = que.front() >> 10;
              que.pop();
              if (field[y][x] == 'g') { ok = true; }
              REP(dir, 4) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (mask[ny][nx] != 0 || (field[ny][nx] != 's' && field[ny][nx] != '0' && field[ny][nx] != 'g')) { continue; }
                if (visit[ny][nx]) { continue; }
                visit[ny][nx] = true;
                que.push((ny << 10) | nx);
              }
            }
            if (!ok) { return false; }
          }
        }
      }
      return true;
    }

    inline bool Movable(const int mask[FS][FS], int x, int y) const {
      return !(x < 0 || x >= w || y < 0 || y >= h || field[y][x] == '1' || field[y][x] >= 1100 || mask[y][x] > 0);
    }
    bool OK(int mask[FS][FS], int bx, int by) const {
      if (mask[by][bx] != 0 || field[by][bx] != '0') { return false; }
      mask[by][bx] = 1;
      {
        int cnt = 0;
        int bits = 0;
        REP(dir, 4) {
          const int dx[4] = { 1, 0, -1, 0 };
          const int dy[4] = { 0, 1, 0, -1 };
          int nx = bx + dx[dir];
          int ny = by + dy[dir];
          bits |= !Movable(mask, nx, ny) << dir;
          if ((bits >> dir) & 1) {
            cnt++;
          }
        }
        if (cnt >= 3) {
          goto ok;
        } else if (cnt == 2) {
          if (bits == 5 || bits == 10) {;}
          else if ((bits == 3  && Movable(mask, bx - 1, by - 1)) ||
                   (bits == 6  && Movable(mask, bx + 1, by - 1)) ||
                   (bits == 12 && Movable(mask, bx + 1, by + 1)) ||
                   (bits == 9  && Movable(mask, bx - 1, by + 1))) {
            goto ok;
          }
        } else if (cnt == 1) {
          if ((bits == 1 && Movable(mask, bx - 1, by + 1) && Movable(mask, bx - 1, by - 1)) ||
              (bits == 2 && Movable(mask, bx - 1, by - 1) && Movable(mask, bx + 1, by - 1)) ||
              (bits == 4 && Movable(mask, bx + 1, by - 1) && Movable(mask, bx + 1, by + 1)) ||
              (bits == 8 && Movable(mask, bx + 1, by + 1) && Movable(mask, bx - 1, by + 1))) {
            goto ok;
          }
        } else {
          REP(dir, 4) {
            const int dx[4] = { 1, 1, -1, -1 };
            const int dy[4] = { 1, -1, 1, -1 };
            int nx = bx + dx[dir];
            int ny = by + dy[dir];
            cnt += Movable(mask, nx, ny);
          }
          if (cnt >= 3) { goto ok; }
        }
      }

      {
        int visit = 0;
        int cnt1 = 0;
        queue<Point> que;
        FOREQ(y, by - 1, by + 1) {
          FOREQ(x, bx - 1, bx + 1) {
            if (bx != x && by != y) { continue; }
            if ((field[y][x] != '0' && field[y][x] != 's' && field[y][x] != 'g') || mask[y][x] != 0) { continue; }
            cnt1++;
            if (que.empty()) {
              que.push(Point(x, y));
              visit |= 1 << ((y - by + 2) * 5 + (x - bx + 2));
            }
          }
        }
        int cnt2 = 0;
        const int dx[4] = { 1, 0, -1, 0 };
        const int dy[4] = { 0, 1, 0, -1 };
        while (!que.empty()) {
          Point p = que.front();
          que.pop();
          if ((p.x == bx || p.y == by) && (abs(p.x - bx) == 1 || abs(p.y - by) == 1)) {
            cnt2++;
          }
          REP(dir, 4) {
            int nx = p.x + dx[dir];
            int ny = p.y + dy[dir];
            if (nx < bx - 2 || nx > bx + 2 || ny < by - 2 || ny > by + 2) { continue; }
            if ((field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') || mask[ny][nx] != 0) { continue; }
            if (visit & (1 << ((ny - by + 2) * 5 + (nx - bx + 2)))) { continue; }
            visit |= 1 << ((ny - by + 2) * 5 + (nx - bx + 2));
            que.push(Point(nx, ny));
          }
        }
        if (cnt1 == cnt2) { goto ok; }
      }

      if (!OK2(mask)) { goto ng; }
ok:
      //assert(OK2(mask));
      mask[by][bx] = 0;
      return true;
ng:;
     mask[by][bx] = 0;
     return false;
    }

    void CalcEnemyRoute(const int mask[FS][FS], int route[FS][FS]) const {
      int move[FS][FS];
      CalcMove(mask, move);
      memset(route, 0, sizeof(int) * FS * FS);
      const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
      const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
      FORIT(it, ss) {
        int x = it->x;
        int y = it->y;
        while (field[y][x] != 'g') {
          route[y][x]++;
          int nx = x + dx[move[y][x]];
          int ny = y + dy[move[y][x]];
          x = nx;
          y = ny;
        }
      }
    }

    void CalcSum(const int mask[FS][FS], int sums[FS][FS]) const {
      int route[FS][FS];
      CalcEnemyRoute(mask, route);
      memset(sums, 0, sizeof(sums));
      REP(sy, h) {
        REP(sx, w) {
          int sum = 0;
          REP(y, h) {
            REP(x, w) {
              int d = square(sy - y) + square(sx - x);
              if (d > square(4 + 4)) { continue; }
              sum += route[y][x];
            }
          }
          sums[sy][sx] = sum;
        }
      }
    }
};
