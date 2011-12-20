#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#pragma comment (lib, "winmm.lib")

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <set>
#include <list>

#include <Windows.h>
#include <mmsystem.h>

using namespace std;
typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;
static const double EPS = 1e-9;
static const double PI = acos(-1.0);

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define FOR(i, s, n) for (int i = (s); i < (int)(n); i++)
#define FOREQ(i, s, n) for (int i = (s); i <= (int)(n); i++)
#define FORIT(it, c) for (auto it= (c).begin(); it != (c).end(); it++)
#define MEMSET(v, h) memset((v), h, sizeof(v))

static int OutputLogVa(const char *filename, const char *format, va_list ap)
{
  assert(filename != NULL && filename[0] != '\0');
	char str[10000];
	vsnprintf(str, 9999, format, ap);
	
	FILE *fp = fopen(filename, "a");
	if (fp == NULL) {
		fprintf(stderr, "can\'t open %s\n", filename);
		return -1;
	}
	fprintf(fp, str);
	fclose(fp);

	return 0;
}

int OutputLog(const char *filename, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int result = OutputLogVa(filename, format, ap);
	va_end(ap);
	return result;
}



template<class T>
T square(T x) { return x * x; }

//cppÉtÉ@ÉCÉãçÏÇÈÇÃÇﬂÇÒÇ«Ç¢
template<class T>
void PrintMatrix(const T mat[51][51], int w, int h) {
  REP(y, h) {
    REP(x, w) {
      fprintf(stderr, "%d ", mat[y][x]);
    }
    fprintf(stderr, "\n");
  }
}

inline int CalcCost(int type, int plevel, int level) {
  return (10 + 5 * type) * ((level + 1) * (level + 2) / 2 - (plevel + 1) * (plevel + 2) / 2);
}

