/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: IntervalTree.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-08
 */
#ifndef INTERVALTREE_H_INCLUDED
#define INTERVALTREE_H_INCLUDED

using namespace std;

typedef pair<int, int> Pii;

class IntTree {
        struct Node {
                int left, right, maxr, siz, id, offset;
                Node* ch[2];
                Node(int l = -1, int r = -1, int i = -1, int o = -1):
                        left(l), right(r), maxr(r), siz(1), id(i), offset(o) {}
                void update()
                {
                        siz = ch[0]->siz + ch[1]->siz + 1;
                        maxr = max(right, max(ch[0]->maxr, ch[1]->maxr));
                }
        } *null, *root;

        void release(Node *u)
        {
                if (u == null) return;
                release(u->ch[0]), release(u->ch[1]);
                delete u;
        }

        void rotate(Node* &u, int d)
        {
                Node *v = u->ch[d^1];
                u->ch[d^1] = v->ch[d];
                v->ch[d] = u, v->siz = u->siz;
                u->update(), u = v;
        }

        void maintain(Node* &u, int d)
        {
                if (u == null) return;
                if (u->ch[d]->ch[d]->siz > u->ch[d^1]->siz) {
                        rotate(u, d ^ 1);
                } else if (u->ch[d]->ch[d^1]->siz <= u->ch[d^1]->siz) {
                        return;
                } else {
                        rotate(u->ch[d], d);
                        rotate(u, d ^ 1);
                }
                maintain(u->ch[0], 0);
                maintain(u->ch[1], 1);
                maintain(u, 0);
                maintain(u, 1);
        }

        void insert(Node* &u, int l, int r, int i, int o)
        {
                if (u == null)
                {
                        u = new Node(l, r, i, o);
                        u->ch[0] = u->ch[1] = null;
                        return;
                }
                int d = (l > u->left);
                insert(u->ch[d], l, r, i, o);
                maintain(u, d), u->update();
        }

        void query(Node *u, int l, int r, vector<Pii> &poss)
        {
                if (u == null || u->maxr < r) return;
                if (u->left <= l && r <= u->right)
                        poss.push_back(Pii(u->id, u->offset + l - u->left));
                query(u->ch[0], l, r, poss);
                if (u->left <= l) query(u->ch[1], l, r, poss);
        }

        public:
        IntTree(): null(new Node()), root(null)
        { null->siz = 0, null->ch[0] = null->ch[1] = null; }

        ~IntTree() { release(root); }

        void insert(int l, int r, int i, int o) { insert(root, l, r, i, o); }

        void query(int l, int r, vector<Pii> &poss) { query(root, l, r, poss); }
};

#endif // INTERVALTREE_H_INCLUDED
