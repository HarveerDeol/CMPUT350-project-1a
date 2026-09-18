#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        // Returns euclidean distance between two points
        // calculate deltas
        float deltax = x - other.x;
        float deltay = y - other.y;

        // square them, add them together, and square root
        double result = std::sqrt(std::pow(deltax, 2) + std::pow(deltay,2));

        return result;
    }
    Point2D operator+(const Point2D &other) const {
        // return new Point2D with summed x and y values
        float sumx = x + other.x;
        float sumy = y + other.y;

        // return new point with summed x and y
        return Point2D(sumx,sumy);
    }
    Point2D operator+(const float &other) const {
        // add other to x and y separately, returns new Point2D
        float sumx = x + other;
        float sumy = y + other;
        return Point2D(sumx, sumy);
    }
    Point2D operator-(const Point2D &other) const {
        // get difference between x and y
        float diffx = x - other.x;
        float diffy = y - other.y;

        return Point2D(diffx,diffy);
    }
    Point2D operator-(const float &other) const {
        // subtract other from x and y
        float diffx = x - other;
        float diffy = y - other;

        return Point2D(diffx,diffy);
    }
    Point2D operator*(const float &scalar) const {
        // scale both x and y, returns new Point2D
        return Point2D(x*scalar,y*scalar);
    }
    Point2D &operator+=(const float &scalar) {
        // add scalar to x and y in place
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        // add other's x and y to this x and y respectively, in place
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        // subtract other's x and y to this x and y respectively, in place
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        return x == other.x && y == other.y;
    }
    Point2D &operator*=(const int &scalar) {
        // multiply by scalar in place
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Point2D &operator/=(const int &scalar) {
        // divide by scalar in place
        x /= scalar;
        y /= scalar;
        return *this;
    }
    float operator*(const Point2D &other) const {
        // dot product of this and other
        return x*other.x + y*other.y;
    }
    float Dot(Point2D b) const {
        // dot product of this and b
        return x*b.x + y*b.y;
    }
    static float Dot(Point2D a, Point2D b) {
        // dot product of a and b
        return a.x*b.x + a.y*b.y;
    }
    static float Cross(Point2D a, Point2D b) {
        // cross product of a and b
        return a.x*b.y - a.y*b.x;;
    }
    void Normalize() {
        // scaling so length is 1 but the direction is kept
        float length = std::sqrt(x*x + y*y);
        if (length != 0) {
            x /= length;
            y /= length;
        }
    }
};

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    // prints (x, y)
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {
    // handles left hand side scalar multiplication, returns new Point2D
    // multiplies rhs.x*number and rhs.y*number
    return rhs*number;
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        // TODO: write this code
        return 0;
    }
    Point2D ClosestPoint(const Point2D &p) const {
        // TODO: write this code
        return p;
    }
    bool Crosses(Line other, Point2D &crossingPoint) const {
        // TODO: write this code
        return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    // TODO: write this code
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(top, left)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator|=(const Point2D &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator|=(const Line &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator&=(const Rect &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        // TODO: write this code
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        // TODO: write this code
        return *this;
    }
    void Inset(int inset) {
        // TODO: write this code
    }
    bool IsInside(const Point2D &p) const {
        // TODO: write this code
        return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    // TODO: write this code
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
