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
#include "IntervalTree.h"

using namespace std;

typedef pair<int, int> Pii;

extern const int inf;

class StringDB {
        int cnts, cntc, dlt;
        SAM *pref, *rcsref, *ovlmap;
        vector<Pii> pseg, rseg;
        vector<pair<int, Pii> > oseg;
        vector<RCS> comp;
        vector<IntTree*> rmemap;
        unordered_map<RME, int> rmeid;

        inline int get_rmeid(const RME &r)
        { return rmeid.count(r)? rmeid[r]: -1; }

        inline int alloc_rmeid(const RME &r)
        { return rmeid.count(r)? rmeid[r]: rmeid[r] = rmeid.size() + 1; }

        void compress(const string &s, RCS &rcs)
        {
                // compress from Pref
                list<pair<RME, int> > seq;
                for (int i = 0; i < s.length();) {
                        int pos = 0, len = 0;
                        pref->lcp(s.c_str() + i, pos, len);
                        vector<Pii>::iterator p = upper_bound(pseg.begin(),
                                        pseg.end(), Pii(pos, inf));
                        int id = p->second;
                        if (p != pseg.begin()) pos -= (--p)->first;
                        if (i + len < s.length()) {
                                RME rme(id, pos, len, s[i+len]);
                                seq.push_back(make_pair(rme, get_rmeid(rme)));
                                i += len + 1;
                        } else {
                                RME rme(id, pos, len - 1, s.back());
                                seq.push_back(make_pair(rme, get_rmeid(rme)));
                                i += len;
                        }
                }
                // compress from RCSref
                while (true) {
                        bool found = false;
                        for (list<pair<RME, int> >::iterator it = seq.begin();
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
                                RME rme(id, comp[id].offset[pos], length,
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
        StringDB(int ql, int ed):
                cnts(0), cntc(0), dlt(ql + ed - 1),
                pref(new SAM()), rcsref(new SAM()), ovlmap(new SAM()) {}

        ~StringDB() { delete pref, delete rcsref, delete ovlmap; }

        void addString(const string &s)
        {
                RCS rcs;
                cntc += s.length();
                int id = cnts++, cost = inf;
                if (pref->length()) {
                        // try compress
                        compress(s, rcs);
                        cost = rcs.size() * sizeof(RME);
                }
                if (cost > s.length()) {
                        // add to Pref
                        for (int i = 0; i < s.length(); ++i) pref->append(s[i]);
                        pref->append(0);
                        pseg.push_back(Pii(pref->length(), id));
                        rcs = RCS(RME(id, 0, s.length() - 1, s.back()));
                }
                // add to COMP
                comp.push_back(rcs);
                // add to RCSref
                for (int i = 0; i < rcs.size(); ++i)
                        rcsref->append(alloc_rmeid(rcs[i]));
                rcsref->append(0);
                rseg.push_back(Pii(rcsref->length(), id));
                // update RMEmap
                rmemap.push_back(new IntTree());
                for (int i = 0; i < rcs.size(); ++i)
                        rmemap[rcs[i].refid]->insert(rcs[i].start,
                                        rcs[i].start + rcs[i].length - 1,
                                        id, rcs.offset[i]);
                // update OVERLAPmap
                for (int i = 0; i < rcs.size() - 1;) {
                        int l = rcs.offset[i] + rcs[i].length - dlt, r;
                        for (int j = i + 1; j < rcs.size(); ++j) {
                                if (j == rcs.size() - 1 ||
                                                rcs[i].length > dlt * 2) {
                                        r = rcs.offset[j] + dlt;
                                        i = j;
                                        break;
                                }
                        }
                        l = max(l, 0), r = min(r, (int)s.length() - 1);
                        for (int j = l; j <= r; ++j) ovlmap->append(s[j]);
                        ovlmap->append(0);
                        oseg.push_back(make_pair(ovlmap->length(), Pii(id, l)));
                }
        }

        void addStringFromDisk(const char* path)
        {
                string buf;
                ifstream fin(path);
                int cnt = 0;
                while (getline(fin, buf)) {
                        if (++cnt > 16) break;
                        addString(buf);
                }
                fin.close();
        }

        void query(const string &s, int k)
        {
                // query in PREF-SAM
                // query in REM-INTERVAL-TREE
                // query in OVERLAP-SAM
        }

        int size() const
        {
                int size = pref->length();
                foreach(it,comp) size += it->size() * sizeof(RME);
                return size;
        }

        int length() const { return cntc; }

        double ratio() const { return (double)size() / length(); }
};

#endif // STRINGDB_H_INCLUDED
