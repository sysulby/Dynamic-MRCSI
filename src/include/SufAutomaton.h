/**
 * Copyright © 2016 Authors. All rights reserved.
 * 
 * FileName: SufAutomaton.h
 * Author: Beiyu Li <sysulby@gmail.com>
 * Date: 2016-04-07
 */
#ifndef SUFAUTOMATON_H_INCLUDED
#define SUFAUTOMATON_H_INCLUDED

using namespace std;

#define foreach(i,c) for (__typeof(c.begin()) i = c.begin(); i != c.end(); ++i)

class SAM {
        int count;
        struct Node {
                int len, right;
                Node *par;
                unordered_set<Node*> ch;
                unordered_map<int, Node*> trans;
        } *root, *last;

        void release(Node* u)
        {
                if (!u) return;
                foreach(it,u->trans) release(it->second);
                delete u;
        }

        void dfs(Node *u, int len, vector<int> &pos)
        {
                if (u->right > 0) pos.push_back(u->right - len);
                foreach(it,u->ch) dfs(*it, len, pos);
        }

        public:
        SAM()
        {
                count = 0;
                root = last = new Node();
                root->len = 0, root->right = -1, root->par = NULL;
        }

        ~SAM() { release(root); }

        int length() const { return count; }

        void append(int v)
        {
                Node *p = last, *np = new Node();
                np->len = p->len + 1, np->right = ++count;
                for (; p && !p->trans[v]; p = p->par) p->trans[v] = np;
                if (!p) {
                        np->par = root;
                        root->ch.insert(np);
                } else {
                        Node *q = p->trans[v];
                        if (p->len + 1 == q->len) {
                                np->par = q;
                                q->ch.insert(np);
                        } else {
                                Node *nq = new Node();
                                nq->len = p->len + 1;
                                nq->right = -abs(q->right);
                                nq->par = q->par, nq->trans = q->trans;
                                nq->par->ch.erase(q), nq->par->ch.insert(nq);
                                np->par = q->par = nq;
                                nq->ch.insert(np), nq->ch.insert(q);
                                for (; p && p->trans[v] == q; p = p->par)
                                        p->trans[v] = nq;
                        }
                }
                last = np;
        }

        void lcp(const char* s, int &pos, int &len)
        {
                Node *p = root;
                for (int i = 0; s[i]; ++i) {
                        if (p->trans.count(s[i])) {
                                len = i + 1;
                                p = p->trans[s[i]];
                        } else {
                                break;
                        }
                }
                pos = abs(p->right) - len;
        }

        void match(const string &s, vector<int> &pos)
        {
                Node *p = root;
                for (int i = 0; i < s.length(); ++i) {
                        if (!p->trans.count(s[i])) return;
                        p = p->trans[s[i]];
                }
                dfs(p, s.length(), pos);
        }
};

#endif // SUFAUTOMATON_H_INCLUDED
