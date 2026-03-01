#include <GL/glut.h>
#include <cmath>
#include <cstdio>

const int WIDTH = 800;
const int HEIGHT = 600;
const float PI = 3.14159f;

float rotAngleY = 0.0f;
float rotAngleX = 0.0f;
float cameraDistance = 5.0f;
float lightAngle = 0.0f;
float expandFactor = 0.0f;
bool transparencyEnabled = false;
float transparencyLevel = 0.5f;

struct Face {
    float tx, ty, tz;
    float rx, ry, rz;
    float color[3];
};

Face faces[6] = {
    //передняя грань
    {0, 0, 0.5f, 0, 0, 0, {0.8f, 0.6f, 0.4f}},
    //задняя грань
    {0, 0, -0.5f, 0, 180, 0, {0.7f, 0.5f, 0.3f}},
    //правая грань
    {0.5f, 0, 0, 0, 90, 0, {0.6f, 0.4f, 0.2f}},
    //левая грань
    {-0.5f, 0, 0, 0, -90, 0, {0.6f, 0.4f, 0.2f}},
    //верхняя грань
    {0, 0.5f, 0, -90, 0, 0, {0.9f, 0.7f, 0.5f}},
    //нижняя грань
    {0, -0.5f, 0, 90, 0, 0, {0.5f, 0.3f, 0.1f}}
};

float lightPos[4] = {2.0f, 2.0f, 2.0f, 1.0f};

//Материалы для граней
GLfloat matAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat matDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat matShininess[] = {50.0f};

//Свет
GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

void drawFace(float size) {
    float s = size / 2.0f;
    
    glBegin(GL_QUADS);
    //Нормаль для освещения
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-s, -s, 0);
    glVertex3f( s, -s, 0);
    glVertex3f( s,  s, 0);
    glVertex3f(-s,  s, 0);
    glEnd();
}

void drawExpandedCube() {
    float baseSize = 1.0f;
    float gap = expandFactor * 0.3f;

    if (transparencyEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glDepthMask(GL_FALSE);
    }
    
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        
        float multiplier = 1.0f + gap;
        
        glTranslatef(faces[i].tx * multiplier, 
                     faces[i].ty * multiplier, 
                     faces[i].tz * multiplier);
        
        glRotatef(faces[i].rx, 1, 0, 0);
        glRotatef(faces[i].ry, 0, 1, 0);
        glRotatef(faces[i].rz, 0, 0, 1);
        
        //Материал грани
        GLfloat faceColor[] = {faces[i].color[0], faces[i].color[1], 
                               faces[i].color[2], transparencyEnabled ? transparencyLevel : 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, faceColor);
        
        drawFace(baseSize);
        
        glPopMatrix();
    }

    if (transparencyEnabled) {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    
    if (expandFactor > 0.01f) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiffuse);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < 6; i++) {
            float dirX = faces[i].tx * 2;
            float dirY = faces[i].ty * 2;
            float dirZ = faces[i].tz * 2;
            
            glVertex3f(0, 0, 0);
            glVertex3f(dirX * (1.0f + gap), dirY * (1.0f + gap), 
                       dirZ * (1.0f + gap));
        }
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    //Камера
    float camX = cameraDistance * cos(rotAngleX) * sin(rotAngleY);
    float camY = cameraDistance * sin(rotAngleX);
    float camZ = cameraDistance * cos(rotAngleX) * cos(rotAngleY);
    gluLookAt(camX, camY, camZ,
              0, 0, 0,
              0, 1, 0);
    
    // Позиция источника света
    lightPos[0] = 3.0f * sin(lightAngle);
    lightPos[1] = 2.0f;
    lightPos[2] = 3.0f * cos(lightAngle);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Рисуем источник света (маленькая сфера)
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
    glutSolidSphere(0.2f, 20, 20);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    drawExpandedCube();
    
    glutSwapBuffers();
}

void timer(int /*value*/) {
    lightAngle += 0.02f;
    if (lightAngle > 2 * PI) lightAngle -= 2 * PI;
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case '+':
        case '=':
            expandFactor += 0.05f;
            if (expandFactor > 1.0f) expandFactor = 1.0f;
            break;
        case '-':
        case '_':
            expandFactor -= 0.05f;
            if (expandFactor < 0.0f) expandFactor = 0.0f;
            break;
        case 't':
        case 'T':
            transparencyEnabled = !transparencyEnabled;
            break;
        case '[':
        case '{':
            transparencyLevel -= 0.1f;
            if (transparencyLevel < 0.0f) transparencyLevel = 0.0f;
            break;
        case ']':
        case '}':
            transparencyLevel += 0.1f;
            if (transparencyLevel > 1.0f) transparencyLevel = 1.0f;
            break;
        case 'r':
        case 'R':
            expandFactor = 0.0f;
            lightAngle = 0.0f;
            rotAngleX = 0.0f;
            rotAngleY = 0.0f;
            cameraDistance = 5.0f;
            break;
        case 'z':
        case 'Z':
            cameraDistance -= 0.5f;
            if (cameraDistance < 2.0f) cameraDistance = 2.0f;
            break;
        case 'x':
        case 'X':
            cameraDistance += 0.5f;
            if (cameraDistance > 10.0f) cameraDistance = 10.0f;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int /*x*/, int /*y*/) {
    switch (key) {
        case GLUT_KEY_LEFT:
            rotAngleY += 0.1f;
            break;
        case GLUT_KEY_RIGHT:
            rotAngleY -= 0.1f;
            break;
        case GLUT_KEY_UP:
            rotAngleX += 0.1f;
            if (rotAngleX > PI/2 - 0.1f) rotAngleX = PI/2 - 0.1f;
            break;
        case GLUT_KEY_DOWN:
            rotAngleX -= 0.1f;
            if (rotAngleX < -PI/2 + 0.1f) rotAngleX = -PI/2 + 0.1f;
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glDisable(GL_BLEND);
    
    //Настройка материалов
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    
    //Настройка источника света
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    //Настройка проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WIDTH / HEIGHT, 1.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("lab3");
    
    init();
    
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    return 0;
}