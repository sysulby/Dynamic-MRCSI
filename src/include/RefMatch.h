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
        short refid, start, length;
        char mismatch;

        Rme(short id, short s, short l, char m):
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
                size_t seed = hash<short>()(r.refid);
                seed = (seed ^ (hash<short>()(r.start) << 1)) >> 1;
                seed = (seed ^ (hash<short>()(r.length) << 1)) >> 1;
                seed = (seed ^ (hash<char>()(r.mismatch) << 1)) >> 1;
                return seed;
        }
};

// Referential match sequence
struct Rcs {
        vector<Rme> seq;
        vector<int> offset;

        Rcs() {}
        Rcs(const Rme &r) { push_back(r); }

        int size() const { return seq.size(); }

        Rme& operator[](int i) { return seq[i]; }

        void push_back(Rme r)
        {
                offset.push_back(seq.empty()? 0:
                                offset.back() + seq.back().length + 1);
                seq.push_back(r);
        }
};

#endif // REFMATCH_H_INCLUDED
