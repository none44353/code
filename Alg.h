#ifndef _Alg_H
#define _Alg_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHash32.h"
#include "BOBHash64.h"

using namespace std;

#define MAXM 500005
#define MAXL 1000005 

class bloomfliter{
  private:
    int t, M; //t次hash 共t*M个位置
    BOBHash32* bobhash[T];
    uint64_t a[T * MAXM];
  public:
    bloomfliter(int t, int M) : t(t), M(M) {
        srand(time(NULL));
        for (int k = 0; k < t; ++k) 
            bobhash[k] = new BOBHash32(rand() % 5000);
        memset(a, 0, sizeof(a));
    }

    void insert(const uint64_t& x, const uint64_t& key) {
        for (int k = 0; k < t; ++k) {
            unsigned int H = bobhash[k] -> run((char *)&x, 8);
            unsigned int pos = H % M;

            a[k * M + pos] = key;
        }

        return;
    }

    uint64_t query(const uint64_t& x) {
        uint64_t res = -1;
        for (int k = 0; k < t; ++k) {
            unsigned int H = bobhash[k] -> run((char *)&x, 8);
            unsigned int pos = H % M;

            res = min(res, a[k * M + pos]);
        }

        return res;
    }
};

class Alg
{   
  private:
    int L;
    bloomfliter timeStamp;
    BOBHash32* bobhash;
    uint64_t counter[MAXL][C];
  
  public:
    int total;
    pair <pair <uint64_t, uint64_t>, double> ans[N];
    
    Alg(int t, int M, int L) : timeStamp(t, M), L(L), total(0) { //前两个参数是bloomfliter的参数, L是hash表的大小
        bobhash = new BOBHash32(rand() % 5000);
        memset(counter, 0, sizeof(counter));
    } 

    void check(const pair <uint64_t, uint64_t>&ele, int pos) {
        double ave = 0, var = 0;
        for (int i = 0; i < C; ++i) ave += counter[pos][i];
        ave /= C;

        for (int i = 0; i < C; ++i) var += sqr(counter[pos][i] - ave);
        var /= C;

        if (var < ave * ave * theta) 
            ans[++total] = make_pair(ele, ave);
        
        return;
    }

    void insert(const pair <uint64_t, uint64_t>& ele) { //丢一个元素进来
        uint64_t lst = timeStamp.query(ele.first), cur = ele.second;
        timeStamp.insert(ele.first, cur);
        
        if (!lst || cur - lst < deltaT) return;
        
        unsigned int H = bobhash -> run((char *)&ele.first, 8);
        unsigned int pos = H % L;
        
        int cnt = 0, w = 0;
        for (int i = 0; i < C; ++i) 
            if (counter[pos][i] == 0) ++cnt, w = i;
        
        counter[pos][w] = cur - lst;
        if (!--cnt) {
            check(ele, pos);
            if (!w) w = C;
            counter[pos][--w] = 0;
        }

        return;
    }
};
#endif
