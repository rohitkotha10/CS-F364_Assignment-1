#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>

class VertexDCEL;
class EdgeDCEL;
class FaceDCEL;

/**
 * @brief to store vertex information according to DCEL Specification
 */
class VertexDCEL {
public:
    std::pair<float, float> org;
    std::vector<std::shared_ptr<EdgeDCEL>> incEdgeList;
};

/**
 * @brief to store edge information according to DCEL Specification
 */
class EdgeDCEL {
public:
    std::shared_ptr<VertexDCEL> org;
    std::shared_ptr<EdgeDCEL> twin;

    std::shared_ptr<FaceDCEL> leftFace;  // also incident face
    std::shared_ptr<EdgeDCEL> next;      // next edge in face
    std::shared_ptr<EdgeDCEL> prev;      // previous edge in face
};

/**
 * @brief to store face information according to DCEL Specification
 */
class FaceDCEL {
public:
    /**
     * @brief enumerate all vertices in face
     * @return vector containing the vertices in ccw order
     */
    std::vector<std::pair<float, float>> getFaceVertices();

    std::shared_ptr<EdgeDCEL> incEdge;
};

/**
 * @brief stores the polygon object in a DCEL
 */
class DCEL {
public:
    /**
     * @brief create a polygon given vertices in ccw order
     * @param vertices these must be given in counter clockwise order
     */
    void createDCEL(std::vector<std::pair<float, float>> vertices);

    /**
     * @brief prints all the faces along with their vertices
     * @param file output file to be written to
     */
    void printFaces(std::string file);

    /**
     * @brief add an edge to decompose
     * @param e1 first edge
     * @param e2 second edge
     */
    void addEdge(std::shared_ptr<EdgeDCEL> e1, std::shared_ptr<EdgeDCEL> e2);  // passing the indices

    /**
     * @brief check if there is an edge
     * @param e1 first edge
     * @param e2 second edge
     * @return true if there is an edge between origins of e1 and e2
     */
    bool existEdge(std::shared_ptr<EdgeDCEL> e1, std::shared_ptr<EdgeDCEL> e2);

    /**
     * @brief get the index in vertexRecords for ease of traversal and notation
     * @param val the value of vertex
     * @return the poisition i in vertexRecords[i]
     */
    int getVertexInd(std::shared_ptr<VertexDCEL> vert);

    std::vector<std::shared_ptr<VertexDCEL>> vertexRecords;
    std::vector<std::shared_ptr<EdgeDCEL>> edgeRecords;
    std::vector<std::shared_ptr<FaceDCEL>> faceRecords;
};
