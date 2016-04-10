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
        string pstr, ostr;
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

        void dp_pre(int k, const string &s, int p,
                        const string &t, int l, int r, vector<Pii> &ret)
        {
                vector<int> f[2] = {
                        vector<int>(r - l + 1),
                        vector<int>(r - l + 1)
                };
                int o = 0;
                for (int j = 0; j <= r - l; ++j) f[o][j] = j;
                for (int i = 1; i <= p; ++i) {
                        f[o^=1][0] = i;
                        for (int j = 1; j <= r - l; ++j) {
                                if (r - j < 0 || !t[r-j]) break;
                                f[o][j] = min({
                                                f[o^1][j] + 1,
                                                f[o][j-1] + 1,
                                                f[o^1][j-1] + (s[p-i] != t[r-j])
                                                });
                        }
                }
                for (int j = 0; j <= r - l; ++j) {
                        if (r - j < 0 || !t[r-j]) break;
                        if (f[o][j] <= k)
                                ret.push_back(Pii(f[o][j], r - j));
                }
                sort(ret.begin(), ret.end());
        }

        void dp_suf(int k, const string &s, int p,
                        const string &t, int l, int r, vector<Pii> &ret)
        {
                vector<int> f[2] = {
                        vector<int>(r - l + 1),
                        vector<int>(r - l + 1)
                };
                int o = 0;
                for (int j = 0; j <= r - l; ++j) f[o][j] = j;
                for (int i = 1; i <= s.length() - p; ++i) {
                        f[o^=1][0] = i;
                        for (int j = 1; j <= r - l; ++j) {
                                if (l + j - 1 >= t.length() || !t[l+j-1]) break;
                                f[o][j] = min({
                                                f[o^1][j] + 1,
                                                f[o][j-1] + 1,
                                                f[o^1][j-1]
                                                + (s[p+i-1] != t[l+j-1])
                                                });
                        }
                }
                for (int j = 0; j <= r - l; ++j) {
                        if (l + j - 1 >= t.length() || !t[l+j-1]) break;
                        if (f[o][j] <= k)
                                ret.push_back(Pii(f[o][j], l + j - 1));
                }
                sort(ret.begin(), ret.end());
        }

        void combine(int id, int offset, int k,
                        const vector<Pii> &fpre, const vector<Pii> &fsuf,
                        vector<Match> &ret)
        {
                set<Match> pre_ret(ret.begin(), ret.end());
                for (int i = 0, j = fsuf.size() - 1; i < fpre.size(); ++i) {
                        while (j >= 0 && fpre[i].first + fsuf[j].first > k) --j;
                        for (int k = j; k >= 0; --k) {
                                int start = fpre[i].second - offset,
                                    len = fsuf[k].second - fpre[i].second + 1,
                                    score = fpre[i].first + fsuf[k].first;
                                pre_ret.insert(Match(id, start, len, score));
                        }
                }
                ret = vector<Match>(pre_ret.begin(), pre_ret.end());
        }

        void match_pref(const string &s, int k, vector<Match> &ret)
        {
                int m = s.length();
                for (int i = 0, l = 0; i < k + 1; ++i) {
                        int r = m * (i + 1) / (k + 1) - l;
                        string seed = s.substr(l, r);
                        vector<int> poss;
                        pref->match(seed, poss);
                        foreach(it,poss) {
                                int pos = *it;
                                vector<Pii> fpre, fsuf;
                                dp_pre(k, s, l, pstr, pos - l - k, pos, fpre);
                                dp_suf(k, s, l + r, pstr,
                                                pos + r, pos + m - l + k, fsuf);
                                vector<Pii>::iterator p = upper_bound(
                                                pseg.begin(), pseg.end(),
                                                Pii(pos, inf));
                                int id = p->second, offset =
                                        (p == pseg.begin()? 0: (--p)->first);
                                combine(id, offset, k, fpre, fsuf, ret);
                        }
                        l += r;
                }
        }

        void bfs_rmemap(vector<Match> &ret)
        {
                set<Match> pre_ret(ret.begin(), ret.end());
                for (int i = 0; i < ret.size(); ++i) {
                        vector<Pii> poss;
                        rmemap[ret[i].refid]->query(ret[i].start,
                                        ret[i].start + ret[i].length - 1, poss);
                        foreach(it,poss) {
                                Match m(it->first, it->second,
                                                ret[i].length, ret[i].score);
                                if (!pre_ret.count(m)) {
                                        pre_ret.insert(m);
                                        ret.push_back(m);
                                }
                        }
                }
                ret = vector<Match>(pre_ret.begin(), pre_ret.end());
        }

        void match_ovlmap(const string &s, int k, vector<Match> &ret)
        {
                int m = s.length();
                for (int i = 0, l = 0; i < k + 1; ++i) {
                        int r = m * (i + 1) / (k + 1) - l;
                        string seed = s.substr(l, r);
                        vector<int> poss;
                        ovlmap->match(seed, poss);
                        foreach(it,poss) {
                                int pos = *it;
                                vector<Pii> fpre, fsuf;
                                dp_pre(k, s, l, ostr, pos - l - k, pos, fpre);
                                dp_suf(k, s, l + r, ostr,
                                                pos + r, pos + m - l + k, fsuf);
                                vector<pair<int, Pii> >::iterator p =
                                        upper_bound(oseg.begin(), oseg.end(),
                                                        make_pair(pos,
                                                                Pii(inf, inf)));
                                int id = p->second.first,
                                    offset = -p->second.second;
                                if (p != oseg.begin()) offset += (--p)->first;
                                combine(id, offset, k, fpre, fsuf, ret);
                        }
                        l += r;
                }
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
                        for (int i = 0; i < s.length(); ++i) {
                                pref->append(s[i]);
                                pstr += s[i];
                        }
                        pref->append(0);
                        pstr += (char)0;
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
                // update RMEMAP
                rmemap.push_back(new IntTree());
                for (int i = 0; i < rcs.size(); ++i)
                        rmemap[rcs[i].refid]->insert(rcs[i].start,
                                        rcs[i].start + rcs[i].length - 1,
                                        id, rcs.offset[i]);
                // update OVERLAPMAP
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
                        for (int j = l; j <= r; ++j) {
                                ovlmap->append(s[j]);
                                ostr += s[j];
                        }
                        ovlmap->append(0);
                        ostr += (char)0;
                        oseg.push_back(make_pair(ovlmap->length(), Pii(id, l)));
                }
        }

        void addStringFromDisk(const char* path)
        {
                string str, buf;
                ifstream fin(path);
                while (getline(fin, buf)) str += buf + '\n';
                addString(str);
                fin.close();
                cout << "Compress ratio: " << ratio() * 100 << "%" << endl;
                cout << endl;
        }

        void query(const string &s, int k)
        {
                vector<Match> ret;
                match_pref(s, k, ret);
                bfs_rmemap(ret);
                match_ovlmap(s, k, ret);
                cout << "Match with " << ret.size() << " substring(s):" << endl;
                cout << string(32, '-') << endl;
                cout << setw(8) << "refid " <<
                        setw(8) << "start " <<
                        setw(8) << "length " <<
                        setw(8) << "score " << endl;
                cout << string(32, '-') << endl;
                foreach(it,ret) cout <<
                        setw(7) << it->refid << " " <<
                        setw(7) << it->start << " " <<
                        setw(7) << it->length << " " <<
                        setw(7) << it->score << " " << endl;
                cout << string(32, '-') << endl;
                cout << endl;
        }

        int size() const
        {
                int size = pstr.length() + ostr.length();
                foreach(it,comp) size += it->size() * sizeof(RME);
                return size;
        }

        int length() const { return cntc; }

        double ratio() const { return (double)size() / length(); }
};

#endif // STRINGDB_H_INCLUDED
