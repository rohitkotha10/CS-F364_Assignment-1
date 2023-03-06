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
            viminus1 = polygon.getVertex(n - 1);
        else
            viminus1 = polygon.getVertex(i - 1);
        pair<float, float> vi = polygon.getVertex(i);
        pair<float, float> viplus1 = polygon.getVertex((i + 1) % n);
        if (isReflex(viminus1, vi, viplus1)) ans[i] = 1;
    }
    return ans;
}

float lineEq(pair<float, float> val, float a, float b, float c) {
    return a * val.first + b * val.second + c;
};
int getNewLast(DCEL& polygon, int notch, int first, int last) {
    pair<float, float> notchval = polygon.getVertex(notch);
    pair<float, float> firstval = polygon.getVertex(first);
    pair<float, float> lastval = polygon.getVertex(last);
    float a = notchval.second - firstval.second;
    float b = firstval.first - notchval.first;
    float c = notchval.first * firstval.second - firstval.first * notchval.second;

    float halfval = lineEq(lastval, a, b, c);  // put last in notch - first equation
    int isPos = halfval >= 0;

    for (int i = last; i > first; i--) {
        pair<float, float> cur = polygon.getVertex(i);
        if (isPos == 1) {
            if (lineEq(cur, a, b, c) < 0) { return i; }
        } else {
            if (lineEq(cur, a, b, c) > 0) { return i; }
        }
    }
    return last;
}
int checkNotchInside(DCEL& polygon, int n, int first, int last, vector<int> notches) {
    while (true) {
        int flag = 0;

        vector<int> lpvs;
        for (int i = 0; i < n; i++) {
            if ((i < first || i > last) && notches[i] == 1) lpvs.push_back(i);
        }
        float refvalx = polygon.getVertex(first).first;
        float refvaly = polygon.getVertex(first).second;
        float recxmax = refvalx;
        float recymax = refvaly;
        float recxmin = refvalx;
        float recymin = refvaly;

        for (int i = first; i <= last; i++) {
            pair<float, float> cur = polygon.getVertex(i);
            recxmax = max(recxmax, cur.first);
            recymax = max(recymax, cur.second);
            recxmin = min(recxmin, cur.first);
            recymin = min(recymin, cur.second);
        }
        vector<int> checker(lpvs.size(), 1);
        for (int i = 0; i < lpvs.size(); i++) {
            if (checker[i] == 1) {
                pair<float, float> cur = polygon.getVertex(lpvs[i]);
                if (!(cur.first > recxmax || cur.first < recxmin || cur.second > recymax || cur.second < recymin)) {
                    flag = 1;
                    int temp = getNewLast(polygon, lpvs[i], first, last);
                    if (temp == last)
                        return temp;
                    else
                        last = temp;
                }
            }
            checker[i] = 0;
        }
        if (flag == 0 || last - first <= 2) break;
    }
    return last;
}

void decomposeIntoConvex(DCEL& polygon, int n, vector<int> notches) {
    int start = -1;
    int fir = 0;
    int las = 2;
    int flag = 0;
    for (int i = 0; i < n; i++) {
        pair<float, float> vi = polygon.getVertex(i);
        pair<float, float> viplus1 = polygon.getVertex((i + 1) % n);
        pair<float, float> viplus2 = polygon.getVertex((i + 2) % n);
        pair<float, float> vfir = polygon.getVertex(fir);
        pair<float, float> vfirplus1 = polygon.getVertex((fir + 1) % n);
        if (isReflex(vi, viplus1, viplus2) || isReflex(viplus1, viplus2, vfir) || isReflex(viplus2, vfir, vfirplus1)) {
            if (las - fir == 2) {
                fir++;
                las++;
            } else {
                // process with the rectangle and then add the appropriate part
                int ends = checkNotchInside(polygon, n, fir, las - 1, notches);
                // ends is returning the last point where the polygon ends after division
                if (ends - fir >= 2) {
                    polygon.addEdge(fir, ends);
                    if (start == -1) start = fir;
                    fir = ends;
                    las = (fir + 2) % n;
                } else {
                    fir++;
                    las = (fir + 2) % n;
                }
            }
            i = fir - 1;
        } else {
            las++;
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
    polygon.printFaces("../tests/output.txt");
    GLUTVisualizer vis;
    vis.run("../tests/output.txt");
    return 0;
}
