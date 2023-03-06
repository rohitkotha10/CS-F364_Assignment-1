#include "dcel.h"
#include "visualizer.h"
#include <iostream>
#include <fstream>
using namespace std;

vector<pair<float, float>> getVertices(string inFile) {
    ifstream ifs(inFile);
    int n;
    ifs >> n;
    vector<pair<float, float>> ans(n);
    for (int i = 0; i < n; i++) {
        float a, b;
        ifs >> a >> b;
        ans[i] = make_pair(a, b);
    }
    return ans;
}

void decomposeIntoConvex(DCEL& polygon, int n) {
    for (int i = 0; i < n; i++) {}
}

int main() {
    vector<pair<float, float>> verts = getVertices("../tests/input.txt");
    int n = verts.size();
    DCEL polygon;
    polygon.createDCEL(verts, n);
    decomposeIntoConvex(polygon);
    polygon.printFaces("../tests/output.txt");
    GLUTVisualizer vis;
    vis.run("../tests/output.txt");
    return 0;
}
