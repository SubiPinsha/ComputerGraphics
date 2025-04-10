#include <GL/glut.h>
#include <cmath>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------
// Global definitions & Globals
// -----------------------------
// Board (clipping region) boundaries
const float boardXmin = 100, boardXmax = 400;
const float boardYmin = 300, boardYmax = 450;

// Cohen-Sutherland region codes
const int INSIDE = 0; // 0000
const int LEFT = 1; // 0001
const int RIGHT = 2; // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8; // 1000

// Compute the region code for a point (x, y)
int computeOutCode(float x, float y, float xmin, float xmax, float ymin, float ymax) {
    int code = INSIDE;
    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin) code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    return code;
}

// Cohen–Sutherland line clipping algorithm.
// Modifies (x0,y0)-(x1,y1) if the line is partially inside the clip rectangle.
bool cohenSutherlandClip(float& x0, float& y0, float& x1, float& y1,
    float xmin, float xmax, float ymin, float ymax) {
    int code0 = computeOutCode(x0, y0, xmin, xmax, ymin, ymax);
    int code1 = computeOutCode(x1, y1, xmin, xmax, ymin, ymax);
    bool accept = false;

    while (true) {
        if ((code0 | code1) == 0) {
            // Both endpoints inside: accept the line.
            accept = true;
            break;
        }
        else if (code0 & code1) {
            // Both endpoints share an outside zone (trivial reject).
            break;
        }
        else {
            // At least one endpoint is outside the clip rectangle.
            int outCode = code0 ? code0 : code1;
            float x, y;
            if (outCode & TOP) {
                // Point is above the clip rectangle.
                x = x0 + (x1 - x0) * (boardYmax - y0) / (y1 - y0);
                y = boardYmax;
            }
            else if (outCode & BOTTOM) {
                // Point is below the clip rectangle.
                x = x0 + (x1 - x0) * (boardYmin - y0) / (y1 - y0);
                y = boardYmin;
            }
            else if (outCode & RIGHT) {
                // Point is to the right of clip rectangle.
                y = y0 + (y1 - y0) * (boardXmax - x0) / (x1 - x0);
                x = boardXmax;
            }
            else if (outCode & LEFT) {
                // Point is to the left of clip rectangle.
                y = y0 + (y1 - y0) * (boardXmin - x0) / (x1 - x0);
                x = boardXmin;
            }
            // Now update the point that was outside the rectangle.
            if (outCode == code0) {
                x0 = x; y0 = y;
                code0 = computeOutCode(x0, y0, xmin, xmax, ymin, ymax);
            }
            else {
                x1 = x; y1 = y;
                code1 = computeOutCode(x1, y1, xmin, xmax, ymin, ymax);
            }
        }
    }
    return accept;
}

// ---------------------
// Object Drawing Functions
// ---------------------

// Draw the black board (which serves as the clipping region for the clock)
void drawBlackBoard() {
    glColor3f(0.0f, 0.5f, 0.0f);  // dark green board
    glBegin(GL_POLYGON);
    glVertex2f(boardXmin, boardYmin);
    glVertex2f(boardXmax, boardYmin);
    glVertex2f(boardXmax, boardYmax);
    glVertex2f(boardXmin, boardYmax);
    glEnd();

    // Draw board frame
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(boardXmin, boardYmin);
    glVertex2f(boardXmax, boardYmin);
    glVertex2f(boardXmax, boardYmax);
    glVertex2f(boardXmin, boardYmax);
    glEnd();
}

// Draw the clock with its circular boundary (approximated by line segments)
// and clip each segment so that only portions inside the board are drawn.
void drawClippedClock() {
    // Clock parameters: placed so that it is partially outside the board.
    float cx = 80, cy = 400; // center of clock
    float r = 50;           // radius of clock
    const int segments = 50;
    glColor3f(1.0f, 0.0f, 0.0f);  // red clock outline

    glBegin(GL_LINES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2.0f * M_PI * i / segments;
        float theta2 = 2.0f * M_PI * (i + 1) / segments;
        float x0 = cx + r * cos(theta1);
        float y0 = cy + r * sin(theta1);
        float x1 = cx + r * cos(theta2);
        float y1 = cy + r * sin(theta2);

        // Copy endpoints for clipping
        float clipX0 = x0, clipY0 = y0;
        float clipX1 = x1, clipY1 = y1;

        if (cohenSutherlandClip(clipX0, clipY0, clipX1, clipY1,
            boardXmin, boardXmax, boardYmin, boardYmax))
        {
            glVertex2f(clipX0, clipY0);
            glVertex2f(clipX1, clipY1);
        }
    }
    glEnd();
}

// Draw a fan on the ceiling.
void drawFan() {
    // Fan body (circle) at (450,480) with radius 20.
    float cx = 450, cy = 480, r = 20;
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_POLYGON);
    int segments = 20;
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();

    // Fan blades (simple lines)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx, cy + r + 10);

    glVertex2f(cx, cy);
    glVertex2f(cx + r + 10, cy);

    glVertex2f(cx, cy);
    glVertex2f(cx, cy - r - 10);

    glVertex2f(cx, cy);
    glVertex2f(cx - r - 10, cy);
    glEnd();
}

// Draw a window on the left wall.
void drawWindow() {
    glColor3f(0.5f, 0.8f, 1.0f);  // light blue glass
    glBegin(GL_POLYGON);
    glVertex2f(10, 350);
    glVertex2f(60, 350);
    glVertex2f(60, 400);
    glVertex2f(10, 400);
    glEnd();

    // Window frame
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10, 350);
    glVertex2f(60, 350);
    glVertex2f(60, 400);
    glVertex2f(10, 400);
    glEnd();
}

// Draw benches for the students.
void drawBenches() {
    glColor3f(0.6f, 0.3f, 0.0f);  // brown bench
    // First bench
    glBegin(GL_POLYGON);
    glVertex2f(120, 50);
    glVertex2f(250, 50);
    glVertex2f(250, 90);
    glVertex2f(120, 90);
    glEnd();
    // Second bench
    glBegin(GL_POLYGON);
    glVertex2f(260, 50);
    glVertex2f(390, 50);
    glVertex2f(390, 90);
    glVertex2f(260, 90);
    glEnd();
}

// Draw the teacher’s table.
void drawTeacherTable() {
    glColor3f(0.8f, 0.5f, 0.2f);  // table color
    glBegin(GL_POLYGON);
    glVertex2f(200, 120);
    glVertex2f(300, 120);
    glVertex2f(300, 170);
    glVertex2f(200, 170);
    glEnd();

    // Table outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(200, 120);
    glVertex2f(300, 120);
    glVertex2f(300, 170);
    glVertex2f(200, 170);
    glEnd();
}

// Draw a stickman representing the teacher.
void drawTeacherStickman() {
    // Head: circle centered at (250,190) with radius 10.
    float cx = 250, cy = 190, r = 10;
    glColor3f(0.0f, 0.0f, 0.0f);
    int segments = 20;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
    // Body
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r);
    glVertex2f(cx, cy - r - 20);
    glEnd();
    // Arms
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r - 5);
    glVertex2f(cx - 10, cy - r - 15);
    glVertex2f(cx, cy - r - 5);
    glVertex2f(cx + 10, cy - r - 15);
    glEnd();
    // Legs
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r - 20);
    glVertex2f(cx - 10, cy - r - 30);
    glVertex2f(cx, cy - r - 20);
    glVertex2f(cx + 10, cy - r - 30);
    glEnd();
}

// Draw a stickman representing a student.
void drawStudentStickman() {
    // Head: circle centered at (180,100) with radius 8.
    float cx = 180, cy = 100, r = 8;
    glColor3f(0.0f, 0.0f, 0.0f);
    int segments = 20;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
    // Body
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r);
    glVertex2f(cx, cy - r - 15);
    glEnd();
    // Arms
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r - 3);
    glVertex2f(cx - 8, cy - r - 8);
    glVertex2f(cx, cy - r - 3);
    glVertex2f(cx + 8, cy - r - 8);
    glEnd();
    // Legs
    glBegin(GL_LINES);
    glVertex2f(cx, cy - r - 15);
    glVertex2f(cx - 8, cy - r - 25);
    glVertex2f(cx, cy - r - 15);
    glVertex2f(cx + 8, cy - r - 25);
    glEnd();
}

// Draw a small plant in a pot.
void drawPlant() {
    // Pot
    glColor3f(0.8f, 0.4f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(420, 150);
    glVertex2f(440, 150);
    glVertex2f(440, 170);
    glVertex2f(420, 170);
    glEnd();
    // Plant (a small green circle)
    float cx = 430, cy = 180, r = 10;
    glColor3f(0.0f, 0.8f, 0.0f);
    int segments = 20;
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

// Draw a door on the right side of the classroom.
void drawDoor() {
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(450, 50);
    glVertex2f(490, 50);
    glVertex2f(490, 150);
    glVertex2f(450, 150);
    glEnd();

    // Door frame
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(450, 50);
    glVertex2f(490, 50);
    glVertex2f(490, 150);
    glVertex2f(450, 150);
    glEnd();
}

// ---------------------
// Display and Main Loop
// ---------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw classroom objects:
    drawBlackBoard();      // (clipping region for clock)
    drawClippedClock();    // clock (only the part inside the board is drawn)
    drawFan();
    drawWindow();
    drawBenches();
    drawTeacherTable();
    drawTeacherStickman();
    drawStudentStickman();
    drawPlant();
    drawDoor();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    // Single buffering and RGB color mode
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Classroom Scene with Clipped Clock");

    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 500, 0, 500);

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
