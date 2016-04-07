/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: StringDB.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-06
 */
#ifndef STRINGDB_H_INCLUDED
#define STRINGDB_H_INCLUDED

#include "SufAutomaton.h"

using namespace std;

typedef pair<int, int> Pii;

extern const int inf;

// Referential match entry
class Rme {
        int refid, start, length;
        char mismatch;

        public:
        Rme(int id, int s, int l, char m):
                refid(id), start(s), length(l), mismatch(m) {}
};

// Referential match sequence
typedef vector<Rme> Rcs;

class StringDB {
        int count, maxql, maxk;
        SAM *pref, *overlap;
        vector<Pii> pseg, oseg;
        vector<Rcs> comp;

        void compress(const string &s, Rcs &rcs)
        {
                for (int i = 0; i < s.length();) {
                        int pos = 0, len = 0;
                        pref->lcp(s.c_str() + i, pos, len);
                        int id = lower_bound(pseg.begin(),
                                        pseg.end(), Pii(pos, 0))->second;
                        if (i + len < s.length()) {
                                rcs.push_back(Rme(id, pos, len, s[i+len]));
                                i += len + 1;
                        } else {
                                rcs.push_back(Rme(id, pos, len - 1, s.back()));
                                i += len;
                        }
                }
        }

        public:
        StringDB(int q, int k): count(0), maxql(q), maxk(k),
        pref(new SAM()), overlap(new SAM()) {}

        ~StringDB() { delete pref; }

        void addString(const string &s)
        {
                Rcs rcs;
                int id = count++, cost = inf;
                if (pref->length()) {
                        compress(s, rcs);
                        cost = rcs.size() * sizeof(Rme);
                }
                if (cost > s.length()) {
                        // add to PREF
                        for (int i = 0; i < s.length(); ++i) pref->append(s[i]);
                        pref->append(0);
                        pseg.push_back(Pii(pref->length(), id));
                        rcs = Rcs(1, Rme(id, 0, s.length() - 1, s.back()));
                }
                // add to COMP
                comp.push_back(rcs);
                // update RME-INTERVAL-TREE
                // update OVERLAP-SAM
        }

        void addStringFromDisk(const char* path)
        {
                string s;
                ifstream fin(path);
                while (getline(fin, s)) addString(s);
                fin.close();
        }

        void query(const string &s, int k)
        {
                // query in PREF-SAM
                // query in REM-INTERVAL-TREE
                // query in OVERLAP-SAM
        }

        int statistics()
        {
                int size = pref->length() + overlap->length();
                foreach(it,comp) size += it->size() * sizeof(Rme);
                return size;
        }
};

#endif // STRINGDB_H_INCLUDED
