// gcc snowman_sample.c -lglut -lGL -lGLU -lm -o snowman && ./snowman

#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include <cmath>

// Rotation
static GLfloat yRot = 0.0f;

// Change viewing volume and viewport.  Called when window is resized  
void ChangeSize(int w, int h)
{
    GLfloat fAspect;

    // Prevent a divide by zero  
    if (h == 0)
        h = 1;

    // Set Viewport to window dimensions  
    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset coordinate system  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Produce the perspective projection  
    gluPerspective(35.0f, fAspect, 1.0, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// This function does any needed initialization on the rendering context.  Here it sets up and initializes the lighting for the scene.
void SetupRC() {

    // Light values and coordinates  
    GLfloat  whiteLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat  lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };

    glEnable(GL_DEPTH_TEST);    // Hidden surface removal  
    glFrontFace(GL_CCW);        // Counter clock-wise polygons face out  
    glEnable(GL_CULL_FACE);     // Do not calculate inside  

    // Enable lighting  
    glEnable(GL_LIGHTING);

    // Setup and enable light 0  
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    // Enable color tracking  
    glEnable(GL_COLOR_MATERIAL);

    // Set Material properties to follow glColor values  
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Black blue background  
    glClearColor(0.25f, 0.25f, 0.50f, 1.0f);

}

// Respond to arrow keys (rotate snowman)
void SpecialKeys(int key, int x, int y) {

    if (key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if (key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    yRot = (GLfloat)((const int)yRot % 360);

    // Refresh the Window  
    glutPostRedisplay();
}

// Called to draw scene  
void RenderScene(void) {
    GLUquadricObj* pObj;    // Quadric Object  

    // Clear the window with current clearing color  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Save the matrix state and do the rotations  
    glPushMatrix();

        // Move object back and do in place rotation  
        glTranslatef(0.0f, -1.0f, -5.0f);
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);

        // Draw something  
        pObj = gluNewQuadric();
        gluQuadricNormals(pObj, GLU_SMOOTH);

        // white
        glColor3f(1.0f, 1.0f, 1.0f);

        // Head
        glPushMatrix(); // save transform matrix state
            glTranslatef(0.0f, 1.5f, 0.0f);
            gluSphere(pObj, 0.24f, 26, 13);

            glColor3f(0.0f, 0.0f, 0.0f);
            // Rigth eye
            glPushMatrix();
                glTranslatef(-0.07f, 0.1f, 0.2f);
                gluSphere(pObj, 0.02f, 26, 13);
            glPopMatrix();
            // Left eye
            glPushMatrix();
                glTranslatef(0.07f, 0.1f, 0.2f);
                gluSphere(pObj, 0.02f, 26, 13);
            glPopMatrix();

            // Nose (orange)
            glColor3f(1.0f, 0.4f, 0.2f);
            glPushMatrix();
                glTranslatef(0.0f, 0.0f, 0.18f);
                gluCylinder(pObj, 0.04f, 0.0f, 0.3f, 26, 13);
            glPopMatrix();

            // Mouth (curve)
            glLineWidth(2.5f);
            glColor3f(0.0f, 0.0f, 0.0f);
            glPushMatrix();
            glBegin(GL_LINE_STRIP); 
                for (float angle = 230.0f; angle <= 310.0f; angle += 10.0f) {
                    float x = 0.0f + 0.2f * cos(angle * M_PI / 180.0f);
                    float y = 0.06f + 0.2f * sin(angle * M_PI / 180.0f);
                    glVertex3f(x, y, 0.195f); // Adiciona o ponto à curva
                }
            glEnd();
            glPopMatrix();

            // Scarf
            glColor3f(0.0f, 0.0f, 1.0f);
            glPushMatrix();
                glPushMatrix();
                    glTranslatef(0.0f, -0.16f, 0.0f);
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.2f, 0.2f, 0.07f, 26, 13);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.1f, -0.285f, 0.235f);
                    glRotatef(100.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
                    glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
                    glScalef(0.23f, 0.01f, 0.07f);
                    glutSolidCube(1.0f);
                glPopMatrix();
            glPopMatrix();

        glPopMatrix(); // restore transform matrix state

        glColor3f(1.0f, 1.0f, 1.0f);
        // Body
        glPushMatrix(); // save transform matrix state
            glTranslatef(0.0f, 1.05f, 0.0f);
            gluSphere(pObj, 0.30f, 26, 13);

            // Bottons
            glColor3f(0.0f, 0.0f, 0.0f);
            // First
            glPushMatrix();
                glTranslatef(0.0f, 0.13f, 0.27f);
                gluSphere(pObj, 0.03f, 26, 13);
            glPopMatrix();
            // Second
            glPushMatrix();
                glTranslatef(0.0f, 0.03f, 0.3f);
                gluSphere(pObj, 0.03f, 26, 13);
            glPopMatrix();
            // Third
            glPushMatrix();
                glTranslatef(0.0f, -0.07f, 0.295f);
                gluSphere(pObj, 0.03f, 26, 13);
            glPopMatrix();


            // Arms
            gluQuadricOrientation(pObj, GLU_INSIDE);
            // Rigtht arm
            glColor3f(0.5f, 0.2f, 0.0f);  // Cor marrom escura
            glPushMatrix();
                glPushMatrix();
                    glTranslatef(-0.2f, 0.02f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.55f, 26, 13);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(-0.6f, 0.165f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(155.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.1f, 26, 13);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(-0.6f, 0.165f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(245.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.1f, 26, 13);
                glPopMatrix();
            glPopMatrix();
            // Left arm
            glPushMatrix();
                glPushMatrix();
                    glTranslatef(0.2f, 0.02f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.55f, 26, 13);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.6f, 0.165f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(-65.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.1f, 26, 13);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.6f, 0.165f, 0.0f);
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
                    gluCylinder(pObj, 0.01f, 0.01f, 0.1f, 26, 13);
                glPopMatrix();
            glPopMatrix();
            gluQuadricOrientation(pObj, GLU_OUTSIDE);
                
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 1.0f);
        // Bottom
        glPushMatrix(); // save transform matrix state
            glTranslatef(0.0f, 0.55f, 0.0f);
            gluSphere(pObj, 0.40f, 26, 13);
        glPopMatrix();

    // Restore the matrix state  
    glPopMatrix();

    // Buffer swap  
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Modeling with Quadrics");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    SetupRC();
    glutMainLoop();

    return 0;
}