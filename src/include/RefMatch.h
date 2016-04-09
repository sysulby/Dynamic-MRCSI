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
struct RME {
        short refid, start, length;
        char mismatch;

        RME(short id, short s, short l, char m):
                refid(id), start(s), length(l), mismatch(m) {}

        bool operator==(const RME &r) const
        {
                return refid == r.refid && start == r.start &&
                        length == r.length && mismatch == r.mismatch;
        }
};

template<>
struct hash<RME>
{
        std::size_t operator()(const RME &r) const
        {
                size_t seed = hash<short>()(r.refid);
                seed = (seed ^ (hash<short>()(r.start) << 1)) >> 1;
                seed = (seed ^ (hash<short>()(r.length) << 1)) >> 1;
                seed = (seed ^ (hash<char>()(r.mismatch) << 1)) >> 1;
                return seed;
        }
};

// Referential match sequence
struct RCS {
        vector<RME> seq;
        vector<int> offset;

        RCS() {}
        RCS(const RME &r) { push_back(r); }

        int size() const { return seq.size(); }

        RME& operator[](int i) { return seq[i]; }

        void push_back(RME r)
        {
                offset.push_back(seq.empty()? 0:
                                offset.back() + seq.back().length + 1);
                seq.push_back(r);
        }
};

// Match result
struct Match {
        short refid, start, length, score;
};

#endif // REFMATCH_H_INCLUDED
