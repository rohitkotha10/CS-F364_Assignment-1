#include "dcel.h"

using namespace std;

void DCEL::createDCEL(vector<pair<float, float>> vertices) {
    shared_ptr<FaceDCEL> faceInf = make_shared<FaceDCEL>();
    shared_ptr<FaceDCEL> faceInitial = make_shared<FaceDCEL>();
    for (int i = 0; i < vertices.size(); i++) {
        shared_ptr<VertexDCEL> ver = make_shared<VertexDCEL>();
        shared_ptr<EdgeDCEL> edg = make_shared<EdgeDCEL>();
        ver->org = vertices[i];
        ver->incEdgeList.push_back(edg);
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

void DCEL::addEdge(shared_ptr<EdgeDCEL> e1, shared_ptr<EdgeDCEL> e2) {
    // the four ptrs below always satisy as we split the face we need in ccw order
    if (e1->leftFace.get() != e2->leftFace.get()) {
        cout << "Bad Edge" << endl;
        return;
    }
    shared_ptr<FaceDCEL> oldface = e1->leftFace;

    shared_ptr<FaceDCEL> faceNew = make_shared<FaceDCEL>();
    shared_ptr<EdgeDCEL> nextj = e2;
    shared_ptr<EdgeDCEL> prevj = e2->prev;
    shared_ptr<EdgeDCEL> nexti = e1;
    shared_ptr<EdgeDCEL> previ = e1->prev;

    shared_ptr<EdgeDCEL> cur = nexti;
    while (cur.get() != nextj.get()) {
        cur->leftFace = faceNew;
        cur = cur->next;
    }

    shared_ptr<EdgeDCEL> edgeNew = make_shared<EdgeDCEL>();
    shared_ptr<EdgeDCEL> edgeNewTwin = make_shared<EdgeDCEL>();

    faceNew->incEdge = e1;
    oldface->incEdge = edgeNew;

    edgeNew->org = e1->org;
    edgeNewTwin->org = e2->org;

    edgeNew->twin = edgeNewTwin;
    edgeNewTwin->twin = edgeNew;

    edgeNew->next = nextj;
    nextj->prev = edgeNew;

    edgeNew->prev = previ;
    previ->next = edgeNew;

    edgeNew->leftFace = oldface;

    edgeNewTwin->next = nexti;
    nexti->prev = edgeNewTwin;

    edgeNewTwin->prev = prevj;
    prevj->next = edgeNewTwin;

    edgeNewTwin->leftFace = faceNew;

    faceRecords.push_back(faceNew);
    edgeRecords.push_back(edgeNew);
    edgeRecords.push_back(edgeNewTwin);
}

bool DCEL::existEdge(shared_ptr<EdgeDCEL> e1, shared_ptr<EdgeDCEL> e2) {
    shared_ptr<VertexDCEL> vert1 = e1->org;
    shared_ptr<VertexDCEL> vert2 = e2->org;
    for (int i = 0; i < edgeRecords.size(); i++) {
        if (vert1.get() == edgeRecords[i]->org.get() && vert2.get() == edgeRecords[i]->twin->org.get()) return true;
    }
    return false;
}

int DCEL::getVertexInd(shared_ptr<VertexDCEL> vert) {
    for (int i = 0; i < vertexRecords.size(); i++) {
        if (vertexRecords[i].get() == vert.get()) return i;
    }
    cout << "ERROR getVertexInd" << endl;
    return -1;
}
