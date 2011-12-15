#pragma once;

#include "Base.h"

struct TowerInfo {
  int x;
  int y;
  int level;
  int type;
  TowerInfo() {;}
  TowerInfo(int x, int y, int level, int type) :
    x(x), y(y), level(level), type(type) {;}
	int Money() const {
		int mo[3] = { 10, 15, 20 };
		return mo[type] * (level + 1) * (level + 2) / 2;
	}
};

struct EnemyInfo {
  int x;
  int y;
  int t;
  int life;
  int speed;
  EnemyInfo() {;}
  EnemyInfo(int x, int y, int t, int life, int speed) :
    x(x), y(y), t(t), life(life), speed(speed) {;}
  bool operator<(const EnemyInfo &rhs) const {
    return t < rhs.t;
  }
};

struct MapInitalInfo {
	vector<TowerInfo> tower;
	vector<EnemyInfo> enemy;
	int life;
	int money;
	MapInitalInfo() {;}
	MapInitalInfo(FILE *fp) { Load(fp); }
	void Release() {
		tower.clear();
		enemy.clear();
		life = 0xdeadbeaf;
		money = 0xdeadbeaf;
	}
	bool Load(FILE *fp) {
		Release();
		int towerCnt, enemyCnt;
		int v = fscanf(fp, "%d %d %d %d", &life, &money, &towerCnt, &enemyCnt);
		assert(v == 4);
		REP(i, towerCnt) {
			int x, y, a, c;
			int v = fscanf(fp, "%d %d %d %d", &x, &y, &a, &c);
			assert(v == 4);
			tower.push_back(TowerInfo(x, y, a, c));
		}
		REP(i, enemyCnt) {
			int x, y, t, l, s;
			int v = fscanf(fp, "%d %d %d %d %d", &x, &y, &t, &l, &s);
			assert(v == 5);
			enemy.push_back(EnemyInfo(x, y, t, l, s));
		}
		return true;
	}
};

struct StageData {
	int w, h;
	int field[51][51];
	int levelCnt;
	vector<MapInitalInfo> infos;
	StageData() {;}
	StageData(FILE *fp) { Load(fp); }
	void Release() {
		w = 0xdeadbeaf; h = 0xdeadbeaf;
		MEMSET(field, 0x0f);
		infos.clear();
	}
	bool LoadHeader(FILE *fp) {
		Release();
		bool ret = true;
		int v = fscanf(fp, "%d %d", &w, &h);
		assert(v == 2);
		REP(y, h) {
			REP(x, w) {
				int c;
				v = fscanf(fp, " %c ", &c);
				assert(v == 1);
				field[y][x] = c;
			}
		}
		v = fscanf(fp, "%d", &levelCnt);
		assert(v == 1);
		v = fscanf(fp, " END");
		assert(v == 0);
	}
	bool LoadMap(FILE *fp) {
		assert((int)infos.size() < levelCnt);
		bool ret = true;
		MapInitalInfo info;
		ret &= info.Load(fp);
		infos.push_back(info);
		return ret;
	}
	bool Load(FILE *fp) {
		Release();
		bool ret = true;
		ret = LoadHeader(fp);
		REP(i, levelCnt) {
			LoadMap(fp);
		}
		return ret;
	}
};
