#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0

#include <GL/freeglut.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief GLUT helper class to visualize polygons
 */
class GLUTVisualizer {
public:
    void runInput(std::string inFile);
    void runOutput(std::string outFile);
};
