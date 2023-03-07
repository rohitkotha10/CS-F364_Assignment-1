#include "dcel.h"

using namespace std;

void DCEL::createDCEL(vector<pair<float, float>> vertices) {
    shared_ptr<FaceDCEL> faceInf = make_shared<FaceDCEL>();
    shared_ptr<FaceDCEL> faceInitial = make_shared<FaceDCEL>();
    for (int i = 0; i < vertices.size(); i++) {
        shared_ptr<VertexDCEL> ver = make_shared<VertexDCEL>();
        shared_ptr<EdgeDCEL> edg = make_shared<EdgeDCEL>();
        ver->org = vertices[i];
        ver->incEdge = edg;
        edg->org = ver;
        edg->leftFace = faceInitial;
        vertexRecords.push_back(ver);
        edgeRecords.push_back(edg);
    }

    for (int i = 0; i < vertices.size(); i++) {
        shared_ptr<EdgeDCEL> edg = make_shared<EdgeDCEL>();
        if (i == vertices.size() - 1)
            edg->org = vertexRecords[0];
        else
            edg->org = vertexRecords[i + 1];
        edg->leftFace = faceInf;
        edg->twin = edgeRecords[i];
        edgeRecords[i]->twin = edg;
        edgeRecords.push_back(edg);
    }

    for (int i = 0; i < vertices.size(); i++) {
        if (i == vertices.size() - 1)
            edgeRecords[i]->next = edgeRecords[0];
        else
            edgeRecords[i]->next = edgeRecords[i + 1];
        if (i == 0)
            edgeRecords[i]->prev = edgeRecords[vertices.size() - 1];
        else
            edgeRecords[i]->prev = edgeRecords[i - 1];
    }

    for (int i = vertices.size(); i < 2 * vertices.size(); i++) {
        if (i == 2 * vertices.size() - 1)
            edgeRecords[i]->prev = edgeRecords[0];
        else
            edgeRecords[i]->prev = edgeRecords[i + 1];
        if (i == vertices.size())
            edgeRecords[i]->next = edgeRecords[2 * vertices.size() - 1];
        else
            edgeRecords[i]->next = edgeRecords[i - 1];
    }
    faceInf->incEdge = edgeRecords.back();
    faceInitial->incEdge = edgeRecords[0];
    faceRecords.push_back(faceInf);
    faceRecords.push_back(faceInitial);
}

vector<pair<float, float>> FaceDCEL::getFaceVertices() {
    vector<pair<float, float>> ans;
    shared_ptr<EdgeDCEL> startEdge = this->incEdge;
    ans.emplace_back(startEdge->org->org.first, startEdge->org->org.second);
    startEdge = startEdge->next;
    while (startEdge.get() != incEdge.get()) {
        ans.emplace_back(startEdge->org->org.first, startEdge->org->org.second);
        startEdge = startEdge->next;
    }

    return ans;
}

void DCEL::printFaces(string file) {
    ofstream ofs(file);
    ofs << faceRecords.size() << endl;
    for (int i = 0; i < faceRecords.size(); i++) {
        vector<pair<float, float>> cur = faceRecords[i]->getFaceVertices();
        ofs << cur.size() << endl;
        for (int i = 0; i < cur.size(); i++) {
            ofs << cur[i].first << ' ' << cur[i].second;
            ofs << endl;
        }
    }
}

void DCEL::addEdge(int i, int j) {
    shared_ptr<VertexDCEL> v1 = vertexRecords[i];
    shared_ptr<VertexDCEL> v2 = vertexRecords[j];

    // the four ptrs below always satisy as we split the face we need in ccw order
    shared_ptr<FaceDCEL> oldface = v1->incEdge->leftFace;

    shared_ptr<FaceDCEL> faceNew = make_shared<FaceDCEL>();
    shared_ptr<EdgeDCEL> nextj = v2->incEdge;
    shared_ptr<EdgeDCEL> prevj = nextj->prev;
    shared_ptr<EdgeDCEL> nexti = v1->incEdge;
    shared_ptr<EdgeDCEL> previ = nexti->prev;

    shared_ptr<EdgeDCEL> cur = nexti;
    while (cur.get() != nextj.get()) {
        cur->leftFace = faceNew;
        cur = cur->next;
    }

    shared_ptr<EdgeDCEL> edgeNew = make_shared<EdgeDCEL>();
    shared_ptr<EdgeDCEL> edgeNewTwin = make_shared<EdgeDCEL>();
    faceNew->incEdge = edgeNewTwin;
    oldface->incEdge = edgeNew;

    edgeNew->org = v1;
    v1->incEdge = edgeNew;
    edgeNewTwin->org = v2;
    edgeNew->twin = edgeNewTwin;
    edgeNewTwin->twin = edgeNew;

    edgeNew->next = nextj;
    edgeNew->prev = previ;
    edgeNew->leftFace = oldface;

    edgeNewTwin->next = nexti;
    edgeNewTwin->prev = prevj;
    edgeNewTwin->leftFace = faceNew;

    previ->next = edgeNew;
    nexti->prev = edgeNewTwin;
    prevj->next = edgeNewTwin;
    nextj->prev = edgeNew;

    faceRecords.push_back(faceNew);
    edgeRecords.push_back(edgeNew);
    edgeRecords.push_back(edgeNewTwin);
}

bool DCEL::existEdge(int st, int en) {
    for (int i = 0; i < edgeRecords.size(); i++) {
        if (vertexRecords[st].get() == edgeRecords[i]->org.get() &&
            vertexRecords[en].get() == edgeRecords[i]->twin->org.get())
            return true;
    }
    return false;
}

int DCEL::getVertexInd(pair<float, float> val) {
    for (int i = 0; i < vertexRecords.size(); i++) {
        if (vertexRecords[i]->org == val) return i;
    }
    return -1;
}
