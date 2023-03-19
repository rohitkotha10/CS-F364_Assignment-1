#include "decomposer.h"
#include "visualizer.h"

using namespace std;

/**
 * @brief first take input from glutfunction as a drawing in ccw order, then process and draw the decomposition
 * @return
 */
int main() {
    string inFile = "../tests/inputGLUT.txt";
    string outFile = "../tests/output.txt";

    GLUTVisualizer vis;
    // make sure you draw the polygon in counter-clockwise order
    vis.runInput(inFile);  // input data written to text file

    Decomposer dec;
    dec.processVerts(inFile);
    dec.decomposeIntoConvex();
    dec.polygon.printFaces(outFile);

    vis.runOutput(outFile);  // output data read from text file
    return 0;
}
