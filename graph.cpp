#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const int nCities = 6;
const int src = 0;
bool isStarted = false;
static int cost = 0;
mutex edgeMutex;

void computeEdgeCost(int v, int i, vector<int>& subset, int* temp, int* mincost, int* path);

GLfloat arr[nCities][2] = {
    {-0.5f, 0.3f},
    {0.0f, 0.7f},
    {0.5f, 0.3f},
    {0.5f, -0.3f},
    {0.0f, -0.7f},
    {-0.5f, -0.3f}
};

int graph[nCities][nCities] = {
    {0, 12, 14, 12, 13, 15},
    {12, 0, 13, 14, 13, 12},
    {14, 13, 0, 12, 14, 13},
    {12, 14, 12, 0, 12, 13},
    {13, 13, 14, 12, 0, 14},
    {13, 12, 13, 13, 14, 0}
};

void highlightEdge(int a, int b) {
    lock_guard<mutex> lock(edgeMutex);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2fv(arr[a]);
    glVertex2fv(arr[b]);
    glEnd();
}

void dehighlightEdge(int a, int b) {
    lock_guard<mutex> lock(edgeMutex);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2fv(arr[a]);
    glVertex2fv(arr[b]);
    glEnd();
}

int tsp(int v, vector<int>* set, int path[]) {
    if (set->empty()) {
        return graph[v][src];
    }
    int size = set->size();
    int mincost = 999999;
    vector<thread> threads;
    for (int i : *set) {
        lock_guard<mutex> lock(edgeMutex);
        highlightEdge(i, src);
        vector<int> subset(set->begin(), set->end());
        subset.erase(remove(subset.begin(), subset.end(), i), subset.end());
        int temp[nCities + 1];
        threads.emplace_back(computeEdgeCost, v, i, ref(subset), temp, &mincost, path);
    }
    for (thread& t : threads) {
        t.join();
    }
    return mincost;
}

void computeEdgeCost(int v, int i, vector<int>& subset, int* temp, int* mincost, int* path) {
    int cost = graph[v][i] + tsp(i, &subset, temp);
    if (cost < *mincost) {
        *mincost = cost;
        temp[subset.size()] = i;
        for (int j = 0; j <= subset.size(); j++) {
            path[j] = temp[j];
        }
    }
    else {
        lock_guard<mutex> lock(edgeMutex);
        dehighlightEdge(i, src);
    }
}

void startTsp() {
    vector<int> set;
    for (int i = 1; i < nCities; i++) {
        set.push_back(i);
    }
    int path[nCities + 1];
    cost = tsp(src, &set, path);
    cout << "Cost: " << cost << endl;
    path[0] = path[nCities] = 0;
    cout << "Path: " << endl;
    for (int i = nCities; i >= 0; i--) {
        cout << path[i] << endl;
    }
    cout << endl;
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void drawFilledCircle(GLfloat x, GLfloat y) {
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle
    GLfloat radius = 0.05f; //radius
    GLfloat twicePi = 2.0f * M_PI;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(
            x + (radius * cos(i *  twicePi / triangleAmount)),
            y + (radius * sin(i * twicePi / triangleAmount))
        );
    }
    glEnd();
}

void drawGraph() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw nodes as circles
    glColor3f(0.5f, 0.5f, 0.5f);
    drawFilledCircle(-0.5f, 0.3f); // Vertex 0
    drawFilledCircle(0.0f, 0.7f);  // Vertex 1
    drawFilledCircle(0.5f, 0.3f);  // Vertex 2
    drawFilledCircle(-0.5f, -0.3f); // Vertex 5
    drawFilledCircle(0.0f, -0.7f);  // Vertex 4
    drawFilledCircle(0.5f, -0.3f);  // Vertex 3

    // Draw node labels
    glColor3f(1.0f, 1.0f, 1.0f); // Set the color to white
    glRasterPos2f(-0.52f, 0.29f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0'); // Vertex 0

    glRasterPos2f(0.0f, 0.7f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '1'); // Vertex 1

    glRasterPos2f(0.5f, 0.3f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '2'); // Vertex 2

    glRasterPos2f(-0.52f, -0.31f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '5'); // Vertex 5

    glRasterPos2f(0.0f, -0.72f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '4'); // Vertex 4

    glRasterPos2f(0.5f, -0.3f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '3'); // Vertex 3

    // Draw edges and label them
    for (int i = 0; i < nCities; i++) {
        for (int j = i + 1; j < nCities; j++) {
            glColor3f(0.5f, 0.5f, 0.5f);
            glBegin(GL_LINES);
            glVertex2fv(arr[i]);
            glVertex2fv(arr[j]);
            glEnd();

            float labelX = (arr[i][0] + arr[j][0]) / 2 + 0.015f;
            float labelY = (arr[i][1] + arr[j][1]) / 2 + 0.015f;

            // Render the label as bitmap characters
            glColor3f(0.0f, 0.0f, 0.0f); // Black color for labels
            glRasterPos2f(labelX, labelY);
            string label = to_string(graph[i][j]); // Use edge weight as label
            for (char c : label) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            }
        }
    }

    glFlush();
}

void display() {
    glLoadIdentity();
    glLineWidth(3.0f);
    drawGraph();

    // Draw the start button
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.9f);
    glVertex2f(0.1f, -0.9f);
    glVertex2f(0.1f, -0.8f);
    glVertex2f(-0.1f, -0.8f);
    glEnd();

    // Draw the button label
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color
    glRasterPos2f(-0.05f, -0.85f);
    if (isStarted) {
        startTsp();
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'S');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'O');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'P');
    } else {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'S');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'R');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
    }
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to OpenGL coordinates
        float xPos = (2.0f * x) / WINDOW_WIDTH - 1.0f;
        float yPos = 1.0f - (2.0f * y) / WINDOW_HEIGHT;

        // Check if the click is within the button area
        if (xPos >= -0.2f && xPos <= 0.2f && yPos >= -0.9f && yPos <= -0.8f) {
            isStarted = !isStarted;
            glutPostRedisplay();
        }
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("TSP Visualization");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
