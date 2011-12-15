#pragma once

#include "Base.h"
#include "Structure.h"

struct Field {
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
	Field(int f[51][51], int w, int h) : w(w), h(h) {
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
		struct LocalPoint {
			int x;
			int y;
			int dist;
			int dir;
			LocalPoint() {;}
			LocalPoint(int x, int y, int dist, int dir) : x(x), y(y), dist(dist), dir(dir) {;}
			bool operator<(const LocalPoint &rhs) const {
				if (dist != rhs.dist) { return dist > rhs.dist; }
				return dir > rhs.dir;
			}
		};
		priority_queue<LocalPoint> que;
		FORIT(it, gs) {
			que.push(LocalPoint(it->x, it->y, 0, 0));
		}
		bool visit[51][51];
		MEMSET(visit, false);
		MEMSET(dist, 0x0f);
    MEMSET(move, 0x0f);
		while (!que.empty()) {
			LocalPoint p = que.top();
			que.pop();
			if (visit[p.y][p.x]) { continue; }
			visit[p.y][p.x] = true;
			move[p.y][p.x] = p.dir;
			const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
			const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
			REP(dir, 8) {
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
				que.push(LocalPoint(nx, ny, ndist, dir));
			}
		}
	}
};