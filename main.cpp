#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <time.h>
#include<bits/stdc++.h>

using namespace std;
typedef long long ll;

const int M = 10000000;
const ll deltaT = 10;
const int C = 4; //观察C个周期
const double theta = 10;
#define N 10000005
#define T 4

inline double sqr(const double& x) {
	return x * x;
}

#include "ssummary.h"
#include "Alg.h"

FILE *fin[5] = {fopen("8202.dat", "rb"), 
				fopen("formatted01.dat", "rb"), 
				fopen("formatted02.dat", "rb"), 
				fopen("formatted03.dat", "rb"), 
				fopen("formatted04.dat", "rb")};
			
pair <string, ll> input[N];	
pair <string, ll> Read(int k)
{
	ll t;
	fread(&t, 8, 1, fin[k]);

	uint8_t buf[20];
	fread(buf, 1, 8, fin[k]);
	buf[8] = '\0';
    string s((char *)buf, 8);
    
	return make_pair(s, t);
}

map <string, ll> timeStamp;
multimap <string, ll> bucket;

int total, totalD;
map <string, int> id;
map <pair <string, ll>, double> ans;
//pair <pair <string, ll>, double> ans[N];

void calc(const pair <string, ll>& ele, const multimap <string, ll> :: iterator& itL) {
	double ave = 0, var = 0;
	
	multimap <string, ll> :: iterator it = itL;
	for (int i = 0; i < C; ++i, ++it) ave += it -> second;
	ave /= C;

	it = itL;
	for (int i = 0; i < C; ++i, ++it) var += sqr(it -> second - ave);
	var /= C;

	if (var < theta) {
		++total;
		ans[ele] = ave;

		if (id.find(ele.first) == id.end()) ++totalD;
		id[ele.first] = 1; 
		//output
		/*cerr << ele.second << ' ' << ave<<  ' '<< var<< endl;
		it = itL;
		for (int i = 0; i < C; ++i, ++it) cerr << it -> second << ' '; cerr << endl;*/
	}
	return;
}

void insert(const pair <string, ll>& ele, const ll& t) {
	string s = ele.first;
	if (bucket.find(s) == bucket.end()) { 
		for (int i = 0; i < C; ++i) bucket.insert(make_pair(s, -1));
	}

	multimap <string, ll> :: iterator itL = bucket.lower_bound(s);
	multimap <string, ll> :: iterator it = itL, pos;

	int cnt = 0;
	for (int i = 0; i < C; ++i, ++it) { 
		if (it -> second < 0) ++cnt, pos = it;
	}

	if (cnt > 0) pos -> second = t;
	if (!--cnt) {
		calc(ele, itL);

		if (pos == itL) pos = it;
		--pos; pos -> second = -1;
	}

	return;
}

void Init(int k) {
	total = totalD = 0;
	ans.clear(), id.clear();
	timeStamp.clear();
	bucket.clear();

	for (int i = 0; i < M; ++i) {
		pair <string, ll> element = Read(0);
		ll delta = -1;

		element.second = i;//counter based time
		input[i] = element;
		if (timeStamp.find(element.first) != timeStamp.end() && (delta = element.second - timeStamp[element.first]) >= deltaT) {
			insert(element, delta);
		}
		timeStamp[element.first] = element.second;
	}
}

void run(int k) {
	Init(k);
	Alg* alg = new Alg(4, 1 << 15, min(totalD * 5, 1 << 15)); //bf 1MB = 4 * (1 << 15) * 8 B
										     //hash table 1MB = 4 * (1 << 15) * 8 B
	for (int i = 0; i < M; ++i) alg -> insert(input[i]);

	int count = 0;
	double precision = 0, recall = 0, aae = 0, are = 0;
	for (int i = 0; i < alg -> total; ++i) {
		pair <pair <string, ll>, double> ele = alg -> ans[i];
		
		if (ans.find(ele.first) != ans.end()) ++count;
		else continue;

		double x = ans[ele.first]; //实际周期
		double y = ele.second; //观测出来的周期

		aae += fabs(x - y), are += fabs(x - y) / x; 
	}
	precision = (double)count / alg -> total;
	recall = (double)count / total;
	if (count) aae /= count, are /= count;
	
	cerr << "total: " << total << endl; 
	cerr << "totalD: " << totalD << endl; 
	cerr << "estimated total: " << alg -> total << endl; 
	cerr << "presicion: " << setprecision(3) << precision << endl; 
	cerr << "recall: " << setprecision(3) << recall << endl; 
	cerr << "aae: " << setprecision(3) << aae << endl; 
	cerr << "are: " << setprecision(3) << are << endl; 
}

int main() {
	run(0);
	return 0;
}