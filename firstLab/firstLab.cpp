#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Константы
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PI = 3.14159f;

// Переменные состояния
float sunAngle = 0.0f;           // Угол солнца (0 = восход, 180 = закат)
float moonAngle = 180.0f;        // Угол луны (начинает с заката)
bool isDay = true;                // День или ночь
bool animationPaused = false;     // Пауза анимации
bool plantAnimation = false;      // Анимация роста растения
float plantHeight = 0.0f;         // Текущая высота растения
float plantGrowthRate = 0.01f;    // Скорость роста
float timeOfDay = 0.0f;           // Время суток (0-1)

// Позиции объектов
float sunX, sunY;
float moonX, moonY;

// Цвета для разных времен суток
struct Color {
    float r, g, b;
};

Color daySky = {0.5f, 0.8f, 1.0f};      // Голубое небо
Color nightSky = {0.05f, 0.05f, 0.1f};   // Темно-синее ночное небо
Color dayGround = {0.0f, 0.8f, 0.0f};    // Зеленая трава днем
Color nightGround = {0.0f, 0.2f, 0.0f};  // Темно-зеленая трава ночью
Color currentSky, currentGround;

// Функция для интерполяции цветов
Color interpolateColor(const Color& c1, const Color& c2, float t) {
    return {
        c1.r * (1 - t) + c2.r * t,
        c1.g * (1 - t) + c2.g * t,
        c1.b * (1 - t) + c2.b * t
    };
}

// Функция для рисования круга
void drawCircle(float x, float y, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Рисование облака
void drawCloud(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(x, y, size * 0.8f, 20);
    drawCircle(x + size * 0.7f, y + size * 0.2f, size * 0.7f, 20);
    drawCircle(x - size * 0.7f, y + size * 0.1f, size * 0.7f, 20);
}

// Рисование дерева
void drawTree(float x, float y, float size) {
    // Ствол
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(x - size * 0.1f, y);
    glVertex2f(x + size * 0.1f, y);
    glVertex2f(x + size * 0.1f, y + size * 0.8f);
    glVertex2f(x - size * 0.1f, y + size * 0.8f);
    glEnd();
    
    // Крона (зеленая, с оттенком в зависимости от времени суток)
    glColor3f(0.0f, 0.5f + currentGround.g * 0.3f, 0.0f);
    drawCircle(x, y + size * 0.9f, size * 0.4f, 15);
    drawCircle(x - size * 0.2f, y + size * 0.7f, size * 0.3f, 15);
    drawCircle(x + size * 0.2f, y + size * 0.7f, size * 0.3f, 15);
    drawCircle(x, y + size * 1.1f, size * 0.3f, 15);
}

// Рисование дома
void drawHouse(float x, float y, float size) {
    // Основная часть дома
    glColor3f(0.8f, 0.6f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x - size * 0.5f, y);
    glVertex2f(x + size * 0.5f, y);
    glVertex2f(x + size * 0.5f, y + size * 0.5f);
    glVertex2f(x - size * 0.5f, y + size * 0.5f);
    glEnd();
    
    // Крыша
    glColor3f(0.8f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - size * 0.6f, y + size * 0.5f);
    glVertex2f(x + size * 0.6f, y + size * 0.5f);
    glVertex2f(x, y + size * 0.9f);
    glEnd();
    
    // Дверь
    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x - size * 0.15f, y);
    glVertex2f(x + size * 0.15f, y);
    glVertex2f(x + size * 0.15f, y + size * 0.25f);
    glVertex2f(x - size * 0.15f, y + size * 0.25f);
    glEnd();
    
    // Окно
    glColor3f(0.9f, 0.9f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(x + size * 0.2f, y + size * 0.2f);
    glVertex2f(x + size * 0.4f, y + size * 0.2f);
    glVertex2f(x + size * 0.4f, y + size * 0.35f);
    glVertex2f(x + size * 0.2f, y + size * 0.35f);
    glEnd();
    
    // Рама окна
    glColor3f(0.3f, 0.2f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x + size * 0.3f, y + size * 0.2f);
    glVertex2f(x + size * 0.3f, y + size * 0.35f);
    glVertex2f(x + size * 0.2f, y + size * 0.275f);
    glVertex2f(x + size * 0.4f, y + size * 0.275f);
    glEnd();
}

// Рисование солнца
void drawSun(float x, float y, float size) {
    // Центр солнца
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(x, y, size, 30);
    
    // Лучики
    glColor3f(1.0f, 0.7f, 0.0f);
    for (int i = 0; i < 12; i++) {
        float angle = 2.0f * PI * i / 12;
        float dx = size * 1.5f * cos(angle);
        float dy = size * 1.5f * sin(angle);
        glBegin(GL_LINES);
        glVertex2f(x + size * 0.8f * cos(angle), y + size * 0.8f * sin(angle));
        glVertex2f(x + dx, y + dy);
        glEnd();
    }
    
    // Глазки и улыбка (только днем)
    if (isDay) {
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(x - size * 0.3f, y + size * 0.2f, size * 0.1f, 10);
        drawCircle(x + size * 0.3f, y + size * 0.2f, size * 0.1f, 10);
        
        glColor3f(0.8f, 0.2f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 10; i++) {
            float t = (float)i / 10.0f;
            float angle = PI * (0.7f + t * 0.6f);
            float dx = size * 0.4f * cos(angle - PI/2);
            float dy = -size * 0.2f + size * 0.2f * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }
}

// Рисование луны
void drawMoon(float x, float y, float size) {
    // Основной круг луны
    glColor3f(0.9f, 0.9f, 0.8f);
    drawCircle(x, y, size, 30);
    
    // Кратеры (немного темнее)
    glColor3f(0.7f, 0.7f, 0.6f);
    drawCircle(x - size * 0.2f, y + size * 0.1f, size * 0.15f, 15);
    drawCircle(x + size * 0.25f, y - size * 0.15f, size * 0.2f, 15);
    drawCircle(x + size * 0.1f, y + size * 0.25f, size * 0.12f, 12);
    
    // Тень от кратера
    glColor3f(0.5f, 0.5f, 0.5f);
    drawCircle(x - size * 0.18f, y + size * 0.12f, size * 0.08f, 10);
    drawCircle(x + size * 0.27f, y - size * 0.13f, size * 0.1f, 10);
    
    // Если не день - добавляем звезды вокруг луны
    if (!isDay) {
        glColor3f(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 5; i++) {
            float starX = x + size * 1.5f * cos(2*PI * i/5);
            float starY = y + size * 1.5f * sin(2*PI * i/5);
            glBegin(GL_POLYGON);
            for (int j = 0; j < 5; j++) {
                float angle = 2*PI * j/5;
                glVertex2f(starX + size * 0.1f * cos(angle), 
                          starY + size * 0.1f * sin(angle));
            }
            glEnd();
        }
    }
}

// Рисование растущего растения
void drawGrowingPlant(float x, float y, float height) {
    // Стебель
    glColor3f(0.0f, 0.8f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y);
    glVertex2f(x, y + height * 0.7f);
    glVertex2f(x + height * 0.2f, y + height);
    glEnd();
    
    // Листья
    glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLES);
    // Левый лист
    glVertex2f(x, y + height * 0.4f);
    glVertex2f(x - height * 0.15f, y + height * 0.5f);
    glVertex2f(x, y + height * 0.6f);
    // Правый лист
    glVertex2f(x, y + height * 0.4f);
    glVertex2f(x + height * 0.15f, y + height * 0.5f);
    glVertex2f(x, y + height * 0.6f);
    // Верхний лист
    glVertex2f(x + height * 0.1f, y + height * 0.8f);
    glVertex2f(x, y + height);
    glVertex2f(x - height * 0.1f, y + height * 0.8f);
    glEnd();
    
    // Цветок (если достаточно вырос)
    if (height > 0.5f) {
        glColor3f(1.0f, 0.0f + (height * 0.5f), 0.5f);
        drawCircle(x + height * 0.2f, y + height * 1.05f, height * 0.1f, 10);
        
        glColor3f(1.0f, 1.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            float angle = 2*PI * i/5;
            float fx = x + height * 0.2f + height * 0.15f * cos(angle);
            float fy = y + height * 1.05f + height * 0.15f * sin(angle);
            drawCircle(fx, fy, height * 0.05f, 8);
        }
    }
}

// Обновление позиций солнца и луны
void updatePositions() {
    float radius = 1.2f; // Радиус траектории
    
    if (isDay) {
        // Солнце движется по дуге от 0 до 180 градусов
        sunAngle += 0.5f; // Скорость движения
        if (sunAngle >= 180.0f) {
            sunAngle = 180.0f;
            isDay = false;
            moonAngle = 180.0f;
        }
        
        // Конвертируем угол в радианы и вычисляем позицию
        float rad = sunAngle * PI / 180.0f;
        sunX = radius * cos(rad - PI/2);  // Начинает слева (-1.2), идет в центр (0), затем направо (1.2)
        sunY = radius * sin(rad - PI/2) + 0.3f; // Высота с коррекцией
        
    } else {
        // Луна движется по дуге от 180 до 360 градусов
        moonAngle += 0.5f;
        if (moonAngle >= 360.0f) {
            moonAngle = 0.0f;
            isDay = true;
            sunAngle = 0.0f;
        }
        
        float rad = moonAngle * PI / 180.0f;
        moonX = radius * cos(rad - PI/2);
        moonY = radius * sin(rad - PI/2) + 0.3f;
    }
    
    // Вычисляем время суток для интерполяции цветов (0-1)
    if (isDay) {
        timeOfDay = sunAngle / 180.0f;
    } else {
        timeOfDay = 1.0f - (moonAngle - 180.0f) / 180.0f;
    }
    
    // Интерполяция цветов для плавного перехода
    currentSky = interpolateColor(daySky, nightSky, timeOfDay);
    currentGround = interpolateColor(dayGround, nightGround, timeOfDay);
}

// Функция отрисовки
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рисуем небо с текущим цветом
    glClearColor(currentSky.r, currentSky.g, currentSky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рисуем землю
    glColor3f(currentGround.r, currentGround.g, currentGround.b);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.5f);
    glVertex2f(1.0f, -0.5f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
    
    // Рисуем лес (несколько деревьев)
    drawTree(-0.7f, -0.4f, 0.3f);
    drawTree(-0.3f, -0.4f, 0.35f);
    drawTree(0.2f, -0.4f, 0.3f);
    drawTree(0.6f, -0.4f, 0.4f);
    
    // Рисуем дом
    drawHouse(-0.2f, -0.5f, 0.6f);
    
    // Рисуем поляну (цветы)
    srand(42);
    for (int i = 0; i < 10; i++) {
        float x = -0.9f + i * 0.2f;
        float y = -0.45f;
        if (i % 2 == 0) y = -0.4f;
        
        glColor3f(1.0f, 0.0f, 1.0f);
        drawCircle(x, y, 0.02f, 8);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawCircle(x + 0.03f, y + 0.03f, 0.01f, 6);
        drawCircle(x - 0.03f, y + 0.03f, 0.01f, 6);
    }
    
    // Рисуем растущее растение
    if (plantAnimation) {
        drawGrowingPlant(0.8f, -0.45f, plantHeight);
        plantHeight += plantGrowthRate;
        if (plantHeight > 1.0f) plantHeight = 0.0f;
    }
    
    // Рисуем облака (только днем)
    if (isDay) {
        drawCloud(-0.5f, 0.3f, 0.2f);
        drawCloud(0.6f, 0.5f, 0.15f);
    } else {
        // Звезды ночью
        glColor3f(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 50; i++) {
            float starX = -0.9f + (i % 10) * 0.2f;
            float starY = 0.3f + (i / 10) * 0.15f;
            float size = 0.01f + (rand() % 100) * 0.0005f;
            glPointSize(size * 20);
            glBegin(GL_POINTS);
            glVertex2f(starX, starY);
            glEnd();
        }
    }
    
    // Рисуем солнце или луну
    if (isDay) {
        drawSun(sunX, sunY, 0.15f);
    } else {
        drawMoon(moonX, moonY, 0.12f);
    }
    
    glutSwapBuffers();
}

// Таймер для анимации
void timer(int value) {
    if (!animationPaused) {
        updatePositions();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Обработка клавиатуры
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case ' ': // Пробел - пауза
            animationPaused = !animationPaused;
            printf("Анимация %s\n", animationPaused ? "остановлена" : "запущена");
            break;
        case 'p':
        case 'P':
            plantAnimation = !plantAnimation;
            if (plantAnimation) {
                plantHeight = 0.0f;
                printf("Растение начало расти\n");
            } else {
                printf("Рост растения остановлен\n");
            }
            break;
        case 'r':
        case 'R':
            // Сброс анимации
            sunAngle = 0.0f;
            moonAngle = 180.0f;
            isDay = true;
            plantHeight = 0.0f;
            plantAnimation = false;
            printf("Анимация сброшена\n");
            break;
        case 'd':
        case 'D':
            // Принудительно день
            isDay = true;
            sunAngle = 90.0f;
            printf("Переключено на день\n");
            break;
        case 'n':
        case 'N':
            // Принудительно ночь
            isDay = false;
            moonAngle = 270.0f;
            printf("Переключено на ночь\n");
            break;
    }
}

// Обработка специальных клавиш
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            plantGrowthRate += 0.005f;
            printf("Скорость роста: %.3f\n", plantGrowthRate);
            break;
        case GLUT_KEY_DOWN:
            plantGrowthRate -= 0.005f;
            if (plantGrowthRate < 0.0f) plantGrowthRate = 0.0f;
            printf("Скорость роста: %.3f\n", plantGrowthRate);
            break;
        case GLUT_KEY_LEFT:
            // Замедлить анимацию (увеличить задержку)
            break;
        case GLUT_KEY_RIGHT:
            // Ускорить анимацию
            break;
    }
}

// Инициализация
void init() {
    glClearColor(daySky.r, daySky.g, daySky.b, 1.0f);
    
    // Настройка прозрачности и сглаживания
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    // Настройка проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Начальные позиции
    sunX = -1.2f;
    sunY = 0.3f;
    moonX = 1.2f;
    moonY = 0.3f;
    
    currentSky = daySky;
    currentGround = dayGround;
    
    // Вывод подсказок
    printf("=== Анимация дня и ночи ===\n");
    printf("Управление:\n");
    printf("  Пробел - пауза/продолжить\n");
    printf("  P - запуск/остановка роста растения\n");
    printf("  R - сброс анимации\n");
    printf("  D - принудительно день\n");
    printf("  N - принудительно ночь\n");
    printf("  Стрелка вверх - увеличить скорость роста\n");
    printf("  Стрелка вниз - уменьшить скорость роста\n");
    printf("  ESC - выход\n");
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("День и ночь - Анимация");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    return 0;
}