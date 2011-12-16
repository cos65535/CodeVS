#pragma once

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

struct LevelInfo {
	vector<TowerInfo> tower;
	vector<EnemyInfo> enemy;
	int life;
	int money;
	LevelInfo() {;}
	LevelInfo(FILE *fp) { Load(fp); }
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
		v = fscanf(fp, " END");
    assert(v == 0);
		return true;
	}

  ////ÇøÇ™Ç§É^ÉèÅ[ÇÃåöÇƒï˚ÇÇµÇƒÇ‡ÇªÇÃÇ‹Ç‹ÇÃèoóÕÇÇ∑ÇÈ
  //void Print() {
  //  printf("%d %d %d %d\n", life, money, (int)tower.size(), (int)enemy.size());
  //  REP(i, tower.size()) {
  //    printf("%d %d %d %d\n", tower[i].x, tower[i].y, tower[i].level, tower[i].type);
  //  }
  //  REP(i, enemy.size()) {
  //    printf("%d %d %d %D\n", enemy[i].x, enemy[i].y, enemy[i].t, enemy[i].life, enemy[i].speed);
  //  }
  //  printf("END\n");
  //}
};

struct MapInfo {
	int w, h;
	int field[51][51];
	int levelCnt;
	vector<LevelInfo> levels;
	MapInfo() {;}
	MapInfo(FILE *fp) { Load(fp); }
	void Release() {
		w = 0xdeadbeaf; h = 0xdeadbeaf;
		MEMSET(field, 0x0f);
		levels.clear();
	}
	bool LoadHeader(FILE *fp) {
		Release();
		bool ret = true;
		int v = fscanf(fp, "%d %d", &w, &h);
		assert(v == 2);
		REP(y, h) {
			REP(x, w) {
				char c;
				v = fscanf(fp, " %c ", &c);
				assert(v == 1);
				field[y][x] = c;
			}
		}
		v = fscanf(fp, "%d", &levelCnt);
		assert(v == 1);
		v = fscanf(fp, " END");
		assert(v == 0);
    return ret;
	}
	bool LoadLevel(FILE *fp) {
		assert((int)levels.size() < levelCnt);
		bool ret = true;
		LevelInfo info;
		ret &= info.Load(fp);
		levels.push_back(info);
		return ret;
	}
	bool Load(FILE *fp) {
		Release();
		bool ret = true;
		ret = LoadHeader(fp);
		REP(i, levelCnt) {
			LoadLevel(fp);
		}
		return ret;
	}
  //void Print() {
  //  printf("%d %d\n", w, h);
  //  REP(y, h) {
  //    REP(x, w) {
  //      printf("%c", (char)field[y][x]);
  //    }
  //    puts("");
  //  }
  //  fprintf("%d\n", levelCnt);
  //  fprintf("END\n");
  //  REP(i, levelCnt) {
  //    levels[i].Print();
  //  }
  //}
};
