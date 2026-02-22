#include <GL/glut.h>
#include <cmath>
#include <cstdio>

const float dt = 0.005f;
float dt_coeff = 1.0f;
bool isPause = false;
const int width = 800;
const int height = 600;
const float PI = 3.14159f;
const float radius = 50.0f;
const int segments = 30;
float t = 0.0f;
bool isDay = true;
const float START_X = -radius;
const float END_X = width + radius;
const float BASE_Y = height * 0.3f;
const float ARC_HEIGHT = (height-radius) - BASE_Y;

//дополнительная анимация (у меня дождь)
bool isRaining = false;
const int MAX_RAINDROPS = 300;
struct Raindrop {
    float x, y;
    float speed;
    float length;
};
Raindrop raindrops[MAX_RAINDROPS];
void initRain() {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].x = rand() % width;
        raindrops[i].y = rand() % (int)(height * 0.7f) + height * 0.3f;
        raindrops[i].speed = 5 + rand() % 10;
        raindrops[i].length = 10 + rand() % 15;
    }
}

struct Color {
    float r, g, b;
};
Color daySky = {0.4f, 0.7f, 1.0f};
Color dayGround = {0.0f, 0.8f, 0.2f};
Color dayStem = {0.0f, 1.0f, 0.0f};
Color dayFlower = {1.0f, 0.5f, 1.0f};
Color dayHouse = {0.8f, 0.5f, 0.2f};
Color dayRoof = {0.9f, 0.2f, 0.1f};
Color dayDoor = {0.5f, 0.3f, 0.1f};
Color dayWindow = {0.3f, 0.5f, 0.8f};
Color dayTrunk = {0.5f, 0.3f, 0.1f};
Color dayFoliage = {0.0f, 0.6f, 0.0f};

Color nightSky = {0.05f, 0.05f, 0.1f};
Color nightGround = {0.0f, 0.2f, 0.0f};
Color nightStem = {0.0f, 0.4f, 0.0f};
Color nightFlower = {0.3f, 0.15f, 0.3f};
Color nightHouse = {0.3f, 0.2f, 0.1f};
Color nightRoof = {0.4f, 0.1f, 0.05f};
Color nightDoor = {0.2f, 0.1f, 0.05f};
Color nightWindow = {0.0f, 0.0f, 0.2f};
Color windowLight = {1.0f, 0.9f, 0.5f};
Color nightTrunk = {0.2f, 0.1f, 0.05f}; 
Color nightFoliage = {0.0f, 0.2f, 0.0f};

Color sky, ground, house, roof, door, window, stem, flower, trunk, foliage;

struct Star {
    float x, y;
    float brightness;
    float phase;
};

const int NUM_STARS = 200;
Star stars[NUM_STARS];

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = rand() % width;
        stars[i].y = height * 0.3f + rand() % (int)(height * 0.7f);
        stars[i].brightness = 0.5f + 0.5f * (rand() % 100) / 100.0f;
        stars[i].phase = (rand() % 100) / 100.0f * 2 * PI;
    }
}

Color smoothTransition(Color c1, Color c2, float factor) {
    Color result;
    result.r = c1.r * (1 - factor) + c2.r * factor;
    result.g = c1.g * (1 - factor) + c2.g * factor;
    result.b = c1.b * (1 - factor) + c2.b * factor;
    return result;
}

void drawText(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // шрифт 18pt
    }
}

void drawInfo() {
    char buffer[100];
    sprintf(buffer, "Speed: x%.2f", dt_coeff);
    drawText(20, height - 30, buffer, 0.0f, 1.0f, 0.0f);

    if (isPause)
        drawText(width - 150, height - 50, "PAUSE", 1.0f, 0.0f, 0.0f);
    if (isRaining) {
        drawText(width - 150, height - 80, "RAIN", 0.5f, 0.7f, 1.0f);
    }

    if (isDay)
        drawText(20, height - 60, "Day", 1.0f, 1.0f, 0.0f);
    else
        drawText(20, height - 60, "Night", 0.8f, 0.8f, 1.0f);
}

void updateRain() {
    if (!isRaining) return;
    
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].y -= raindrops[i].speed * dt_coeff;
        if (raindrops[i].y < 0) {
            raindrops[i].x = rand() % width;
            raindrops[i].y = height + rand() % 100;
            raindrops[i].speed = 5 + rand() % 10;
        }
    }
}

void drawRain() {
    if (!isRaining) return;
    
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        if (isDay) {
            glColor3f(0.8f, 0.8f, 1.0f);
        } else {
            glColor3f(0.3f, 0.3f, 0.5f);
        }
        glVertex2f(raindrops[i].x, raindrops[i].y);
        glVertex2f(raindrops[i].x + 2, raindrops[i].y - raindrops[i].length);
    }
    glEnd();
}

void updateColors() {
    if (isDay) {
        //t=0 - утро, t=0.5 - полдень, t=1 - вечер
        float dayAmount;
        if (t <= 0.5f) {
            dayAmount = t * 2.0f; //0->1
        } else {
            dayAmount = 2.0f - t * 2.0f; //1->0
        }
        sky = smoothTransition(nightSky, daySky, dayAmount);
        ground = smoothTransition(nightGround, dayGround, dayAmount);
        house = smoothTransition(nightHouse, dayHouse, dayAmount);
        roof = smoothTransition(nightRoof, dayRoof, dayAmount);
        door = smoothTransition(nightDoor, dayDoor, dayAmount);
        window = smoothTransition(nightWindow, dayWindow, dayAmount);
        stem = smoothTransition(nightStem, dayStem, dayAmount);
        flower = smoothTransition(nightFlower, dayFlower, dayAmount);
        trunk = smoothTransition(nightTrunk, dayTrunk, dayAmount);
        foliage = smoothTransition(nightFoliage, dayFoliage, dayAmount);
    } else {
        sky = nightSky;
        ground = nightGround;
        stem = nightStem;
        flower = nightFlower;
        house = nightHouse;
        roof = nightRoof;
        door = nightDoor;
        window = windowLight;
        trunk = nightTrunk;
        foliage = nightFoliage;
    }
}

void drawCircle(float x, float y, float radius, int segments, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, 
                  float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawTree(float x, float y, float size, Color trunkColor, Color foliageColor) {
    drawRect(x - size*0.1f, y, size*0.2f, size*0.5f, 
             trunkColor.r, trunkColor.g, trunkColor.b);
    
    float foliageY = y + size*0.5f;
    
    drawTriangle(x - size*0.5f, foliageY,
                 x + size*0.5f, foliageY,
                 x, foliageY + size*0.8f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
    
    drawTriangle(x - size*0.4f, foliageY + size*0.4f,
                 x + size*0.4f, foliageY + size*0.4f,
                 x, foliageY + size*1.2f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
    
    drawTriangle(x - size*0.3f, foliageY + size*0.9f,
                 x + size*0.3f, foliageY + size*0.9f,
                 x, foliageY + size*1.5f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
}

void drawSun(float x, float y, float radius, int segments) {
    drawCircle(x, y, radius, segments, 1.0f, 1.0f, 0.0f);
    
    glColor3f(1.0f, 0.7f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    for (int i = 0; i < 12; i++) {
        float angle = 2.0f * PI * i / 12;
        
        float innerX = x + radius * 0.9f * cos(angle);
        float innerY = y + radius * 0.9f * sin(angle);
        
        float outerX = x + radius * 1.5f * cos(angle);
        float outerY = y + radius * 1.5f * sin(angle);
        
        glVertex2f(innerX, innerY);
        glVertex2f(outerX, outerY);
    }
    glEnd();
    drawCircle(x - radius*0.3f, y + radius*0.2f, radius*0.15f, segments, 0, 0, 0);
    drawCircle(x + radius*0.3f, y + radius*0.2f, radius*0.15f, segments, 0, 0, 0);
    drawCircle(x - radius*0.35f, y + radius*0.25f, radius*0.05f, segments, 1, 1, 1);
    drawCircle(x + radius*0.25f, y + radius*0.25f, radius*0.05f, segments, 1, 1, 1);
}

void drawMoon(float x, float y, float radius, int segments) {
    drawCircle(x, y, radius, segments, 0.9f, 0.9f, 0.8f);
    
    drawCircle(x - radius*0.3f, y + radius*0.25f, radius*0.2f, segments, 0.7f, 0.7f, 0.6f);
    drawCircle(x + radius*0.35f, y - radius*0.2f, radius*0.25f, segments, 0.7f, 0.7f, 0.6f);
    drawCircle(x + radius*0.2f, y + radius*0.3f, radius*0.1f, segments, 0.7f, 0.7f, 0.6f);
    
    drawCircle(x - radius*0.35f, y + radius*0.2f, radius*0.1f, segments, 0.5f, 0.5f, 0.5f);
    drawCircle(x + radius*0.3f, y - radius*0.25f, radius*0.1f, segments, 0.5f, 0.5f, 0.5f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(x - radius*0.1f, y + radius*0.2f);
    glEnd();
}

void drawHouse(float x, float y, float w, float h) {
    drawRect(x, y, w, h, house.r, house.g, house.b);
    
    drawTriangle(x - w*0.1f, y + h, 
                 x + w*1.1f, y + h,
                 x + w*0.5f, y + h*1.4f,
                 roof.r, roof.g, roof.b);
    
    float doorW = w * 0.3f;
    float doorH = h * 0.5f;
    drawRect(x + w*0.35f, y, doorW, doorH, door.r, door.g, door.b);
    
    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(x + w*0.5f, y + doorH*0.5f);
    glEnd();
    
    float windowW = w * 0.25f;
    float windowH = h * 0.25f;
    drawRect(x + w*0.1f, y + h*0.6f, windowW, windowH, 
             window.r, window.g, window.b);
    
    glColor3f(0.3f, 0.2f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x + w*0.1f + windowW/2, y + h*0.6f);
    glVertex2f(x + w*0.1f + windowW/2, y + h*0.6f + windowH);
    glVertex2f(x + w*0.1f, y + h*0.6f + windowH/2);
    glVertex2f(x + w*0.1f + windowW, y + h*0.6f + windowH/2);
    glEnd();
}

void drawGrass() {
    glColor3f(ground.r, ground.g, ground.b);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, height * 0.3f);
    glVertex2f(0, height * 0.3f);
    glEnd();
    for (int i = 0; i < 15; i++) {
        float flowerX = 50 + i * 50;
        float flowerY = 30 + 10 * sin(i);
        
        glColor3f(stem.r, stem.g, stem.b);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(flowerX, flowerY);
        glVertex2f(flowerX, flowerY + 30);
        glEnd();
        
        drawCircle(flowerX, flowerY + 35, 8, 10, 
                    flower.r, flower.g, flower.b);
    }
}

void drawStars() {
    if (!isDay) {
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        
        for (int i = 0; i < NUM_STARS; i++) {
            float flicker = 0.7f + 0.3f * sin(t * 5 + stars[i].phase);
            float b = stars[i].brightness * flicker;
            
            glColor3f(b, b, b);
            glVertex2f(stars[i].x, stars[i].y);
        }
        glEnd();
    }
}

void drawForest() {
    float treeX[] = {50, 120, 190, 280, 350, 420, 490, 560, 630, 700, 750};
    float treeSize[] = {40, 55, 45, 60, 50, 65, 45, 55, 50, 60, 40};
    
    for (int i = 0; i < 11; i++) {
        drawTree(treeX[i], 150, treeSize[i], trunk, foliage);
    }
    
    float smallTreeX[] = {130, 270, 410, 550, 680};
    for (int i = 0; i < 5; i++) {
        drawTree(smallTreeX[i], 130, 30, trunk, foliage);
    }
}

void display() {
    updateColors();
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float x = START_X + t * (END_X - START_X);
    float y = BASE_Y + ARC_HEIGHT * sin(t * PI);
    drawStars();
    if (isDay)
        drawSun(x, y, radius, segments);
    else
        drawMoon(x, y, radius, segments);
    drawGrass();
    drawForest();
    drawHouse(500, 150, 120, 150);
    drawHouse(200, 100, 80, 100);
    drawRain();
    drawInfo();
    glFlush();
}

void timer(int value) {
    if (isPause) {
        glutTimerFunc(16, timer, 0);
        return;
    }
    t += (dt * dt_coeff);
    if (t >= 1.0f) {
        t = 0.0f;
        isDay = !isDay;
    }
    updateRain();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            isPause = !isPause;
            break;
        case '+':
            if (dt_coeff < 100.0f)
                dt_coeff += 0.25f;
            break;
        case '-':
            if (dt_coeff > 0.25f)
                dt_coeff -= 0.25f;
            break;
        case 'd':
            isRaining = !isRaining;
            if (isRaining)
                initRain();
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
}

int main(int argc, char** argv) {
    initStars();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Sun");

    init();
    
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}