/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: main.cpp
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

const int maxql = 20;
const int maxk = 5;

int main(int argc[], char* argv[])
{
        StringDB *sdb = new StringDB(maxql, maxk);

        sdb->addStringFromDisk("toydata/string1.txt");
        sdb->addStringFromDisk("toydata/string2.txt");
        sdb->addStringFromDisk("toydata/string3.txt");
        sdb->addStringFromDisk("toydata/string4.txt");
        sdb->addStringFromDisk("toydata/string5.txt");

        sdb->query("Helseinki", 1);

        return 0;
}
