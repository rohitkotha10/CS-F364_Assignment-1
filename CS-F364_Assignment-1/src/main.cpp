#include "dcel.h"
#include "decomposer.h"
#include "visualizer.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * @brief first take input from glutfunction as a drawing in ccw order, then process and draw the decomposition
 * @return
 */
int main() {
    GLUTVisualizer vis;
    vis.runInput();  // input data written to text file

    Decomposer dec;
    dec.processVerts("../tests/inputGLUT.txt");
    dec.decomposeIntoConvex();
    dec.polygon.printFaces("../tests/output.txt");

    vis.runOutput();  // output data read from text file
    return 0;
}
