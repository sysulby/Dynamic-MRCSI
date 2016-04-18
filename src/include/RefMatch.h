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
        unsigned short refid, start, length;
        char mismatch;

        RME(unsigned short i, unsigned short s, unsigned short l, char m):
                refid(i), start(s), length(l), mismatch(m) {}

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
                size_t seed = hash<unsigned short>()(r.refid);
                seed = (seed ^ (hash<unsigned short>()(r.start) << 1)) >> 1;
                seed = (seed ^ (hash<unsigned short>()(r.length) << 1)) >> 1;
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
        int refid, start, length, score;

        Match(int i, int s, int l, int c):
                refid(i), start(s), length(l), score(c) {}

        bool operator<(const Match &m) const
        {
                if (refid ^ m.refid) return refid < m.refid;
                if (start ^ m.start) return start < m.start;
                return length < m.length;
        }
};

#endif // REFMATCH_H_INCLUDED
