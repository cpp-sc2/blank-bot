#include <sc2api/sc2_api.h>
#include "constants.h"
#include "map2d.hpp"

//#define MAXW 12
//#define MAXH 11

//#define Point2DI pair<int, int>

#define V_PII vector<Point2DI>

//#define M_P make_pair 

using namespace std;

//char maze[][MAXH] = {"**********", "*...*....*", "*.*...*.S*", "*.********", "*.*......*", "*.*.****.*",
//                     "*.*.*....*", "*.*.*.****", "*.*.*.*E.*", "*...*....*", "**********"};

int g_dir[8][2] = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}, {1, -1}, {-1, 1}, {-1, -1}, {1, 1}};

struct Node {
    Point2DI pos;
    int g;
    int f;
    Point2DI parent;
    Node() {
        parent.x = -1;
    }
    Node(Point2DI _pos, int _g, int _f, Point2DI _parent) : pos(_pos), g(_g), f(_f), parent(_parent) {
    }
    bool haveParent() {
        return parent.x != -1;
    }
    Point2DI getDirection() {
        return Point2DI((pos.x - parent.x) / max(abs(pos.x - parent.x), 1),
                   (pos.y - parent.y) / max(abs(pos.y - parent.y), 1));
    }
};

//int g_f[MAXW][MAXH];
//map2d < int> g_f()

bool operator<(const Node &a, const Node &b) {
    return a.f < b.f;
}

class Maze {
private:
    //bool inque[MAXW][MAXH];
    //bool vis[MAXW][MAXH];
    map2d<bool> *inque;
    map2d<bool> *vis;
    int w, h;

public:
    Maze(int width, int height){
        inque = new map2d<bool>(width, height, true);
        vis = new map2d<bool>(width, height, true); 
        w = width;
        h = height;
    }

    void clearQue() {
        inque->clear();
    }
    void pushQue(Point2DI t) {
        imRef(inque,t.x,t.y) = true;
    }
    void popQue(Point2DI t) {
        imRef(inque, t.x, t.y) = false;
    }
    bool isInQue(Point2DI t) {
        return imRef(inque, t.x, t.y);
    }
    void clearVis() {
        vis->clear();
    }
    void pushVis(Point2DI t) {
        imRef(vis, t.x, t.y) = true;
    }
    void popVis(Point2DI t) {
        imRef(vis, t.x, t.y) = false;
    }
    bool isInVis(Point2DI t) {
        return imRef(vis, t.x, t.y);
    }

    bool checkMaze(int x, int y, Agent *agent) {
        if (x < 0 || x > w || y < 0 || y > h)
            return false;
        return Aux::checkPathable(x, y, agent);
    }
};
class NodeList {
private:
    vector<Node> l;

public:
    void clear() {
        l.clear();
    }
    void push(Node t) {
        l.push_back(t);
        sort(l.begin(), l.end());
    }
    void pop() {
        vector<Node>::iterator k = l.begin();
        l.erase(k);
    }
    Node top() {
        return l[0];
    }
    void update(Node t) {
        for (int i = 0; i < l.size(); i++) {
            if (l[i].pos.x == t.pos.x && l[i].pos.y == t.pos.y) {
                l[i].f = min(l[i].f, t.f);
            }
        }
    }
    bool empty() {
        return l.size() == 0;
    }
};
class PathFinder {
private:
    NodeList pri_List;
    Point2DI start, end;
    Maze mazer;
    //int mem_g[MAXW][MAXH];1
    //Point2DI path[MAXW][MAXH];

    map2d<int> *mem_g;
    map2d<Point2DI> *path;

public:

    PathFinder(int width, int height) : mazer(width, height){
        mem_g = new map2d<int>(width, height, true);
        path = new map2d<Point2DI>(width, height, true);
    }

    /*void getStartPos() {
        int i, j;
        for (i = 0; i < MAXH; i++)
            for (j = 0; j < MAXW; j++)
                if (maze[i][j] == 'S')
                    sta = M_P(i, j);
    }
    void getEndPos() {
        int i, j;
        for (i = 0; i < MAXH; i++)
            for (j = 0; j < MAXW; j++)
                if (maze[i][j] == 'E')
                    end = M_P(i, j);
    }*/
    void pqClear() {
        pri_List.clear();
    }

    V_PII neighbourPrune(Node c, Agent *agent) {
        V_PII t;
        int i;
        Point2DI dir;
        t.clear();
        if (c.haveParent()) {
            dir = c.getDirection();
            if (dir.x != 0 && dir.y != 0) {
                if (mazer.checkMaze(c.pos.x, c.pos.y + dir.y, agent))
                    t.push_back(Point2DI(c.pos.x, c.pos.y + dir.y));

                if (mazer.checkMaze(c.pos.x + dir.x, c.pos.y, agent))
                    t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y));

                if (mazer.checkMaze(c.pos.x, c.pos.y + dir.y, agent) ||
                    mazer.checkMaze(c.pos.x + dir.x, c.pos.y, agent))
                    t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y + dir.y));

                if (!mazer.checkMaze(c.pos.x - dir.x, c.pos.y, agent) &&
                    mazer.checkMaze(c.pos.x, c.pos.y + dir.y, agent))
                    t.push_back(Point2DI(c.pos.x - dir.x, c.pos.y + dir.y));

                if (!mazer.checkMaze(c.pos.x, c.pos.y - dir.y, agent) &&
                    mazer.checkMaze(c.pos.x + dir.x, c.pos.y, agent))
                    t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y - dir.y));
            } else {
                if (dir.x == 0) {
                    if (mazer.checkMaze(c.pos.x, c.pos.y + dir.y, agent)) {
                        if (mazer.checkMaze(c.pos.x, c.pos.y + dir.y, agent))
                            t.push_back(Point2DI(c.pos.x, c.pos.y + dir.y));

                        if (!mazer.checkMaze(c.pos.x + 1, c.pos.y, agent))
                            t.push_back(Point2DI(c.pos.x + 1, c.pos.y + dir.y));

                        if (!mazer.checkMaze(c.pos.x - 1, c.pos.y, agent))
                            t.push_back(Point2DI(c.pos.x - 1, c.pos.y + dir.y));
                    }
                } else {
                    if (mazer.checkMaze(c.pos.x + dir.x, c.pos.y, agent)) {
                        if (mazer.checkMaze(c.pos.x + dir.x, c.pos.y, agent))
                            t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y));

                        if (!mazer.checkMaze(c.pos.x, c.pos.y + 1, agent))
                            t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y + 1));

                        if (!mazer.checkMaze(c.pos.x, c.pos.y - 1, agent))
                            t.push_back(Point2DI(c.pos.x + dir.x, c.pos.y - 1));
                    }
                }
            }
        } else {
            for (i = 0; i < 8; i++) {
                if (mazer.checkMaze(c.pos.x + g_dir[i][0], c.pos.y + g_dir[i][1], agent))
                    t.push_back(Point2DI(c.pos.x + g_dir[i][0], c.pos.y + g_dir[i][1]));
            }

            if (mazer.checkMaze(c.pos.x + 1, c.pos.y, agent) || mazer.checkMaze(c.pos.x, c.pos.y + 1, agent)) {
                if (mazer.checkMaze(c.pos.x + 1, c.pos.y + 1, agent))
                    t.push_back(Point2DI(c.pos.x + 1, c.pos.y + 1));
            }

            if (mazer.checkMaze(c.pos.x + 1, c.pos.y, agent) || mazer.checkMaze(c.pos.x, c.pos.y - 1, agent)) {
                if (mazer.checkMaze(c.pos.x + 1, c.pos.y - 1, agent)) {
                    t.push_back(Point2DI(c.pos.x + 1, c.pos.y - 1));
                }
            }
            if (mazer.checkMaze(c.pos.x - 1, c.pos.y, agent) || mazer.checkMaze(c.pos.x, c.pos.y + 1, agent)) {
                if (mazer.checkMaze(c.pos.x - 1, c.pos.y + 1, agent))
                    t.push_back(Point2DI(c.pos.x - 1, c.pos.y + 1));
            }
            if (mazer.checkMaze(c.pos.x - 1, c.pos.y, agent) || mazer.checkMaze(c.pos.x, c.pos.y - 1, agent)) {
                if (mazer.checkMaze(c.pos.x - 1, c.pos.y - 1, agent))
                    t.push_back(Point2DI(c.pos.x - 1, c.pos.y - 1));
            }
        }
        for (i = 0; i < t.size(); i++) {
            printf("%d %d\n", t[i].x, t[i].y);
        }
        return t;
    }
    Point2DI checkJump(Point2DI now, Point2DI pre, Agent *agent) {
        Point2DI dir = Point2DI(now.x - pre.x, now.y - pre.y);
        Point2DI tmp = Point2DI(-1, -1);
        if (mazer.checkMaze(now.x, now.y, agent) == false)
            return tmp;
        if (now == end)
            return now;
        if (dir.x != 0 && dir.y != 0) {
            if ((mazer.checkMaze(now.x - dir.x, now.y + dir.y, agent) &&
                 !mazer.checkMaze(now.x - dir.x, now.y, agent)) ||
                (mazer.checkMaze(now.x + dir.x, now.y - dir.y, agent) && !mazer.checkMaze(now.x, now.y - dir.y, agent)))
                return now;
        } else {
            if (dir.x != 0) {
                if ((mazer.checkMaze(now.x + dir.x, now.y + 1, agent) && !mazer.checkMaze(now.x, now.y + 1, agent)) ||
                    (mazer.checkMaze(now.x + dir.x, now.y - 1, agent) && !mazer.checkMaze(now.x, now.y - 1, agent)))
                    return now;
            } else {
                if ((mazer.checkMaze(now.x + 1, now.y + dir.y, agent) && !mazer.checkMaze(now.x + 1, now.y, agent)) ||
                    (mazer.checkMaze(now.x - 1, now.y + dir.y, agent) && !mazer.checkMaze(now.x - 1, now.y, agent)))
                    return now;
            }
        }
        if (dir.x != 0 && dir.y != 0) {
            tmp = checkJump(Point2DI(now.x + dir.x, now.y), now, agent);
            Point2DI t2 = checkJump(Point2DI(now.x, now.y + dir.y), now, agent);
            if (tmp.x != -1 || t2.x != -1)
                return now;
        }
        if (mazer.checkMaze(now.x + dir.x, now.y, agent) || mazer.checkMaze(now.x, now.y + dir.y, agent)) {
            tmp = checkJump(Point2DI(now.x + dir.x, now.y + dir.y), now, agent);
            if (tmp.x != -1)
                return tmp;
        }
        return tmp;
    }
    void expandSuccessors(Node c, Agent *agent) {
        int i, d, g, h;
        Node t;
        Point2DI jp;
        V_PII nbs = neighbourPrune(c, agent);
        for (i = 0; i < nbs.size(); i++) {
            jp = checkJump(nbs[i], c.pos, agent);
            if (jp.x != -1) {
                if (mazer.isInVis(jp) == true)
                    continue;
                d = abs(jp.x - c.pos.x) + abs(jp.y - c.pos.y);
                g = c.g + d;
                if (mazer.isInQue(jp) == false || g < imRef(mem_g,jp.x,jp.y)) {
                    imRef(mem_g, jp.x, jp.y) = g;
                    t.g = g;
                    h = abs(jp.x - end.x) + abs(jp.y - end.y);
                    t.f = t.g + h;
                    t.pos = jp;
                    // printf("%d %d %d %d %d
                    // %d\n",t.pos.x,t.pos.y,t.parent.x,t.parent.y,c.pos.x,c.pos.y);
                    t.parent = c.pos;

                    if (mazer.isInQue(jp) == false) {
                        imRef(path, jp.x, jp.y) = c.pos;
                        pri_List.push(t);
                        mazer.pushQue(t.pos);
                    } else {
                        pri_List.update(t);
                    }
                }
            }
        }
    }
    void backtrace(int x, int y) {
        if (x != start.x && y != start.y) {
            printf("%d %d\n", x, y);
            backtrace(imRef(path, x, y).x, imRef(path, x, y).y);
        } else {
            printf("%d %d\n", x, y);
            return;
        }
    }
    int findPath(Point2DI _start, Point2DI _end, Agent *agent) {

        pqClear();
        start = _start;
        end = _end;
        mazer.clearQue();
        mazer.clearVis();
        memset(mem_g, 0x3f, sizeof(mem_g));

        Node s = Node(start, 0, 0, Point2DI(-1, -1)), tmp;
        pri_List.push(s);
        mazer.pushQue(s.pos);
        while (!pri_List.empty()) {
            tmp = pri_List.top();
            printf("%d %d %d %d %d\n", tmp.pos.x, tmp.pos.y, tmp.parent.x, tmp.parent.y, tmp.f);
            mazer.pushVis(tmp.pos);
            pri_List.pop();
            if (tmp.pos == end) {
                backtrace(end.x, end.y);
                return 1;
            }
            expandSuccessors(tmp, agent);
        }
        return 0;
    }
};