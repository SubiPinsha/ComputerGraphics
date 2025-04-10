#include <GL/glut.h>
#include <cmath>
#include <vector>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define a simple 2D point structure
struct Point {
    float x, y;
};

// Global variables for our play area (clipping rectangle)
float winXmin = 150, winXmax = 350, winYmin = 150, winYmax = 350;

// ---------------------------------------------------------------------
// Sutherland–Hodgman Polygon Clipping Functions (clip against 4 edges)
// ---------------------------------------------------------------------

// Clip against left boundary: x >= xMin
std::vector<Point> clipLeft(const std::vector<Point>& poly, float xMin) {
    std::vector<Point> result;
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        Point curr = poly[i];
        Point prev = poly[(i + n - 1) % n];
        bool currInside = (curr.x >= xMin);
        bool prevInside = (prev.x >= xMin);
        if (prevInside && currInside) {
            result.push_back(curr);
        }
        else if (prevInside && !currInside) {
            float t = (xMin - prev.x) / (curr.x - prev.x);
            Point inter = { xMin, prev.y + t * (curr.y - prev.y) };
            result.push_back(inter);
        }
        else if (!prevInside && currInside) {
            float t = (xMin - prev.x) / (curr.x - prev.x);
            Point inter = { xMin, prev.y + t * (curr.y - prev.y) };
            result.push_back(inter);
            result.push_back(curr);
        }
    }
    return result;
}

// Clip against right boundary: x <= xMax
std::vector<Point> clipRight(const std::vector<Point>& poly, float xMax) {
    std::vector<Point> result;
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        Point curr = poly[i];
        Point prev = poly[(i + n - 1) % n];
        bool currInside = (curr.x <= xMax);
        bool prevInside = (prev.x <= xMax);
        if (prevInside && currInside) {
            result.push_back(curr);
        }
        else if (prevInside && !currInside) {
            float t = (xMax - prev.x) / (curr.x - prev.x);
            Point inter = { xMax, prev.y + t * (curr.y - prev.y) };
            result.push_back(inter);
        }
        else if (!prevInside && currInside) {
            float t = (xMax - prev.x) / (curr.x - prev.x);
            Point inter = { xMax, prev.y + t * (curr.y - prev.y) };
            result.push_back(inter);
            result.push_back(curr);
        }
    }
    return result;
}

// Clip against bottom boundary: y >= yMin
std::vector<Point> clipBottom(const std::vector<Point>& poly, float yMin) {
    std::vector<Point> result;
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        Point curr = poly[i];
        Point prev = poly[(i + n - 1) % n];
        bool currInside = (curr.y >= yMin);
        bool prevInside = (prev.y >= yMin);
        if (prevInside && currInside) {
            result.push_back(curr);
        }
        else if (prevInside && !currInside) {
            float t = (yMin - prev.y) / (curr.y - prev.y);
            Point inter = { prev.x + t * (curr.x - prev.x), yMin };
            result.push_back(inter);
        }
        else if (!prevInside && currInside) {
            float t = (yMin - prev.y) / (curr.y - prev.y);
            Point inter = { prev.x + t * (curr.x - prev.x), yMin };
            result.push_back(inter);
            result.push_back(curr);
        }
    }
    return result;
}

// Clip against top boundary: y <= yMax
std::vector<Point> clipTop(const std::vector<Point>& poly, float yMax) {
    std::vector<Point> result;
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        Point curr = poly[i];
        Point prev = poly[(i + n - 1) % n];
        bool currInside = (curr.y <= yMax);
        bool prevInside = (prev.y <= yMax);
        if (prevInside && currInside) {
            result.push_back(curr);
        }
        else if (prevInside && !currInside) {
            float t = (yMax - prev.y) / (curr.y - prev.y);
            Point inter = { prev.x + t * (curr.x - prev.x), yMax };
            result.push_back(inter);
        }
        else if (!prevInside && currInside) {
            float t = (yMax - prev.y) / (curr.y - prev.y);
            Point inter = { prev.x + t * (curr.x - prev.x), yMax };
            result.push_back(inter);
            result.push_back(curr);
        }
    }
    return result;
}

// Sutherland–Hodgman clipping: clips polygon 'poly' against a rectangular region.
std::vector<Point> sutherlandHodgmanClip(const std::vector<Point>& poly,
    float xMin, float xMax, float yMin, float yMax) {
    std::vector<Point> output = clipLeft(poly, xMin);
    output = clipRight(output, xMax);
    output = clipBottom(output, yMin);
    output = clipTop(output, yMax);
    return output;
}

// --------------------------
// Drawing Functions for Scene
// --------------------------

// Draw background: sky (upper) and grass (lower)
void drawBackground() {
    // Sky
    glColor3f(0.53f, 0.81f, 0.98f); // light blue
    glBegin(GL_POLYGON);
    glVertex2f(0, 250);
    glVertex2f(500, 250);
    glVertex2f(500, 500);
    glVertex2f(0, 500);
    glEnd();
    // Grass
    glColor3f(0.0f, 0.8f, 0.0f); // green
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(500, 0);
    glVertex2f(500, 250);
    glVertex2f(0, 250);
    glEnd();
}

// Draw the play area as a light-gray rectangle (this is our clipping window)
void drawPlayArea() {
    glColor3f(0.9f, 0.9f, 0.9f); // light gray fill
    glBegin(GL_POLYGON);
    glVertex2f(winXmin, winYmin);
    glVertex2f(winXmax, winYmin);
    glVertex2f(winXmax, winYmax);
    glVertex2f(winXmin, winYmax);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f); // black border
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(winXmin, winYmin);
    glVertex2f(winXmax, winYmin);
    glVertex2f(winXmax, winYmax);
    glVertex2f(winXmin, winYmax);
    glEnd();
}

// Draw a slide at the lower left
void drawSlide() {
    // Platform
    glColor3f(0.6f, 0.3f, 0.0f); // brown
    glBegin(GL_POLYGON);
    glVertex2f(50, 100);
    glVertex2f(100, 100);
    glVertex2f(100, 130);
    glVertex2f(50, 130);
    glEnd();
    // Slanted slide surface
    glColor3f(0.8f, 0.8f, 0.8f); // light gray
    glBegin(GL_POLYGON);
    glVertex2f(100, 130);
    glVertex2f(150, 80);
    glVertex2f(140, 70);
    glVertex2f(90, 120);
    glEnd();
}

// Draw a swing set with two posts, a top bar, ropes, and a seat
void drawSwing() {
    // Left post
    glColor3f(0.5f, 0.25f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(200, 200);
    glVertex2f(210, 200);
    glVertex2f(210, 300);
    glVertex2f(200, 300);
    glEnd();
    // Right post
    glBegin(GL_POLYGON);
    glVertex2f(240, 200);
    glVertex2f(250, 200);
    glVertex2f(250, 300);
    glVertex2f(240, 300);
    glEnd();
    // Top bar
    glBegin(GL_POLYGON);
    glVertex2f(200, 300);
    glVertex2f(250, 300);
    glVertex2f(250, 310);
    glVertex2f(200, 310);
    glEnd();
    // Ropes
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(210, 300);
    glVertex2f(210, 270);
    glVertex2f(240, 300);
    glVertex2f(240, 270);
    glEnd();
    // Seat (red)
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(205, 260);
    glVertex2f(245, 260);
    glVertex2f(245, 265);
    glVertex2f(205, 265);
    glEnd();
}

// Draw a merry-go-round: a circle with spokes
void drawMerryGoRound() {
    glColor3f(0.0f, 0.5f, 0.5f);
    const int segments = 40;
    float cx = 400, cy = 400, r = 40;
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < segments; i += 8) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx, cy);
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

// Draw a tree: trunk and circular foliage
void drawTree() {
    // Trunk
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_POLYGON);
    glVertex2f(80, 180);
    glVertex2f(90, 180);
    glVertex2f(90, 250);
    glVertex2f(80, 250);
    glEnd();
    // Foliage
    glColor3f(0.0f, 0.8f, 0.0f);
    const int segments = 30;
    float cx = 85, cy = 270, r = 30;
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

// Draw a bench
void drawBench() {
    // Seat
    glColor3f(0.6f, 0.3f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(300, 50);
    glVertex2f(400, 50);
    glVertex2f(400, 70);
    glVertex2f(300, 70);
    glEnd();
    // Backrest
    glBegin(GL_POLYGON);
    glVertex2f(300, 70);
    glVertex2f(400, 70);
    glVertex2f(400, 90);
    glVertex2f(300, 90);
    glEnd();
}

// Draw a soccer ball (approximated as a circle) and clip the part inside the play area
void drawBall() {
    std::vector<Point> ballPoly;
    const int segments = 50;
    float cx = 300, cy = 300, r = 60;
    // Create polygon for ball
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        float x = cx + r * cos(theta);
        float y = cy + r * sin(theta);
        ballPoly.push_back({ x, y });
    }
    // Draw full ball (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (auto p : ballPoly)
        glVertex2f(p.x, p.y);
    glEnd();
    // Clip the ball polygon against the play area
    std::vector<Point> insidePoly = sutherlandHodgmanClip(ballPoly, winXmin, winXmax, winYmin, winYmax);
    // "Erase" the inside portion by filling it with the play area color (light gray)
    glColor3f(0.9f, 0.9f, 0.9f);
    if (!insidePoly.empty()) {
        glBegin(GL_POLYGON);
        for (auto p : insidePoly)
            glVertex2f(p.x, p.y);
        glEnd();
    }
}

// --------------------------
// Display Callback
// --------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background (sky and grass)
    drawBackground();
    // Draw the designated play area (clipping window)
    drawPlayArea();

    // Draw playground objects
    drawSlide();
    drawSwing();
    drawMerryGoRound();
    drawTree();
    drawBench();
    // Draw the soccer ball with clipping applied so only the portion outside the play area shows
    drawBall();

    glFlush();
}

// --------------------------
// Main Function
// --------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    // Single buffering and RGB mode
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Realistic Playground with Polygon Clipping");

    // Set background clear color (won't be seen because we draw our background)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Set up a 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 500, 0, 500);

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
