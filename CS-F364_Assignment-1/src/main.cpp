#include "dcel.h"
#include "visualizer.h"
#include <iostream>
#include <fstream>
using namespace std;

vector<pair<float, float>> getVertices(string inFile) {
    ifstream ifs(inFile);
    int n;
    ifs >> n;
    vector<pair<float, float>> ans(n);
    for (int i = 0; i < n; i++) {
        float a, b;
        ifs >> a >> b;
        ans[i] = make_pair(a, b);
    }
    return ans;
}

bool isReflex(pair<float, float> v1, pair<float, float> v2, pair<float, float> v3) {
    pair<float, float> vec1 = make_pair(v2.first - v1.first, v2.second - v1.second);
    pair<float, float> vec2 = make_pair(v3.first - v2.first, v3.second - v2.second);
    float cross = vec1.first * vec2.second - vec1.second * vec2.first;
    return cross < 0;
}

vector<int> findNotches(DCEL& polygon, int n) {
    vector<int> ans(n, 0);
    for (int i = 0; i < n; i++) {
        pair<float, float> viminus1;
        if (i == 0)
            viminus1 = polygon.getVertexVal(n - 1);
        else
            viminus1 = polygon.getVertexVal(i - 1);
        pair<float, float> vi = polygon.getVertexVal(i);
        pair<float, float> viplus1 = polygon.getVertexVal((i + 1) % n);
        if (isReflex(viminus1, vi, viplus1)) ans[i] = 1;
    }
    return ans;
}

float lineEq(pair<float, float> val, float a, float b, float c) {
    return a * val.first + b * val.second + c;
}

int getNewLast(DCEL& polygon, int n, int notch, int first, int last) {
    pair<float, float> notchval = polygon.getVertexVal(notch);
    pair<float, float> firstval = polygon.getVertexVal(first);
    pair<float, float> lastval = polygon.getVertexVal(last);
    float a = notchval.second - firstval.second;
    float b = firstval.first - notchval.first;
    float c = notchval.first * firstval.second - firstval.first * notchval.second;

    float halfval = lineEq(lastval, a, b, c);  // put last in notch - first equation

    int cur = last;
    while (cur != first) {
        pair<float, float> temp = polygon.getVertexVal(cur);
        if (halfval > 0) {
            if (lineEq(temp, a, b, c) < 0) { return cur; }
        } else if (halfval < 0) {
            if (lineEq(temp, a, b, c) > 0) { return cur; }
        }
        cur = polygon.getPrevVertex(cur);
    }
    // this means all vertices including last in same halfplane
    return last;
}

pair<int, int> checkNotchInside(DCEL& polygon, int n, int first, int last, vector<int> notches) {
    while (true) {
        int flag = 0;

        vector<int> lpvs;
        vector<int> isthere(n, 0);
        int cur = first;
        while (cur != polygon.getNextVertex(last)) {
            isthere[cur] = 1;
            cur = polygon.getNextVertex(cur);
        }
        for (int i = 0; i < n; i++) {
            if (isthere[i] == 0 && notches[i] == 1) lpvs.push_back(i);
        }

        float refvalx = polygon.getVertexVal(first).first;
        float refvaly = polygon.getVertexVal(first).second;
        float recxmax = refvalx;
        float recymax = refvaly;
        float recxmin = refvalx;
        float recymin = refvaly;

        cur = first;
        while (cur != polygon.getNextVertex(last)) {
            pair<float, float> temp = polygon.getVertexVal(cur);
            recxmax = max(recxmax, temp.first);
            recymax = max(recymax, temp.second);
            recxmin = min(recxmin, temp.first);
            recymin = min(recymin, temp.second);
            cur = polygon.getNextVertex(cur);
        }
        vector<int> checker(lpvs.size(), 1);
        for (int i = 0; i < lpvs.size(); i++) {
            if (checker[i] == 1) {
                pair<float, float> cur = polygon.getVertexVal(lpvs[i]);
                if (!(cur.first > recxmax || cur.first < recxmin || cur.second > recymax || cur.second < recymin)) {
                    int temp = getNewLast(polygon, n, lpvs[i], first, last);
                    if (temp != last) {
                        flag = 1;
                        last = temp;
                    }
                }
            }
            checker[i] = 0;
        }
        if (flag == 0) break;
    }
    int pathdis = 0;
    int cur = first;
    while (cur != last) {
        pathdis++;
        cur = polygon.getNextVertex(cur);
    }
    return make_pair(pathdis, last);
}

void decomposeIntoConvex(DCEL& polygon, int n, vector<int> notches) {
    int fir = 0;
    int curVertex = 0;
    int pathdis = 2;  // from fir to las initially i.e vi to vi+2
    while (true) {
        int inflag = 0;
        pair<float, float> vi = polygon.getVertexVal(curVertex);
        pair<float, float> vfir = polygon.getVertexVal(fir);
        int viplus1ind = polygon.getNextVertex(curVertex);
        int viplus2ind = polygon.getNextVertex(viplus1ind);
        int las = viplus2ind;
        int vfirplus1ind = polygon.getNextVertex(fir);
        pair<float, float> viplus1 = polygon.getVertexVal(viplus1ind);
        pair<float, float> viplus2 = polygon.getVertexVal(viplus2ind);
        pair<float, float> vfirplus1 = polygon.getVertexVal(vfirplus1ind);
        if (isReflex(vi, viplus1, viplus2) || isReflex(viplus1, viplus2, vfir) || isReflex(viplus2, vfir, vfirplus1)) {
            if (pathdis == 2) {
                fir = polygon.getNextVertex(fir);
                curVertex = fir;
            } else {
                // process with the rectangle and then add the appropriate part
                pair<int, int> endPath = checkNotchInside(polygon, n, fir, polygon.getPrevVertex(las), notches);
                // ends is returning the last point where the polygon ends after division and the path length
                if (endPath.first >= 2) {
                    if (polygon.existEdge(fir, endPath.second))
                        break;
                    else
                        polygon.addEdge(fir, endPath.second);
                    inflag = 1;
                    fir = endPath.second;
                    curVertex = fir;
                } else {
                    fir = polygon.getNextVertex(fir);
                    curVertex = fir;
                }
            }
        } else {
            pathdis++;
            curVertex = polygon.getNextVertex(curVertex);
            if (las == fir) break;
        }
    }
}

int main() {
    vector<pair<float, float>> verts = getVertices("../tests/input.txt");
    int n = verts.size();
    DCEL polygon;
    polygon.createDCEL(verts);
    vector<int> notches = findNotches(polygon, n);
    decomposeIntoConvex(polygon, n, notches);
    // polygon.addEdge(0, 2);
    polygon.printFaces("../tests/output.txt");
    GLUTVisualizer vis;
    vis.run("../tests/output.txt");
    return 0;
}
