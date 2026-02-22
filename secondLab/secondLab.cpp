#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <vector>

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

class VertexArrayScene {
private:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLuint> lineIndices;
    std::vector<GLuint> pointIndices;
    std::vector<GLuint> triangleIndices;
    
public:
    VertexArrayScene() {
        clear();
    }
    
    void clear() {
        vertices.clear();
        colors.clear();
        lineIndices.clear();
        pointIndices.clear();
        triangleIndices.clear();
    }
    
    void vertex(float x, float y) {
        vertices.push_back(x);
        vertices.push_back(y);
    }
    
    void color(float r, float g, float b) {
        colors.push_back(r);
        colors.push_back(g);
        colors.push_back(b);
    }
    
    void addPoint(float x, float y, float r, float g, float b, bool draw = true) {
        int currentIdx = vertices.size() / 2;
        vertex(x, y);
        color(r, g, b);
        if (draw)
            pointIndices.push_back(currentIdx);
    }
    
    void addLine(float x1, float y1, float x2, float y2, 
                 float r, float g, float b) {
        int startIdx = vertices.size() / 2;
        
        addPoint(x1, y1, r, g, b, false);
        addPoint(x2, y2, r, g, b, false);
        
        lineIndices.push_back(startIdx);
        lineIndices.push_back(startIdx + 1);
    }

    void addTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
                 float r, float g, float b, bool filled = true) {
        int startIdx = vertices.size() / 2;
        
        addPoint(x1, y1, r, g, b, false);
        addPoint(x2, y2, r, g, b, false);
        addPoint(x3, y3, r, g, b, false);
        
        if (filled) {
            triangleIndices.push_back(startIdx);
            triangleIndices.push_back(startIdx + 1);
            triangleIndices.push_back(startIdx + 2);
        } else {
            lineIndices.push_back(startIdx);
            lineIndices.push_back(startIdx + 1);
            lineIndices.push_back(startIdx + 1);
            lineIndices.push_back(startIdx + 2);
            lineIndices.push_back(startIdx + 2);
            lineIndices.push_back(startIdx);
        }
    }
    
    void addRect(float x, float y, float w, float h, 
             float r, float g, float b, bool filled = true) {
        int startIdx = vertices.size() / 2;
        
        addPoint(x, y, r, g, b, false);
        addPoint(x + w, y, r, g, b, false);
        addPoint(x + w, y + h, r, g, b, false);
        addPoint(x, y + h, r, g, b, false);
        
        if (filled) {
            triangleIndices.push_back(startIdx);
            triangleIndices.push_back(startIdx + 1);
            triangleIndices.push_back(startIdx + 2);
            
            triangleIndices.push_back(startIdx);
            triangleIndices.push_back(startIdx + 2);
            triangleIndices.push_back(startIdx + 3);
        } else {
            lineIndices.push_back(startIdx);
            lineIndices.push_back(startIdx + 1);
            lineIndices.push_back(startIdx + 1);
            lineIndices.push_back(startIdx + 2);
            lineIndices.push_back(startIdx + 2);
            lineIndices.push_back(startIdx + 3);
            lineIndices.push_back(startIdx + 3);
            lineIndices.push_back(startIdx);
        }
    }
    
    void addCircle(float cx, float cy, float rad, 
               float r, float g, float b, int seg = segments, bool filled = true) {
        int startIdx = vertices.size() / 2;
        
        if (filled) {
            addPoint(cx, cy, r, g, b, false);
            
            for (int i = 0; i <= seg; i++) {
                float angle = 2 * PI * i / seg;
                float x = cx + rad * cos(angle);
                float y = cy + rad * sin(angle);
                addPoint(x, y, r, g, b, false);
            }
            
            for (int i = 0; i < seg; i++) {
                triangleIndices.push_back(startIdx);
                triangleIndices.push_back(startIdx + 1 + i);
                triangleIndices.push_back(startIdx + 1 + i + 1);
            }
        } else {
            for (int i = 0; i <= seg; i++) {
                float angle = 2 * PI * i / seg;
                float x = cx + rad * cos(angle);
                float y = cy + rad * sin(angle);
                addPoint(x, y, r, g, b, false);
            }
            
            for (int i = 0; i < seg; i++) {
                lineIndices.push_back(startIdx + i);
                lineIndices.push_back(startIdx + i + 1);
            }
        }
    }
    
    void render() {
        if (vertices.empty()) return;
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        glVertexPointer(2, GL_FLOAT, 0, &vertices[0]);
        glColorPointer(3, GL_FLOAT, 0, &colors[0]);
        
        if (!triangleIndices.empty()) {
            glDrawElements(GL_TRIANGLES, triangleIndices.size(), 
                        GL_UNSIGNED_INT, &triangleIndices[0]);
        }
        
        if (!lineIndices.empty()) {
            glDrawElements(GL_LINES, lineIndices.size(), 
                        GL_UNSIGNED_INT, &lineIndices[0]);
        }
        
        if (!pointIndices.empty()) {
            glDrawElements(GL_POINTS, pointIndices.size(), 
                        GL_UNSIGNED_INT, &pointIndices[0]);
        }
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
};

VertexArrayScene backgroundScene;
VertexArrayScene midScene;
VertexArrayScene foregroundScene;


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

void drawRain(VertexArrayScene& scene) {
    if (!isRaining) return;
    
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        float r, g, b;
        if (isDay) {
            r = 0.8f; g = 0.8f; b = 1.0f;
        } else {
            r = 0.3f; g = 0.3f; b = 0.5f;
        }
        scene.addLine(raindrops[i].x, raindrops[i].y,
                      raindrops[i].x + 2, raindrops[i].y - raindrops[i].length,
                      r, g, b);
    }
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

void buildTree(float x, float y, float size, Color trunkColor, Color foliageColor, VertexArrayScene& scene) {
    scene.addRect(x - size*0.1f, y, size*0.2f, size*0.5f, 
             trunkColor.r, trunkColor.g, trunkColor.b);
    
    float foliageY = y + size*0.5f;
    
    scene.addTriangle(x - size*0.5f, foliageY,
                 x + size*0.5f, foliageY,
                 x, foliageY + size*0.8f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
    
    scene.addTriangle(x - size*0.4f, foliageY + size*0.4f,
                 x + size*0.4f, foliageY + size*0.4f,
                 x, foliageY + size*1.2f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
    
    scene.addTriangle(x - size*0.3f, foliageY + size*0.9f,
                 x + size*0.3f, foliageY + size*0.9f,
                 x, foliageY + size*1.5f,
                 foliageColor.r, foliageColor.g, foliageColor.b);
}

void buildSun(float x, float y, float radius, VertexArrayScene& scene) {
    scene.addCircle(x, y, radius, 1.0f, 1.0f, 0.0f);
    
    for (int i = 0; i < 12; i++) {
        float angle = 2 * PI * i / 12;
        float x1 = x + radius * 0.9f * cos(angle);
        float y1 = y + radius * 0.9f * sin(angle);
        float x2 = x + radius * 1.5f * cos(angle);
        float y2 = y + radius * 1.5f * sin(angle);
        scene.addLine(x1, y1, x2, y2, 1, 0.7f, 0);
    }

    scene.addCircle(x - radius*0.3f, y + radius*0.2f, radius*0.15f, 0, 0, 0, 15);
    scene.addCircle(x + radius*0.3f, y + radius*0.2f, radius*0.15f, 0, 0, 0, 15);
    scene.addPoint(x - radius*0.35f, y + radius*0.25f, 1, 1, 1);
    scene.addPoint(x + radius*0.25f, y + radius*0.25f, 1, 1, 1);
}

void buildMoon(float x, float y, float radius, VertexArrayScene& scene) {
    scene.addCircle(x, y, radius, 0.9f, 0.9f, 0.8f);
    
    scene.addCircle(x - radius*0.3f, y + radius*0.25f, radius*0.2f, 0.7f, 0.7f, 0.6f, 12);
    scene.addCircle(x + radius*0.35f, y - radius*0.2f, radius*0.25f, 0.7f, 0.7f, 0.6f, 12);
    scene.addCircle(x + radius*0.2f, y + radius*0.3f, radius*0.1f, 0.7f, 0.7f, 0.6f, 10);
}

void buildHouse(float x, float y, float w, float h, VertexArrayScene& scene) {
    scene.addRect(x, y, w, h, house.r, house.g, house.b);
    
    scene.addTriangle(x - w*0.1f, y + h, 
                 x + w*1.1f, y + h,
                 x + w*0.5f, y + h*1.4f,
                 roof.r, roof.g, roof.b);
    
    float doorW = w * 0.3f;
    float doorH = h * 0.5f;
    scene.addRect(x + w*0.35f, y, doorW, doorH, door.r, door.g, door.b);

    scene.addPoint(x + w*0.5f, y + doorH*0.5f, 1.0f, 1.0f, 0.0f, true);
    
    float windowW = w * 0.25f;
    float windowH = h * 0.25f;
    scene.addRect(x + w*0.1f, y + h*0.6f, windowW, windowH, 
             window.r, window.g, window.b);

    float winX = x + w*0.1f;
    float winY = y + h*0.6f;
    scene.addLine(winX + windowW/2, winY,
                  winX + windowW/2, winY + windowH,
                  0.3f, 0.2f, 0.1f);
    scene.addLine(winX, winY + windowH/2,
                  winX + windowW, winY + windowH/2,
                  0.3f, 0.2f, 0.1f);
}

void buildGrass(VertexArrayScene& scene) {
    scene.addRect(0, 0, width, height * 0.3f, 
                  ground.r, ground.g, ground.b, true);

    for (int i = 0; i < 15; i++) {
        float flowerX = 50 + i * 50;
        float flowerY = 30 + 10 * sin(i);
        
        scene.addLine(flowerX, flowerY,
                      flowerX, flowerY + 30,
                      stem.r, stem.g, stem.b);
        
        scene.addCircle(flowerX, flowerY + 35, 8, 
                        flower.r, flower.g, flower.b, 10, true);
    }
}

void buildStars(VertexArrayScene& scene) {
    if (!isDay) {
        for (int i = 0; i < NUM_STARS; i++) {
            float flicker = 0.7f + 0.3f * sin(t * 5 + stars[i].phase);
            float b = stars[i].brightness * flicker;
            
            scene.addPoint(stars[i].x, stars[i].y, b, b, b, true);
        }
    }
}

void buildForest(VertexArrayScene& scene) {
    float treeX[] = {50, 120, 190, 280, 350, 420, 490, 560, 630, 700, 750};
    float treeSize[] = {40, 55, 45, 60, 50, 65, 45, 55, 50, 60, 40};
    
    for (int i = 0; i < 11; i++) {
        buildTree(treeX[i], 150, treeSize[i], trunk, foliage, scene);
    }
    
    float smallTreeX[] = {130, 270, 410, 550, 680};
    for (int i = 0; i < 5; i++) {
        buildTree(smallTreeX[i], 130, 30, trunk, foliage, scene);
    }
}

void buildBackground() {
    backgroundScene.clear();
    float x = START_X + t * (END_X - START_X);
    float y = BASE_Y + ARC_HEIGHT * sin(t * PI);
    buildStars(backgroundScene);
    if (isDay)
        buildSun(x, y, radius, backgroundScene);
    else
        buildMoon(x, y, radius, backgroundScene);
}
void buildMid() {
    midScene.clear();
    buildGrass(midScene);
    buildForest(midScene);
}
void buildForeground() {
    foregroundScene.clear();
    buildHouse(500, 150, 120, 150, foregroundScene);
    buildHouse(200, 100, 80, 100, foregroundScene);
    drawRain(foregroundScene);
}

void display() {
    updateColors();
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    buildBackground();
    buildMid();
    buildForeground();
    backgroundScene.render();
    midScene.render();
    foregroundScene.render();

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