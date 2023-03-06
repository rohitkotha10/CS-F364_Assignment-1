#include "dcel.h"

using namespace std;

vector<pair<float, float>> FaceDCEL::getFaceVertices() {
    vector<pair<float, float>> ans;
    shared_ptr<EdgeDCEL> startEdge = this->incEdge;
    while (startEdge->next.get() != incEdge.get()) {
        ans.emplace_back(startEdge->org->org.first, startEdge->org->org.second);
        startEdge = startEdge->next;
    }
    ans.emplace_back(startEdge->org->org.first, startEdge->org->org.second);

    return ans;
}

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

    if (faceRecords.size() > 2 && v2->incEdge.get() == faceRecords[2]->incEdge.get()) {
        if (faceRecords[2]->incEdge->prev->org.get() == v1.get())
            return;
        else {
            shared_ptr<EdgeDCEL> edgeStart = v1->incEdge;
            shared_ptr<FaceDCEL> faceNew = make_shared<FaceDCEL>();
            faceNew->incEdge = edgeStart;

            shared_ptr<EdgeDCEL> edgeNew = make_shared<EdgeDCEL>();
            shared_ptr<EdgeDCEL> edgeNewTwin = make_shared<EdgeDCEL>();

            edgeStart->leftFace = faceNew;
            edgeStart = edgeStart->next;
            while (edgeStart.get() != v1->incEdge.get()) {
                edgeStart->leftFace = faceNew;
                edgeStart = edgeStart->next;
            }

            edgeNew->org = v1;
            edgeNewTwin->org = v2;
            edgeNew->twin = edgeNewTwin;
            edgeNewTwin->twin = edgeNew;

            edgeNew->next = v2->incEdge->prev->twin;
            edgeNew->prev = v1->incEdge->prev;
            edgeNew->leftFace = faceRecords[1];

            edgeNewTwin->next = v1->incEdge;
            edgeNewTwin->prev = v2->incEdge->prev->twin->prev;
            edgeNewTwin->leftFace = faceNew;

            v1->incEdge->prev->next = edgeNew;
            v2->incEdge->prev->twin->prev->next = edgeNewTwin;
            v1->incEdge->prev = edgeNewTwin;
            v2->incEdge->prev->twin->prev = edgeNew;

            faceRecords.push_back(faceNew);
            edgeRecords.push_back(edgeNew);
            edgeRecords.push_back(edgeNewTwin);
        }

    } else {
        shared_ptr<EdgeDCEL> edgeStart = v1->incEdge;
        shared_ptr<FaceDCEL> faceNew = make_shared<FaceDCEL>();
        faceNew->incEdge = edgeStart;

        shared_ptr<EdgeDCEL> edgeNew = make_shared<EdgeDCEL>();
        shared_ptr<EdgeDCEL> edgeNewTwin = make_shared<EdgeDCEL>();

        while (edgeStart.get() != v2->incEdge.get()) {
            edgeStart->leftFace = faceNew;
            edgeStart = edgeStart->next;
        }

        edgeNew->org = v1;
        edgeNewTwin->org = v2;
        edgeNew->twin = edgeNewTwin;
        edgeNewTwin->twin = edgeNew;

        edgeNew->next = v2->incEdge;
        edgeNew->prev = v1->incEdge->prev;
        edgeNew->leftFace = faceRecords[1];

        edgeNewTwin->next = v1->incEdge;
        edgeNewTwin->prev = v2->incEdge->prev;
        edgeNewTwin->leftFace = faceNew;

        v1->incEdge->prev->next = edgeNew;
        v2->incEdge->prev->next = edgeNewTwin;
        v1->incEdge->prev = edgeNewTwin;
        v2->incEdge->prev = edgeNew;

        faceRecords.push_back(faceNew);
        edgeRecords.push_back(edgeNew);
        edgeRecords.push_back(edgeNewTwin);
    }
}
