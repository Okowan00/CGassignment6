
// Compile with: g++ phong_shading_final.cpp -o main -lGL -lGLU -lglut
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <array>
#include <limits>
#include <algorithm>
#include <iostream>

const int WIDTH = 512, HEIGHT = 512;
unsigned char framebuffer[HEIGHT][WIDTH][3];
float zbuffer[HEIGHT][WIDTH];

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vec3 {
    float x, y, z;
    Vec3(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const {
        float len = std::sqrt(x * x + y * y + z * z);
        return len > 0 ? (*this) * (1.0f / len) : Vec3();
    }
};

std::vector<Vec3> vertices;
std::vector<Vec3> vertexNormals;
std::vector<std::array<int, 3>> indices;

void clearBuffers() {
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x) {
            framebuffer[y][x][0] = framebuffer[y][x][1] = framebuffer[y][x][2] = 0;
            zbuffer[y][x] = std::numeric_limits<float>::infinity();
        }
}

void setPixel(int x, int y, const Vec3& color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    framebuffer[y][x][0] = (unsigned char)(std::pow(std::clamp(color.x, 0.0f, 1.0f), 1.0f / 2.2f) * 255.0f);
    framebuffer[y][x][1] = (unsigned char)(std::pow(std::clamp(color.y, 0.0f, 1.0f), 1.0f / 2.2f) * 255.0f);
    framebuffer[y][x][2] = (unsigned char)(std::pow(std::clamp(color.z, 0.0f, 1.0f), 1.0f / 2.2f) * 255.0f);
}

void applyTransform(Vec3& v) {
    float l = -0.1f, r = 0.1f, b = -0.1f, t = 0.1f, n = -0.1f, f = -1000.0f;
    float x = (2 * n * v.x) / (r - l);
    float y = (2 * n * v.y) / (t - b);
    float z = (f + n) / (f - n) * v.z + (2 * f * n) / (f - n);
    float w = -v.z;
    x /= w; y /= w; z /= w;
    v.x = ((x + 1) * 0.5f) * WIDTH;
    v.y = ((y + 1) * 0.5f) * HEIGHT;
    v.z = z;
}

Vec3 computeLighting(const Vec3& pos, const Vec3& normal) {
    Vec3 N = normal.normalize();
    Vec3 light = Vec3(-4, 4, -3);
    Vec3 L = Vec3(-light.x, -light.y, light.z) - pos; // visual match to example image
    L = L.normalize();
    Vec3 V = (Vec3(0, 0, 0) - pos).normalize();
    Vec3 R = (N * (2.0f * N.dot(L)) - L).normalize();

    float Ia = 0.2f;
    Vec3 ka(0, 1, 0), kd(0, 0.5f, 0), ks(0.5f, 0.5f, 0.5f);
    float diffuse = std::max(0.0f, N.dot(L));
    float specular = std::pow(std::max(0.0f, R.dot(V)), 32.0f);
    return ka * Ia + kd * diffuse + ks * specular;
}

void rasterizePhong(Vec3 v0_scr, Vec3 n0, Vec3 v1_scr, Vec3 n1, Vec3 v2_scr, Vec3 n2,
    Vec3 v0_cam, Vec3 v1_cam, Vec3 v2_cam) {
    int minX = std::max(1, (int)std::floor(std::min({ v0_scr.x, v1_scr.x, v2_scr.x })));
    int maxX = std::min(WIDTH - 2, (int)std::ceil(std::max({ v0_scr.x, v1_scr.x, v2_scr.x })));
    int minY = std::max(1, (int)std::floor(std::min({ v0_scr.y, v1_scr.y, v2_scr.y })));
    int maxY = std::min(HEIGHT - 2, (int)std::ceil(std::max({ v0_scr.y, v1_scr.y, v2_scr.y })));

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float denom = (v1_scr.y - v2_scr.y) * (v0_scr.x - v2_scr.x) + (v2_scr.x - v1_scr.x) * (v0_scr.y - v2_scr.y);
            float w0 = ((v1_scr.y - v2_scr.y) * (x - v2_scr.x) + (v2_scr.x - v1_scr.x) * (y - v2_scr.y)) / denom;
            float w1 = ((v2_scr.y - v0_scr.y) * (x - v2_scr.x) + (v0_scr.x - v2_scr.x) * (y - v2_scr.y)) / denom;
            float w2 = 1.0f - w0 - w1;
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float z = w0 * v0_scr.z + w1 * v1_scr.z + w2 * v2_scr.z;
                if (z < zbuffer[y][x]) {
                    zbuffer[y][x] = z;
                    Vec3 interpPos = v0_cam * w0 + v1_cam * w1 + v2_cam * w2;
                    Vec3 interpNormal = (n0 * w0 + n1 * w1 + n2 * w2).normalize();
                    Vec3 color = computeLighting(interpPos, interpNormal);
                    setPixel(x, y, color);
                }
            }
        }
    }
}

void createSphere(int width = 32, int height = 16) {
    float radius = 2.0f;
    vertices.clear(); vertexNormals.clear(); indices.clear();

    for (int j = 1; j < height - 1; ++j) {
        float theta = M_PI * j / (height - 1);
        for (int i = 0; i < width; ++i) {
            float phi = 2 * M_PI * i / width;
            float x = radius * sinf(theta) * cosf(phi);
            float y = radius * cosf(theta);
            float z = radius * sinf(theta) * sinf(phi);
            vertices.emplace_back(x, y, z - 7.0f);
            vertexNormals.emplace_back(0, 0, 0);
        }
    }
    vertices.emplace_back(0, radius, -7);
    vertices.emplace_back(0, -radius, -7);
    vertexNormals.emplace_back(0, 0, 0);
    vertexNormals.emplace_back(0, 0, 0);

    int top = vertices.size() - 2;
    int bottom = vertices.size() - 1;

    for (int i = 0; i < width; ++i) {
        indices.push_back({ top, i, (i + 1) % width });
        indices.push_back({ bottom, (height - 3) * width + (i + 1) % width, (height - 3) * width + i });
    }
    for (int j = 0; j < height - 3; ++j) {
        for (int i = 0; i < width; ++i) {
            int idx = j * width + i;
            int next = (i + 1) % width;
            indices.push_back({ idx, (j + 1) * width + next, (j + 1) * width + i });
            indices.push_back({ idx, j * width + next, (j + 1) * width + next });
        }
    }

    for (const auto& tri : indices) {
        Vec3 v0 = vertices[tri[0]];
        Vec3 v1 = vertices[tri[1]];
        Vec3 v2 = vertices[tri[2]];
        Vec3 normal = (v1 - v0).cross(v2 - v0).normalize();
        vertexNormals[tri[0]] += normal;
        vertexNormals[tri[1]] += normal;
        vertexNormals[tri[2]] += normal;
    }
    for (auto& n : vertexNormals) n = n.normalize();
}

void render() {
    clearBuffers();
    for (const auto& tri : indices) {
        Vec3 v0 = vertices[tri[0]];
        Vec3 v1 = vertices[tri[1]];
        Vec3 v2 = vertices[tri[2]];
        Vec3 n0 = vertexNormals[tri[0]];
        Vec3 n1 = vertexNormals[tri[1]];
        Vec3 n2 = vertexNormals[tri[2]];

        Vec3 v0_scr = v0, v1_scr = v1, v2_scr = v2;
        applyTransform(v0_scr);
        applyTransform(v1_scr);
        applyTransform(v2_scr);

        rasterizePhong(v0_scr, n0, v1_scr, n1, v2_scr, n2, v0, v1, v2);
    }
}

void display() {
    render();
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
    glutSwapBuffers();
}

void initOpenGL() {
    glClearColor(0, 0, 0, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Phong Shading");
    initOpenGL();
    createSphere();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
