/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: StringDB.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-06
 */
#ifndef STRINGDB_H_INCLUDED
#define STRINGDB_H_INCLUDED

#include "RefMatch.h"
#include "SufAutomaton.h"

using namespace std;

typedef pair<int, int> Pii;

extern const int inf;

class StringDB {
        int count, maxql, maxk;
        SAM *pref, *rcsref, *ovl;
        vector<Pii> pseg, rseg, oseg;
        vector<Rcs> comp;
        unordered_map<Rme, int> rmeid;

        inline int get_rmeid(const Rme &r)
        { return rmeid.count(r)? rmeid[r]: -1; }

        inline int alloc_rmeid(const Rme &r)
        { return rmeid.count(r)? rmeid[r]: rmeid[r] = rmeid.size() + 1; }

        void compress(const string &s, Rcs &rcs)
        {
                list<pair<Rme, int> > seq;
                for (int i = 0; i < s.length();) {
                        int pos = 0, len = 0;
                        pref->lcp(s.c_str() + i, pos, len);
                        vector<Pii>::iterator p = upper_bound(pseg.begin(),
                                        pseg.end(), Pii(pos, inf));
                        int id = p->second;
                        if (p != pseg.begin()) pos -= (--p)->first;
                        if (i + len < s.length()) {
                                Rme rme(id, pos, len, s[i+len]);
                                seq.push_back(make_pair(rme, get_rmeid(rme)));
                                i += len + 1;
                        } else {
                                Rme rme(id, pos, len - 1, s.back());
                                seq.push_back(make_pair(rme, get_rmeid(rme)));
                                i += len;
                        }
                }
                while (true) {
                        bool found = false;
                        for (list<pair<Rme, int> >::iterator it = seq.begin();
                                        it != seq.end();) {
                                int pos = 0, len = 0;
                                rcsref->lcp(seq, it, pos, len);
                                if (len <= 1) {
                                        ++it;
                                        continue;
                                }
                                vector<Pii>::iterator p = upper_bound(
                                                rseg.begin(), rseg.end(),
                                                Pii(pos, inf));
                                int id = p->second;
                                if (p != rseg.begin()) pos -= (--p)->first;
                                int length = -1;
                                for (int i = pos; i < pos + len; ++i)
                                        length += comp[id][i].length + 1;
                                Rme rme(id, comp[id][pos].offset, length,
                                                comp[id][pos+len-1].mismatch);
                                seq.insert(it, make_pair(rme, get_rmeid(rme)));
                                found = true;
                                break;
                        }
                        if (!found) break;
                }
                foreach(it,seq) rcs.push_back(it->first);
        }

        public:
        StringDB(int q, int k): count(0), maxql(q), maxk(k),
        pref(new SAM()), rcsref(new SAM()), ovl(new SAM()) {}

        ~StringDB() { delete pref, delete rcsref, delete ovl; }

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
                        rcs = Rcs(Rme(id, 0, s.length() - 1, s.back()));
                }
                // add to COMP
                comp.push_back(rcs);
                for (int i = 0; i < rcs.size(); ++i)
                        rcsref->append(alloc_rmeid(rcs[i]));
                rcsref->append(0);
                rseg.push_back(Pii(rcsref->length(), id));
                // update RME-INTERVAL-TREE
                // update OVERLAP-SAM
        }

        void addStringFromDisk(const char* path)
        {
                string s, buf;
                ifstream fin(path);
                while (getline(fin, buf)) s += buf + '\n';
                addString(s);
                fin.close();
        }

        void query(const string &s, int k)
        {
                // query in PREF-SAM
                // query in REM-INTERVAL-TREE
                // query in OVERLAP-SAM
        }

        int size()
        {
                int size = pref->length();
                foreach(it,comp) size += it->size() * sizeof(Rme);
                return size;
        }
};

#endif // STRINGDB_H_INCLUDED
