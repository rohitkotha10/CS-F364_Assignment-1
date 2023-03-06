#include "visualizer.h"

using namespace std;

int scrSize = 1000;
vector<vector<pair<int, int>>> ans;

vector<vector<pair<int, int>>> normalize(vector<vector<pair<float, float>>>& ansFloat, int scrSize) {
    float maxX = ansFloat[0][0].first;
    float maxY = ansFloat[0][0].second;
    float minX = ansFloat[0][0].first;
    float minY = ansFloat[0][0].second;
    for (int i = 0; i < ansFloat.size(); i++) {
        for (int j = 0; j < ansFloat[i].size(); j++) {
            maxX = max(maxX, ansFloat[i][j].first);
            maxY = max(maxY, ansFloat[i][j].second);
            minX = min(maxX, ansFloat[i][j].first);
            minY = min(maxY, ansFloat[i][j].second);
        };
    }

    float diff = min(minX, minY);
    for (int i = 0; i < ansFloat.size(); i++) {
        for (int j = 0; j < ansFloat[i].size(); j++) {
            ansFloat[i][j].first += diff + 1;
            ansFloat[i][j].second += diff + 1;
        };
    }
    maxX += diff + 1;
    maxY += diff + 1;

    int scaler = (float)(scrSize - 50.0f) / (float)max(maxX, maxY);

    vector<vector<pair<int, int>>> temp(ansFloat.size());
    for (int i = 0; i < ansFloat.size(); i++) {
        for (int j = 0; j < ansFloat[i].size(); j++) {
            int a = scaler * ansFloat[i][j].first;
            int b = scaler * ansFloat[i][j].second;
            temp[i].emplace_back(a, b);
        };
    }

    return temp;
}

void getFaces(string file) {
    ifstream ifs("../tests/output.txt");
    vector<vector<pair<float, float>>> ansFloat;
    int nFaces;
    ifs >> nFaces;
    for (int i = 0; i < nFaces; i++) {
        int n;
        ifs >> n;
        vector<pair<float, float>> vertices(n);
        for (int i = 0; i < n; i++) {
            float a, b;
            ifs >> a >> b;
            vertices[i] = make_pair(a, b);
        }
        vertices.push_back(vertices[0]);
        ansFloat.push_back(vertices);
    }
    ans = normalize(ansFloat, scrSize);
}

void drawLines() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 1.0, 0.0);
    glPointSize(5.0);
    for (int i = 0; i < ans.size(); i++) {
        glBegin(GL_LINES);
        for (int j = 0; j < ans[i].size() - 1; j++) {
            glVertex2i(ans[i][j].first, ans[i][j].second);
            glVertex2i(ans[i][j + 1].first, ans[i][j + 1].second);
        }
        glEnd();
        glFlush();
    }
}

void GLUTVisualizer::init() {
    glClearColor(0, 0, 0, 0);

    glViewport(0, 0, scrSize, scrSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, scrSize, 0, scrSize, 1, -1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLUTVisualizer::run(string file) {
    getFaces(file);

    int argc = 1;
    char* argv[1] = {(char*)""};
    glutInit(&argc, argv);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(scrSize, scrSize);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutCreateWindow("Polygon Decomposition");
    this->init();
    glutDisplayFunc(drawLines);
    glutMainLoop();
}
