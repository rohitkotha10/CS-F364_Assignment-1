#include "dcel.h"
#include "visualizer.h"
#include <iostream>
#include <fstream>
#include <tuple>
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
            viminus1 = polygon.vertexRecords[n - 1]->org;
        else
            viminus1 = polygon.vertexRecords[i - 1]->org;
        pair<float, float> vi = polygon.vertexRecords[i]->org;
        pair<float, float> viplus1 = polygon.vertexRecords[(i + 1) % n]->org;
        if (isReflex(viminus1, vi, viplus1)) ans[i] = 1;
    }
    return ans;
}

float lineEq(pair<float, float> val, float a, float b, float c) {
    return a * val.first + b * val.second + c;
}

shared_ptr<EdgeDCEL> getNewLast(
    DCEL& polygon, int n, int notch, shared_ptr<EdgeDCEL>& first, shared_ptr<EdgeDCEL>& last) {
    pair<float, float> notchval = polygon.vertexRecords[notch]->org;
    pair<float, float> firstval = first->org->org;
    pair<float, float> lastval = last->org->org;
    float a = notchval.second - firstval.second;
    float b = firstval.first - notchval.first;
    float c = notchval.first * firstval.second - firstval.first * notchval.second;

    float halfval = lineEq(lastval, a, b, c);  // put last in notch - first equation

    shared_ptr<EdgeDCEL> cur = last;
    while (cur != first) {
        pair<float, float> temp = cur->org->org;
        if (halfval > 0) {
            if (lineEq(temp, a, b, c) < 0) { return cur; }
        } else if (halfval < 0) {
            if (lineEq(temp, a, b, c) > 0) { return cur; }
        }
        cur = cur->prev;
    }
    // this means all vertices including last in same halfplane
    return last;
}

pair<int, shared_ptr<EdgeDCEL>> checkNotchInside(
    DCEL& polygon, int n, shared_ptr<EdgeDCEL>& first, shared_ptr<EdgeDCEL>& last, vector<int>& notches) {
    while (true) {
        int flag = 0;

        vector<int> lpvs;
        vector<int> isthere(n, 0);
        shared_ptr<EdgeDCEL> cur = first;
        while (cur != last->next) {
            isthere[polygon.getVertexInd(cur->org->org)] = 1;
            cur = cur->next;
        }

        for (int i = 0; i < n; i++) {
            if (isthere[i] == 0 && notches[i] == 1) lpvs.push_back(i);
        }

        float refvalx = first->org->org.first;
        float refvaly = first->org->org.second;
        float recxmax = refvalx;
        float recymax = refvaly;
        float recxmin = refvalx;
        float recymin = refvaly;

        cur = first;
        while (cur != last->next) {
            pair<float, float> temp = cur->org->org;
            recxmax = max(recxmax, temp.first);
            recymax = max(recymax, temp.second);
            recxmin = min(recxmin, temp.first);
            recymin = min(recymin, temp.second);
            cur = cur->next;
        }

        vector<int> checker(lpvs.size(), 1);
        for (int i = 0; i < lpvs.size(); i++) {
            if (checker[i] == 1) {
                pair<float, float> cur = polygon.vertexRecords[lpvs[i]]->org;
                if (!(cur.first > recxmax || cur.first < recxmin || cur.second > recymax || cur.second < recymin)) {
                    shared_ptr<EdgeDCEL> temp = getNewLast(polygon, n, lpvs[i], first, last);
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
    shared_ptr<EdgeDCEL> cur = first;
    while (cur != last) {
        pathdis++;
        cur = cur->next;
    }
    return make_pair(pathdis, last);
}

void decomposeIntoConvex(DCEL& polygon, int n, vector<int>& notches) {
    while (true) {
        int flag = 0;
        for (int i = 1; i < polygon.faceRecords.size(); i++) {
            shared_ptr<EdgeDCEL> curEdge = polygon.faceRecords[i]->incEdge;
            shared_ptr<EdgeDCEL> vfir = curEdge;
            shared_ptr<EdgeDCEL> vfirplus1 = curEdge->next;
            curEdge = curEdge->next;
            int edgeChange = 1;
            int pathdis = 2;
            while (curEdge.get() != polygon.faceRecords[i]->incEdge.get()) {
                if (edgeChange == 1) {
                    edgeChange = 0;
                    curEdge = polygon.faceRecords[i]->incEdge;
                }
                shared_ptr<EdgeDCEL> vi = curEdge;
                shared_ptr<EdgeDCEL> viplus1 = curEdge->next;
                shared_ptr<EdgeDCEL> viplus2 = curEdge->next->next;
                if (isReflex(vi->org->org, viplus1->org->org, viplus2->org->org) ||
                    isReflex(viplus1->org->org, viplus2->org->org, vfir->org->org) ||
                    isReflex(viplus2->org->org, vfir->org->org, vfirplus1->org->org)) {
                    if (pathdis == 2) {
                        curEdge = curEdge->next;
                        vfir = curEdge;
                        vfirplus1 = curEdge->next;
                    } else {
                        // process with the rectangle and then add the appropriate part
                        // ends is returning the last point where the polygon ends after division and the path length
                        pair<int, shared_ptr<EdgeDCEL>> endPath = checkNotchInside(polygon, n, vfir, viplus1, notches);
                        if (endPath.first >= 2) {
                            int firInd = polygon.getVertexInd(vfir->org->org);
                            int lasInd = polygon.getVertexInd(endPath.second->org->org);

                            curEdge = endPath.second;
                            vfir = curEdge;
                            vfirplus1 = curEdge->next;
                            pathdis = 2;

                            if (firInd == -1 || lasInd == -1) {
                                cout << "ERROR" << endl;
                                return;
                            }
                            if (polygon.existEdge(firInd, lasInd))
                                break;
                            else
                                polygon.addEdge(firInd, lasInd);
                        } else {
                            vfir = vfir->next;
                            curEdge = vfir;
                            vfirplus1 = vfir->next;
                            pathdis = 2;
                        }
                    }
                } else {
                    pathdis++;
                    curEdge = curEdge->next;
                }
            }
        }
        if (flag == 0) return;
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
