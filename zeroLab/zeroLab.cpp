#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;

class Canvas {
private:
    unsigned int height;
    unsigned int width;
    vector<vector<char>> canvas = vector<vector<char>>();
    void initVector() {
        vector<char> tmp;
        canvas.clear();
        for (unsigned int i = 0; i < height; i++) {
            tmp = vector<char>();
            for (unsigned int j = 0; j < width; j++)
                tmp.push_back(' ');
            canvas.push_back(tmp);
        }
    }
public:
    Canvas(unsigned int height, unsigned int width) {
        this->height = height;
        this->width = width;
        if (height == 0 || width == 0)
            throw runtime_error("zero size");
        initVector();
    }
    void clear() {
        initVector();
    }
    void setElement(int y, int x, char element) {
        if ( y < 0 || x < 0 || y >= height || x >= width)
            throw runtime_error("out of range insertion");
        canvas[y][x] = element;
    }
    void print(ofstream& file) {
        for (unsigned int i = 0; i < width; i++) {
            std::cout << "-";
            if (file.is_open())
                file << "-";
        }
        std::cout << std::endl;
        if (file.is_open())
            file << std::endl;
        for (unsigned int i = 0; i < height; i++) {
            std::cout << "|";
            if (file.is_open())
                file << "|";
            for (unsigned int j = 0; j < width; j++) {
                std::cout << canvas[i][j];
                if (file.is_open())
                    file << canvas[i][j];
            }
            std::cout << "|" << std::endl;
            if (file.is_open())
                file << "|" << std::endl;
        }
        for (unsigned int i = 0; i < width; i++) {
            std::cout << "-";
            if (file.is_open())
                file << "-";
        }
        std::cout << std::endl;
        if (file.is_open())
            file << std::endl;
    }
};

void brezenchemAlgorithm(Canvas& canvas, int y_start, int x_start, int y_end, int x_end) {
    float error = 0;
    float delta_error;
    int dx = abs(x_end - x_start);
    int dy = abs(y_end - y_start);
    int stepX = (x_start < x_end) ? 1 : -1;
    int stepY = (y_start < y_end) ? 1 : -1;
    int x = x_start;
    int y = y_start;
    canvas.setElement(y, x, '*');
    if (dx >= dy) {
        delta_error = (float)dy/dx;
        for (int i = 1; i <= dx; i++) {
            x+=stepX;
            error += delta_error;
            if (error >= 0.5) {
                error -= 1;
                y+=stepY;
            }
            //std::cout << x << ";" << y << ";" << error << ";" << dx << ";" << dy << std::endl;
            canvas.setElement(y, x, '*');
        }
    }
    else {
        delta_error = (float)dx/dy;
        for (int i = 1; i <= dy; i++) {
            y+=stepY;
            error += delta_error;
            if (error >= 0.5) {
                error -= 1;
                x+=stepX;
            }
            canvas.setElement(y, x, '*');
        }
    }
}

void drawPointOnCircle(Canvas& canvas, int xc, int yc, int x, int y) {
    canvas.setElement(yc + y, xc + x, '*');
    canvas.setElement(yc + x, xc + y, '*');
    canvas.setElement(yc - x, xc + y, '*');
    canvas.setElement(yc - y, xc + x, '*');
    canvas.setElement(yc - y, xc - x, '*');
    canvas.setElement(yc - x, xc - y, '*');
    canvas.setElement(yc + x, xc - y, '*');
    canvas.setElement(yc + y, xc - x, '*');
}

void drawCircle(Canvas& canvas, int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2*r;
    drawPointOnCircle(canvas, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        drawPointOnCircle(canvas, xc, yc, x, y);
    }
}

void drawTriangle(Canvas& canvas, int x1, int y1, int x2, int y2, int x3, int y3) {
    brezenchemAlgorithm(canvas, y1, x1, y2, x2);
    brezenchemAlgorithm(canvas, y2, x2, y3, x3);
    brezenchemAlgorithm(canvas, y3, x3, y1, x1);
}

void test_case1(Canvas& canvas, ofstream& file) {
    std::cout << "1/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 10, 19);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 12, 17);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 14, 14);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    std::cout << "2/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 15, 14);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 17, 12);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 19, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case2(Canvas& canvas, ofstream& file) {
    std::cout << "3/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 19, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 17, 8);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 15, 5);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    std::cout << "4/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 14, 4);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 12, 2);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 10, 0);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case3(Canvas& canvas, ofstream& file) {
    std::cout << "5/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 10, 0);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 8, 2);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 5, 5);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    std::cout << "6/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 3, 7);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 2, 8);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 0, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case4(Canvas& canvas, ofstream& file) {
    std::cout << "7/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 0, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 2, 12);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 5, 15);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    std::cout << "8/8 четверть" << std::endl;
    brezenchemAlgorithm(canvas, 10, 10, 7, 17);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 8, 18);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    brezenchemAlgorithm(canvas, 10, 10, 10, 19);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case5(Canvas& canvas, ofstream& file) {
    drawCircle(canvas, 10, 10, 5);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    drawCircle(canvas, 10, 10, 9);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case6(Canvas& canvas, ofstream& file) {
    drawTriangle(canvas, 0, 0, 10, 0, 10, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    drawTriangle(canvas, 0, 0, 0, 10, 5, 5);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
    drawTriangle(canvas, 0, 0, 16, 0, 10, 10);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test_case7(Canvas& canvas, ofstream& file) {
    brezenchemAlgorithm(canvas, 0, 0, 10, 10);
    brezenchemAlgorithm(canvas, 10, 0, 0, 10);
    brezenchemAlgorithm(canvas, 5, 0, 5, 10);
    brezenchemAlgorithm(canvas, 0, 5, 10, 5);
    canvas.print(file);
    this_thread::sleep_for(chrono::seconds(1));
    canvas.clear();
}

void test(Canvas& canvas, const string& filename) {
    while (true) {
        std::ofstream file(filename, std::ios::trunc);
        if (!file.is_open()) {
            std::cout << "Проблемы с открытием файла" << std::endl;
            return;
        }
        test_case1(canvas, file);
        test_case2(canvas, file);
        test_case3(canvas, file);
        test_case4(canvas, file);
        test_case5(canvas, file);
        test_case6(canvas, file);
        test_case7(canvas, file);
        file.close();
    }
}

int main() {
    Canvas canvas = Canvas(20, 20);
    test(canvas, "test.txt");
    return 0;
}