#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0

#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <vector>

std::vector<std::vector<std::pair<int, int>>> normalize(std::vector<std::vector<std::pair<float, float>>>& ansFloat, int scrSize);
void getFaces(std::string file);
void drawLines();

class GLUTVisualizer {
public:
    void init();
    void run(std::string file);
};