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
	int field[51][51];
	int move[51][51];
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
	Field(const int f[51][51], int W, int H) {
    Create(f, W, H);
	}
  Field (const MapInfo &mapInfo) {
    Create(mapInfo.field, mapInfo.w, mapInfo.h);
  }

  void Create(const int f[51][51], int W, int H) {
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
			assert(0 <= it->type && it->type <= 2);
			if (field[it->y][it->x] == 's' || field[it->y][it->x] == 'g' || field[it->y][it->x] == '1') { return -1; }
      ret += it->Money();
      int ptype = field[it->y][it->x] / 1000 - 1;
      int plevel = field[it->y][it->x] % 1000 / 100 - 1;
      if (ptype == it->type) {
        ret -= TowerInfo(-1, -1, plevel, ptype).Money();
      }
			field[it->y][it->x] = (it->type + 1) * 1000 + (it->level + 1) * 100;
		}
    int mask[51][51];
    MEMSET(mask, 0);
		CalcMove(mask, move);
    return ret;
	}

  void CalcMove(const int mask[51][51], int move[51][51]) const {
		priority_queue<DistPoint> que;
		FORIT(it, gs) {
			que.push(DistPoint(it->x, it->y, 0, 0));
		}
		bool visit[51][51];
  	int dist[51][51];
		MEMSET(visit, false);
		MEMSET(dist, 0x0f);
    memset(move, 0x0f, sizeof(int) * 51 * 51);
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
				if (field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g' || mask[ny][nx]) { continue; }
				if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
				if (dir % 2 == 1) {
					if (field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g' || mask[ny][p.x]) { continue; }
					if (field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g' || mask[p.y][nx]) { continue; }
				}
				dist[ny][nx] = ndist;
				que.push(DistPoint(nx, ny, ndist, dir));
			}
		}
	}

  int CalcDist(const int mask[51][51]) const {
    bool visit[51][51];
    int dist[51][51];
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
				if ((field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') || mask[ny][nx] != 0) { continue; }
				if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
				if (dir % 2 == 1) {
					if ((field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g') || mask[ny][p.x] != 0) { continue; }
					if ((field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g') || mask[p.y][nx] != 0) { continue; }
				}
				dist[ny][nx] = ndist;
				que.push(DistPoint(nx, ny, ndist, dir));
      }
    }
    if (cnt != ss.size()) { return 1 << 28; }
    return ret;
  }

  int CalcDist2(const int mask[51][51], int dist[51][51]) const {
    bool visit[51][51];
    MEMSET(visit, false);
    memset(dist, 0x0f, sizeof(int) * 51 * 51);
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
				if ((field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') || mask[ny][nx] != 0) { continue; }
				if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
				if (dir % 2 == 1) {
					if ((field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g') || mask[ny][p.x] != 0) { continue; }
					if ((field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g') || mask[p.y][nx] != 0) { continue; }
				}
				dist[ny][nx] = ndist;
				que.push(DistPoint(nx, ny, ndist, dir));
      }
    }
    if (cnt != ss.size()) { return 1 << 28; }
    return ret;
  }

  bool dfs(const int mask[51][51], bool visit[51][51], int x, int y) const {
    visit[y][x] = true;
    bool ret = field[y][x] == 'g';
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    REP(dir, 4) {
      int nx = x + dx[dir];
      int ny = y + dy[dir];
      if (field[ny][nx] != 's' && field[ny][nx] != '0' && field[ny][nx] != 'g' || mask[ny][nx] != 0) { continue; }
      if (visit[ny][nx]) { continue; }
      ret |= dfs(mask, visit, nx, ny);
    }
    return ret;
  }

  bool OK2(const int mask[51][51]) const {
    bool visit[51][51];
    MEMSET(visit, false);
    REP(y, h) {
      REP(x, w) {
        if (visit[y][x]) { continue; }
        if (field[y][x] == 's') {
          if (!dfs(mask, visit, x, y)) { return false; }
        }
      }
    }
    return true;
  }

  bool OK(int mask[51][51], int bx, int by) const {
    if (mask[by][bx] != 0 || field[by][bx] != '0') { return false; }
    mask[by][bx] = 1;
    {
      int visit = 0;
      int cnt1 = 0;
      queue<Point> que;
      FOREQ(y, by - 1, by + 1) {
        FOREQ(x, bx - 1, bx + 1) {
          if ((field[y][x] != '0' && field[y][x] != 's' && field[y][x] != 'g') || mask[y][x] != 0) { continue; }
          cnt1++;
          if (que.empty()) {
            que.push(Point(x, y));
            visit |= 1 << ((y - by + 1) * 3 + (x - bx + 1));
          }
        }
      }
      int cnt2 = 0;
      const int dx[4] = { 1, 0, -1, 0 };
      const int dy[4] = { 0, 1, 0, -1 };
      while (!que.empty()) {
        Point p = que.front();
        que.pop();
        cnt2++;
        REP(dir, 4) {
          int nx = p.x + dx[dir];
          int ny = p.y + dy[dir];
          if (nx < bx - 1 || nx > bx + 1 || ny < by - 1 || ny > by + 1) { continue; }
          if ((field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') || mask[ny][nx] != 0) { continue; }
          if (visit & (1 << ((ny - by + 1) * 3 + (nx - bx + 1)))) { continue; }
          visit |= 1 << ((ny - by + 1) * 3 + (nx - bx + 1));
          que.push(Point(nx, ny));
        }
      }
      if (cnt1 == cnt2) { goto ok; }
    }
    if (!OK2(mask)) { goto ng; }
  ok:
    mask[by][bx] = 0;
    return true;
  ng:;
    mask[by][bx] = 0;
    return false;
  }

  void CalcEnemyRoute(const int mask[51][51], int route[51][51]) const {
    int move[51][51];
    CalcMove(mask, move);
    memset(route, 0, sizeof(int) * 51 * 51);
	  const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	  const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
    FORIT(it, ss) {
      int x = it->x;
      int y = it->y;
      while (field[y][x] != 'g') {
        route[y][x] |= 2;
        int nx = x + dx[move[y][x]];
        int ny = y + dy[move[y][x]];
        x = nx;
        y = ny;
      }
    }
  }

  void CalcSum(const int mask[51][51], int sums[51][51]) {
    int route[51][51];
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