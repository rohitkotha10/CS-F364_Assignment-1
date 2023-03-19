#include "decomposer.h"

using namespace std;

bool isReflex(pair<float, float> v1, pair<float, float> v2, pair<float, float> v3) {
    pair<float, float> vec1 = make_pair(v2.first - v1.first, v2.second - v1.second);
    pair<float, float> vec2 = make_pair(v3.first - v2.first, v3.second - v2.second);
    float cross = vec1.first * vec2.second - vec1.second * vec2.first;
    return cross < 0;
}

float lineEq(pair<float, float> val, float a, float b, float c) {
    return a * val.first + b * val.second + c;
}

void Decomposer::processVerts(string inFile) {
    ifstream ifs(inFile);
    int n;
    ifs >> n;
    vector<pair<float, float>> ans(n);
    for (int i = 0; i < n; i++) {
        float a, b;
        ifs >> a >> b;
        ans[i] = make_pair(a, b);
    }

    polygon.createDCEL(ans);
    this->vertSize = n;
    findNotches();
}

void Decomposer::findNotches() {
    notches = vector<int>(vertSize, 0);
    for (int i = 0; i < vertSize; i++) {
        pair<float, float> viminus1;
        if (i == 0)
            viminus1 = polygon.vertexRecords[vertSize - 1]->org;
        else
            viminus1 = polygon.vertexRecords[i - 1]->org;
        pair<float, float> vi = polygon.vertexRecords[i]->org;
        pair<float, float> viplus1 = polygon.vertexRecords[(i + 1) % vertSize]->org;
        if (isReflex(viminus1, vi, viplus1)) notches[i] = 1;
    }
}

void Decomposer::decomposeIntoConvex() {
    while (true) {
        int flag = 0;

        shared_ptr<EdgeDCEL> curEdge = polygon.faceRecords[1]->incEdge;
        shared_ptr<EdgeDCEL> efir = curEdge;
        shared_ptr<EdgeDCEL> efirplus1 = curEdge->next;
        int startWhile = 1;  // to pass while loop and continue from edge start
        int pathdis = 2;

        // terminate when the face is looped and we return back
        while (startWhile == 1 || efir.get() != polygon.faceRecords[1]->incEdge.get()) {
            if (pathdis > vertSize) break;
            if (startWhile == 1 && efir.get() != polygon.faceRecords[1]->incEdge.get()) { startWhile = 0; }
            shared_ptr<EdgeDCEL> ei = curEdge;
            shared_ptr<EdgeDCEL> eiplus1 = curEdge->next;
            shared_ptr<EdgeDCEL> eiplus2 = curEdge->next->next;
            if (isReflex(ei->org->org, eiplus1->org->org, eiplus2->org->org) ||
                isReflex(eiplus1->org->org, eiplus2->org->org, efir->org->org) ||
                isReflex(eiplus2->org->org, efir->org->org, efirplus1->org->org)) {
                if (pathdis == 2) {
                    curEdge = curEdge->next;
                    efir = curEdge;
                    efirplus1 = curEdge->next;
                } else {
                    // process with the rectangle and then add the appropriate part
                    // ends is returning the last point where the polygon ends after division and the path length
                    pair<int, shared_ptr<EdgeDCEL>> endPath = checkNotchInside(efir, eiplus1);
                    if (endPath.first >= 2) {
                        if (polygon.existEdge(efir, endPath.second))  // stop looping face
                            break;
                        else {
                            polygon.addEdge(efir, endPath.second);
                            flag = 1;
                            curEdge = endPath.second;
                            efir = curEdge;
                            efirplus1 = curEdge->next;
                            pathdis = 2;
                        }
                    } else {
                        efir = efir->next;
                        curEdge = efir;
                        efirplus1 = efir->next;
                        pathdis = 2;
                    }
                }
            } else {
                pathdis++;
                curEdge = curEdge->next;
            }
        }

        if (flag == 0) return;
    }
}

pair<int, shared_ptr<EdgeDCEL>> Decomposer::checkNotchInside(shared_ptr<EdgeDCEL>& first, shared_ptr<EdgeDCEL>& last) {
    vector<int> lpvs;
    vector<int> isthere(vertSize, 0);

    shared_ptr<EdgeDCEL> cur = first;
    while (cur != last->next) {
        isthere[polygon.getVertexInd(cur->org)] = 1;
        cur = cur->next;
    }

    for (int i = 0; i < vertSize; i++) {
        if (isthere[i] == 0 && notches[i] == 1) lpvs.push_back(i);
    }

    for (int i = 0; i < lpvs.size(); i++) {
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

        pair<float, float> vertLp = polygon.vertexRecords[lpvs[i]]->org;
        if (!(vertLp.first > recxmax || vertLp.first < recxmin || vertLp.second > recymax || vertLp.second < recymin)) {
            last = getNewLast(lpvs[i], first, last);
        }
    }

    int pathdis = 0;
    cur = first;
    while (cur != last) {
        pathdis++;
        cur = cur->next;
    }
    return make_pair(pathdis, last);
}

shared_ptr<EdgeDCEL> Decomposer::getNewLast(int notch, shared_ptr<EdgeDCEL>& first, shared_ptr<EdgeDCEL>& last) {
    pair<float, float> notchval = polygon.vertexRecords[notch]->org;
    pair<float, float> firstval = first->org->org;
    pair<float, float> lastval = last->org->org;
    float a = notchval.second - firstval.second;
    float b = firstval.first - notchval.first;
    float c = notchval.first * firstval.second - firstval.first * notchval.second;

    float halfval = lineEq(lastval, a, b, c);  // put last in (notch - first) equation

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
