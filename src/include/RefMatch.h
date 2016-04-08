/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: RefMatch.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-08
 */
#ifndef REFMATCH_H_INCLUDED
#define REFMATCH_H_INCLUDED

using namespace std;

// Referential match entry
struct Rme {
        int refid, start, length, offset;
        char mismatch;

        Rme(int id, int s, int l, char m):
                refid(id), start(s), length(l), mismatch(m) {}

        bool operator==(const Rme &r) const
        {
                return refid == r.refid && start == r.start &&
                        length == r.length && mismatch == r.mismatch;
        }
};

template<>
struct hash<Rme>
{
        std::size_t operator()(const Rme &r) const
        {
                size_t seed = hash<int>()(r.refid);
                seed = (seed ^ (hash<int>()(r.start) << 1)) >> 1;
                seed = (seed ^ (hash<int>()(r.length) << 1)) >> 1;
                seed = (seed ^ (hash<char>()(r.mismatch) << 1)) >> 1;
                return seed;
        }
};

// Referential match sequence
struct Rcs {
        int len;
        vector<Rme> seq;

        Rcs(): len(0) {}
        Rcs(const Rme &r): len(0) { push_back(r); }

        int size() { return seq.size(); }

        Rme& operator[](int i) { return seq[i]; }

        void push_back(Rme r)
        {
                r.offset = len;
                len += r.length + 1;
                seq.push_back(r);
        }
};

#endif // REFMATCH_H_INCLUDED
