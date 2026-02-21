#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cstdlib>

class Canvas {
private:
    int width, height;
    std::vector<std::vector<char>> buffer;
    
public:
    Canvas(int w, int h) : width(w), height(h) {
        buffer.resize(height, std::vector<char>(width, ' '));
    }
    
    void clear() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                buffer[y][x] = ' ';
            }
        }
    }
    
    void setPixel(int x, int y, char c = '*') {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y][x] = c;
        }
    }
    
    // Алгоритм Брезенхема для линии
    void drawLine(int x1, int y1, int x2, int y2, char c = '*') {
        int dx = abs(x2 - x1);
        int dy = -abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx + dy;
        int e2;
        
        while (true) {
            setPixel(x1, y1, c);
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x1 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y1 += sy;
            }
        }
    }
    
    // Алгоритм Брезенхема для окружности
    void drawCircle(int xc, int yc, int radius, char c = '*') {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;
        
        drawCirclePoints(xc, yc, x, y, c);
        
        while (y >= x) {
            x++;
            if (d > 0) {
                y--;
                d = d + 4 * (x - y) + 10;
            } else {
                d = d + 4 * x + 6;
            }
            drawCirclePoints(xc, yc, x, y, c);
        }
    }
    
    void drawCirclePoints(int xc, int yc, int x, int y, char c) {
        setPixel(xc + x, yc + y, c);
        setPixel(xc - x, yc + y, c);
        setPixel(xc + x, yc - y, c);
        setPixel(xc - x, yc - y, c);
        setPixel(xc + y, yc + x, c);
        setPixel(xc - y, yc + x, c);
        setPixel(xc + y, yc - x, c);
        setPixel(xc - y, yc - x, c);
    }
    
    // Рисование треугольника (три линии)
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, char c = '*') {
        drawLine(x1, y1, x2, y2, c);
        drawLine(x2, y2, x3, y3, c);
        drawLine(x3, y3, x1, y1, c);
    }
    
    void display() {
        // Очистка консоли в Linux
        std::cout << "\033[2J\033[1;1H";  // ANSI escape codes для очистки экрана
        
        // Верхняя граница
        std::cout << "+";
        for (int x = 0; x < width; x++) std::cout << "-";
        std::cout << "+" << std::endl;
        
        // Содержимое
        for (int y = 0; y < height; y++) {
            std::cout << "|";
            for (int x = 0; x < width; x++) {
                std::cout << buffer[y][x];
            }
            std::cout << "|" << std::endl;
        }
        
        // Нижняя граница
        std::cout << "+";
        for (int x = 0; x < width; x++) std::cout << "-";
        std::cout << "+" << std::endl;
        
        std::cout << "Press Ctrl+C to exit..." << std::endl;
    }
    
    // Сохранение в файл
    void saveToFile(const std::string& filename) {
        FILE* file = fopen(filename.c_str(), "w");
        if (!file) return;
        
        fprintf(file, "+");
        for (int x = 0; x < width; x++) fprintf(file, "-");
        fprintf(file, "+\n");
        
        for (int y = 0; y < height; y++) {
            fprintf(file, "|");
            for (int x = 0; x < width; x++) {
                fprintf(file, "%c", buffer[y][x]);
            }
            fprintf(file, "|\n");
        }
        
        fprintf(file, "+");
        for (int x = 0; x < width; x++) fprintf(file, "-");
        fprintf(file, "+\n");
        
        fclose(file);
    }
};

int main() {
    const int WIDTH = 60;
    const int HEIGHT = 30;
    Canvas canvas(WIDTH, HEIGHT);
    
    int frameCount = 0;
    
    while (true) {
        // 1. Две пересекающиеся линии
        canvas.clear();
        canvas.drawLine(10, 5, 50, 25, '*');
        canvas.drawLine(10, 25, 50, 5, '#');
        canvas.display();
        canvas.saveToFile("lines.txt");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 2. Окружность
        canvas.clear();
        canvas.drawCircle(30, 15, 10, 'O');
        canvas.display();
        canvas.saveToFile("circle.txt");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 3. Треугольник
        canvas.clear();
        canvas.drawTriangle(20, 5, 40, 5, 30, 20, '@');
        canvas.display();
        canvas.saveToFile("triangle.txt");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 4. Комбинация (линии + окружность + треугольник)
        canvas.clear();
        canvas.drawLine(5, 5, 55, 5, '-');
        canvas.drawLine(5, 25, 55, 25, '-');
        canvas.drawCircle(30, 15, 8, 'O');
        canvas.drawTriangle(25, 10, 35, 10, 30, 18, '+');
        canvas.display();
        canvas.saveToFile("combination.txt");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        frameCount++;
    }
    
    return 0;
}