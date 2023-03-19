#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0

#include <GL/freeglut.h>

#include <iostream>
#include <fstream>
#include <vector>

/**
 * @brief GLUT helper class to visualize polygons
 */
class GLUTVisualizer {
public:
    void init();
    void runOutput();
    void runInput();
};
