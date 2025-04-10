# OpenGL Graphics Assignment

This project demonstrates various fundamental computer graphics algorithms using OpenGL in C/C++. It includes implementation of polygon clipping, line clipping, text clipping, DDA and Bresenham’s line drawing algorithms, midpoint circle drawing, and scene creation.

##  Requirements
- OpenGL (GL/glut.h)
- C/C++ Compiler (e.g., g++)
- Code::Blocks or any IDE with OpenGL support

---
---

##  Tasks Implemented

### 1. Playground Scene + Polygon Clipping
- Created a playground with at least 5 objects.
- One object is clipped using the **Polygon Clipping Algorithm** to show only the part **outside the clipping window**.

### 2. Classroom Scene + Line Clipping
- Designed a classroom with 10+ objects.
- An object near the blackboard was clipped using **Cohen-Sutherland / Liang-Barsky Algorithm** to display only the part **inside the blackboard**.

### 3. Text Clipping
- Text drawn on the blackboard is clipped using **Text Clipping** techniques to restrict visibility within the board's boundaries.

### 4. Drawing Initials (First 3 Letters of Name)
- Used both **DDA and Bresenham’s Line Drawing Algorithms** to render the first 3 letters of the name with pixel-level accuracy.

### 5. Olympics Logo
- Used **Midpoint Circle Drawing Algorithm** to draw the five rings of the Olympics logo.
- Each ring is filled using **different pattern attributes**.

### 6. Combined Classroom Scene
- Created a final integrated classroom scene using all learned algorithms.
  - DDA & Bresenham for drawing.
  - Midpoint for circular objects.
  - Polygon & Line Clipping for spatial constraints.
  - Text Clipping for content boundaries.

---

## 🚀 How to Run
1. Compile using a g++ compatible compiler with OpenGL:
   ```bash
   g++ main.cpp -lGL -lGLU -lglut -o graphics_assignment
   ./graphics_assignment
