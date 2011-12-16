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
	int dist[51][51];
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
    memcpy(this->dist, rhs.dist, sizeof(this->dist));
  }
  Field &operator=(const Field &rhs) {
    this->w = rhs.w;
    this->h = rhs.h;
    memcpy(this->field, rhs.field, sizeof(this->field));
    memcpy(this->move, rhs.move, sizeof(this->move));
    memcpy(this->dist, rhs.dist, sizeof(this->dist));
    return *this;
  }
	Field(const int f[51][51], int w, int h) : w(w), h(h) {
		memcpy(field, f, sizeof(field));
		MEMSET(move, 0x0f);
		MEMSET(dist, 0x0f);
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
		CalcMove();
    return ret;
	}
	void CalcMove() {
		priority_queue<DistPoint> que;
		FORIT(it, gs) {
			que.push(DistPoint(it->x, it->y, 0, 0));
		}
		bool visit[51][51];
		MEMSET(visit, false);
		MEMSET(dist, 0x0f);
    MEMSET(move, 0x0f);
		while (!que.empty()) {
			DistPoint p = que.top();
			que.pop();
			if (visit[p.y][p.x]) { continue; }
			visit[p.y][p.x] = true;
			move[p.y][p.x] = p.dir;
			REP(dir, 8) {
			  const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
			  const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				int nx = p.x + dx[dir];
				int ny = p.y + dy[dir];
				int ndist = p.dist + 2 + dir % 2;
				if (field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') { continue; }
				if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
				if (dir % 2 == 1) {
					if (field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g') { continue; }
					if (field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g') { continue; }
				}
				dist[ny][nx] = ndist;
				que.push(DistPoint(nx, ny, ndist, dir));
			}
		}
	}
  int CalcDist(const int mask[51][51]) {
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
    while (!que.empty()) {
      DistPoint p = que.top();
      que.pop();
      if (visit[p.y][p.x]) { continue; }
      visit[p.y][p.x] = true;
      if (field[p.y][p.x] == 's') {
        ret = min(ret, p.dist);
        cnt++;
      }
      REP(dir, 8) {
			  const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
			  const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        int nx = p.x + dx[dir];
        int ny = p.y + dy[dir];
				int ndist = p.dist + 10 + dir % 2 * 4;
				if ((field[ny][nx] != '0' && field[ny][nx] != 's' && field[ny][nx] != 'g') || mask[ny][nx] != 0) { continue; }
				if (visit[ny][nx] || dist[ny][nx] < ndist) { continue; }
				if (dir % 2 == 1) {
					if ((field[ny][p.x] != '0' && field[ny][p.x] != 's' && field[ny][p.x] != 'g') || mask[ny][nx] != 0) { continue; }
					if ((field[p.y][nx] != '0' && field[p.y][nx] != 's' && field[p.y][nx] != 'g') || mask[ny][nx] != 0) { continue; }
				}
				dist[ny][nx] = ndist;
				que.push(DistPoint(nx, ny, ndist, dir));
      }
    }
    if (cnt != ss.size()) { return 1 << 28; }
    return ret;
  }
  bool dfs(int mask[51][51], bool visit[51][51], int x, int y) const {
    if (visit[y][x]) { return false; }
    visit[y][x] = true;
    bool ret = field[y][x] == 'g';
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    REP(dir, 4) {
      int nx = x + dx[dir];
      int ny = y + dy[dir];
      if (field[ny][nx] != 's' && field[ny][nx] != '0' && field[ny][nx] != 'g' || mask[ny][nx] != 0) { continue; }
      ret |= dfs(mask, visit, nx, ny);
    }
    return ret;
  }

  bool OK(int mask[51][51], int bx, int by) {
    if (mask[bx][by] == 1 || field[by][bx] != '0') { return false; }
    bool visit[51][51];
    MEMSET(visit, false);
    field[by][bx] = 'P';
    REP(y, h) {
      REP(x, w) {
        if (visit[y][x]) { continue; }
        if (field[y][x] == 's') {
          if (!dfs(mask, visit, x, y)) { goto ng; }
        }
      }
    }
    field[by][bx] = '0';
    return true;
  ng:;
    field[by][bx] = '0';
    return false;
  }
};