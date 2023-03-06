#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>

class VertexDCEL;
class EdgeDCEL;
class FaceDCEL;

class VertexDCEL {
public:
    std::pair<float, float> org;
    std::shared_ptr<EdgeDCEL> incEdge;
};

class EdgeDCEL {
public:
    std::shared_ptr<VertexDCEL> org;
    std::shared_ptr<EdgeDCEL> twin;

    std::shared_ptr<FaceDCEL> leftFace;  // also incident face
    std::shared_ptr<EdgeDCEL> next;      // next edge in face
    std::shared_ptr<EdgeDCEL> prev;      // previous edge in face
};

class FaceDCEL {
public:
    std::vector<std::pair<float, float>> getFaceVertices();
    std::shared_ptr<EdgeDCEL> incEdge;
};

class DCEL {
public:
    void createDCEL(std::vector<std::pair<float, float>> vertices);
    void printFaces(std::string file);

private:
    std::vector<std::shared_ptr<VertexDCEL>> vertexRecords;
    std::vector<std::shared_ptr<EdgeDCEL>> edgeRecords;
    std::vector<std::shared_ptr<FaceDCEL>> faceRecords;
};
