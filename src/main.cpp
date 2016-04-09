/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: main.cpp
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-07
 */
#include <bits/stdc++.h>
#include "include/StringDB.h"

using namespace std;

#define rep(i,n) for (int i = 0; i < (n); ++i)
#define For(i,s,t) for (int i = (s); i <= (t); ++i)
#define foreach(i,c) for (__typeof(c.begin()) i = c.begin(); i != c.end(); ++i)

typedef long long LL;
typedef pair<int, int> Pii;

const int inf = 0x3f3f3f3f;
const LL infLL = 0x3f3f3f3f3f3f3f3fLL;

const int maxql = 20;   // max query length
const int maxk = 5;     // max edit distance error

int main(int argc, char* argv[])
{
        // new string data base
        StringDB *sdb = new StringDB(maxql, maxk);

        // add text
        sdb->addStringFromDisk("../toydata/string1.txt");
        sdb->addStringFromDisk("../toydata/string2.txt");
        sdb->addStringFromDisk("../toydata/string3.txt");
        sdb->addStringFromDisk("../toydata/string4.txt");
        sdb->addStringFromDisk("../toydata/string5.txt");

        // query
        sdb->query("Helsinki", 0);
        // sdb->query("Helseinki", 1);

        cout << sdb->ratio() * 100 << "%" << endl;

        return 0;
}
