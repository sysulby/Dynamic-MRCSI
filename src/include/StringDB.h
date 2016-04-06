/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: StringDB.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-06
 */

#include <bits/stdc++.h>

using namespace std;

#define rep(i,n) for (int i = 0; i < (n); ++i)
#define For(i,s,t) for (int i = (s); i <= (t); ++i)
#define foreach(i,c) for (__typeof(c.begin()) i = c.begin(); i != c.end(); ++i)

typedef long long LL;
typedef pair<int, int> Pii;

const int inf = 0x3f3f3f3f;
const LL infLL = 0x3f3f3f3f3f3f3f3fLL;

#ifndef STRINGDB_H_INCLUDED
#define STRINGDB_H_INCLUDED

class StringDB {
        int maxql, maxk;

        public:
        StringDB(int maxql, int maxk)
        {
                this->maxql = maxql, this->maxk = maxk;
        }

        ~StringDB()
        {
        }

        void addString(const string &str)
        {
        }

        void addStringFromDisk(const char* path)
        {
                string str;
                ifstream fin(path);
                while (getline(fin, str)) addString(str);
                fin.close();
        }

        void query(const string &str, int k)
        {
        }
};

#endif // STRINGDB_H_INCLUDED
