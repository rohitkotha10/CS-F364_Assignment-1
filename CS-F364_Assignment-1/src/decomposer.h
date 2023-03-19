#pragma once
#include "dcel.h"

#include <iostream>
#include <fstream>
#include <tuple>

class Decomposer {
public:
    /**
     * @brief get the verts from file into a DCEL structure
     * @param inFile path for input data
     */
    void processVerts(std::string inFile);

    /**
     * @brief enumerate all facerecords while there is no new addition
     */
    void decomposeIntoConvex();

    /**
     * @brief check if there are other vertices inside first-last edge and remove vertices according to algorithm
     * @param first
     * @param last
     * @return
     */
    std::pair<int, std::shared_ptr<EdgeDCEL>> checkNotchInside(
        std::shared_ptr<EdgeDCEL>& first, std::shared_ptr<EdgeDCEL>& last);

    /**
     * @brief shrink rectangle by removing vertices which are unnecessary
     * @param notch
     * @param first
     * @param last
     * @return
     */
    std::shared_ptr<EdgeDCEL> getNewLast(int notch, std::shared_ptr<EdgeDCEL>& first, std::shared_ptr<EdgeDCEL>& last);

    /**
     * @brief report all notches in polygon. notch is one which is a reflex angle
     * @return
     */
    void findNotches();

    DCEL polygon;
    int vertSize;
    std::vector<int> notches;
};

// some helper functions

/**
 * @brief check if angle >= 180 degrees using cross product
 * @param v1
 * @param v2
 * @param v3
 * @return
 */
bool isReflex(std::pair<float, float> v1, std::pair<float, float> v2, std::pair<float, float> v3);

/**
 * @brief lineEquation to check vertex in which halfplane
 * @param val the point to check
 * @param a a in a*x+b*y+c line equation
 * @param b b in a*x+b*y+c line equation
 * @param c c in a*x+b*y+c line equation
 * @return greater or less than zero determining which halfplane
 */
float lineEq(std::pair<float, float> val, float a, float b, float c);
