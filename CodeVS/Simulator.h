#pragma once;

#include "Base.h"
#include "Field.h"
#include "Structure.h"

class Simulator {
	struct Tower {
		int x;
		int y;
		int level;
		int type;
		int charge;
		deque<int> target;
		Tower(TowerInfo info) : x(info.x), y(info.y), level(info.level), type(info.type), charge(0) {;}
		int Attack() const {
			int at[3] = { 10, 20, 3 };
			return at[type] * (level + 1);
		}
		int Wait() const {
			if (type == 2) { return 20; }
			int wa[2] = { 10, 20 };
			return wa[type] - 2 * level;
		}
		int Range2() const {
			int ra[3] = { 4, 5, 2 };
			return square(ra[type] + level);
		}
	};
	struct Enemy {
		EnemyInfo info;
		int x;
		int y;
		int life;
		int wait;
		int charge;
		bool alive;
		Enemy(EnemyInfo info) : info(info), x(info.x), y(info.y), life(info.life), wait(info.t), charge(0), alive(true) {;}
	};
	vector<StageData> stages;
	Simulator() {;}
	Simulator(const char *filename) { Load(filename); }
	void Release() {
		stages.clear();
	}
	bool Load(const char *filename) {
		Release();
		bool ret = true;
		FILE *fp = fopen(filename, "r");
		assert(fp != NULL);
		int stageCnt = -1;
		int v = fscanf(fp, "%d", stageCnt);
		assert(v == 1);
		REP(i, stageCnt) {
			StageData stage;
			ret &= stage.Load(fp);
			stages.push_back(stage);
		}
		fclose(fp);
		return ret;
	}
	pair<int, int> OneStageSimulation(int stage, const vector<TowerInfo> &towers) {
		pair<int, int> ret(0, 0);
    FORIT(it, towers) {
      ret.second -= it->Money();
    }
		REP(i, stages[stage].infos.size()) {
			pair<int, int> nret = OneMapSimulation(stage, i, towers);
			ret.first += nret.first;
			ret.second += nret.second;
		}
		return ret;
	}
	pair<int, int> OneMapSimulation(int stage, int level, const vector<TowerInfo> &towerInfos) {
		pair<int, int> ret(0, 0);
		Field field(stages[stage].field, stages[stage].w, stages[stage].h);
		if (!field.PutTower(towerInfos)) {
			ret.first = -1000;
			return ret;
		}
		vector<EnemyInfo> enemyInfos = stages[stage].infos[level].enemy;
		stable_sort(enemyInfos.begin(), enemyInfos.end());
		vector<Enemy> enemys;
		vector<Tower> towers;
		FORIT(it, towerInfos) { towers.push_back(Tower(*it)); }
		int index = 0;
    for (int frame = 0; frame < (1 << 30); frame++) {
      //ˆÚ“®‚³‚¹‚é
			const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
			const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
			FORIT(it, enemys) {
				it->charge++;
				int dir = field.move[it->y][it->x];
				int need = it->wait * (dir % 2 == 0 ? 10 : 14) / 10;
				if (need == it->charge) {
					it->x += dx[dir];
					it->y += dy[dir];
					it->charge = 0;
				}
			}

      //“G‚ð’Ç‰Á
			while (index < (int)enemyInfos.size() && enemyInfos[index].t == frame) {
				enemys.push_back(enemyInfos[index]);
				index++;
			}

      //ƒ^ƒ[‚Ìˆ—
      FORIT(it1, towers) {
        REP(i, enemys.size()) {
          int d = square(it1->x - enemys[i].x) + square(it1->y - enemys[i].y);
          bool in = d <= it1->Range2() && enemys[i].alive;
          FORIT(it2, it1->target) {
            if (*it2 == i) {
              if (!in) {
                it1->target.erase(it2);
              }
              goto next;
            }
          }
          if (in) {
            it1->target.push_back(i);
          }
next:;
        }

        //’e‚ðŒ‚‚Ä‚é‚È‚çŒ‚‚Â
        if (it1->charge < 0 && !it1->target.empty()) {
          int target = it1->target.front();
          enemys[target].life -= it1->Attack();
          if (it1->type == 2) {
            enemys[target].charge -= 2;
          }
          it1->charge = it1->Wait();
        }
        it1->charge--;
      }

      //“G‚ªŽ€–S‚µ‚Ä‚é‚©‚Ç‚¤‚©Šm”F
      int deadCnt = 0;
      FORIT(it, enemys) {
        if (it->life <= 0) {
          if (it->alive && stage >= 40) {
            int money = (frame - it->info.t) / 10;
            ret.second += money;
          }
          deadCnt++;
          it->alive = false;
        }
      }
      if (deadCnt == enemyInfos.size()) { break; }
		}
		return ret;
	}
};