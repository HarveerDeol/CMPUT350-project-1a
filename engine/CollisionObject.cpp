#include "CollisionObject.h"
#include <limits>

namespace CMPUT350 {

namespace {

/**
 * @brief Tests whether two directed line segments cross.
 * @param a The first segment, treated as directed from its p1 to its p2 (const Line&).
 * @param b The segment to test against (const Line&).
 * @param contactPoint Output, set to the crossing point when the result is true (Point2D&).
 * @return True if the segments cross (bool).
 */
bool LineLine(const Line &a, const Line &b, Point2D &contactPoint) {
    return a.Crosses(b, contactPoint);
}

/**
 * @brief Tests whether a directed line segment reaches a circle, and where it first touches it.
 * @param l The segment, directed from p1 to p2 (const Line&).
 * @param c The circle to test against (const Circle&).
 * @param contactPoint Output, the first point along the line that touches the circle. Set to p1 when the line starts inside the circle (Point2D&).
 * @return True if any part of the segment lies within the circle (bool).
 */
bool LineCircle(const Line &l, const Circle &c, Point2D &contactPoint) {
    Point2D a = l.p1;
    Point2D b = l.p2;

    float x; // distance from a to the closest point on line ab
    Vector2D ab = b - a; // vector from a to b
    Vector2D ac = c.center - a; // vector from a to p
    float ablen = a.Distance(b); // length of ab

    if (ablen == 0){ // ab is a single point, check if its in the circle
        contactPoint = a;
        return a.Distance(c.center) <= c.radius;
    }
    
    x = ab.Dot(ac) / ablen;
    ab.Normalize();
    Point2D closest = a + ab*x; // closest point on line ab to c
    float cplen = closest.Distance(c.center);

    // if the closest point is outside the circle, return false
    if (cplen > c.radius) {
        return false;
    }

    float q = std::sqrt(c.radius * c.radius - cplen * cplen); // distance from closest point to intersection points

    // check if within line
    if (x+q < 0 || x-q > ablen)
        return false;

    // return the first intersection point
    if (x-q < 0){
        contactPoint = a; // line starts in the circle
    }
    else{ 
        contactPoint = closest - ab * q;
    }

    return true;
}

/**
 * @brief Tests whether a directed line segment reaches a rectangle, and where it first touches it.
 * @param l The segment, directed from p1 to p2 (const Line&).
 * @param r The axis-aligned rectangle to test against (const Rect&).
 * @param contactPoint Output, the first point along the line that touches the rect. Set to p1 when the line starts inside it (Point2D&).
 * @return True if any part of the segment lies within the rectangle (bool).
 */
bool LineRect(const Line &l, const Rect &r, Point2D &contactPoint) {
    // check first point of the line
    if (r.IsInside(l.p1)) {
        contactPoint = l.p1;
        return true;
    }
    // dont check second one, because the first intersection might be before there

    // four corners
    Point2D topLeft(r.topLeft.x, r.topLeft.y);
    Point2D topRight(r.topLeft.x + r.width, r.topLeft.y);
    Point2D bottomLeft(r.topLeft.x, r.topLeft.y + r.height);
    Point2D bottomRight(r.topLeft.x + r.width, r.topLeft.y + r.height);

    // make lines out of corners
    Line top(topLeft, topRight);
    Line bottom(bottomLeft, bottomRight);
    Line left(topLeft, bottomLeft);
    Line right(topRight, bottomRight);

    Line edges[4] = {top, right, bottom, left}; // clockwise order, starting from top edge
    Point2D temp; // where the edge intersects, if it does
    float tempDist = std::numeric_limits<float>::infinity(); // set to infinity, so any collision found is better than this
    bool found = false;
    for (size_t i = 0; i < 4; i++){
        if (l.Crosses(edges[i], temp)) {
            found = true;
            float d = l.p1.Distance(temp);
            if ( d < tempDist){
                contactPoint = temp;
                tempDist = d;
            }
        }
    }

    return found;
}

/**
 * @brief Tests whether two circles overlap.
 * @param a The first circle (const Circle&).
 * @param b The second circle (const Circle&).
 * @return True if the distance between centres is no greater than the sum of the radii, so touching counts (bool).
 */
bool CircleCircle(const Circle &a, const Circle &b) {
    return a.center.Distance(b.center) <= (a.radius + b.radius);
}

/**
 * @brief Tests whether a circle overlaps an axis-aligned rectangle.
 * @param c The circle (const Circle&).
 * @param r The rectangle (const Rect&).
 * @return True if the two overlap in any way (bool).
 */
bool CircleRect(const Circle &c, const Rect &r) {
    // four corners
    Point2D topLeft(r.topLeft.x, r.topLeft.y);
    Point2D topRight(r.topLeft.x + r.width, r.topLeft.y);
    Point2D bottomLeft(r.topLeft.x, r.topLeft.y + r.height);
    Point2D bottomRight(r.topLeft.x + r.width, r.topLeft.y + r.height);

    // make lines out of corners
    Line top(topLeft, topRight);
    Line bottom(bottomLeft, bottomRight);
    Line left(topLeft, bottomLeft);
    Line right(topRight, bottomRight);

    Point2D corners[4] = {topLeft, topRight, bottomRight, bottomLeft};

    Line edges[4] = {top, right, bottom, left}; // clockwise order, starting from top edge

    // check if circle is in rect
    if (r.IsInside(c.center)){
        return true;
    }

    // check if rect is inside circle, start with corners
    for (size_t i = 0; i < 4; i++){
        if (corners[i].Distance(c.center) <= c.radius){
            return true;
        }
    }

    // check if circle collides w any edges
    for (size_t i = 0; i < 4; i++){
        Point2D temp;
        if (LineCircle(edges[i], c, temp)){
            return true;
        }
    }

    return false;
}

/**
 * @brief Tests whether two axis-aligned rectangles overlap.
 * @param a The first rectangle (const Rect&).
 * @param b The second rectangle (const Rect&).
 * @return True only if the overlap has positive area.
 */
bool RectRect(const Rect &a, const Rect &b) {
    Rect overlap = a;
    overlap &= b;
    // If the overlap from &= has negative/0 width or height, then the rectangles do not intersect
    if (overlap.width <= 0 || overlap.height <= 0) {
        return false;
    }
    return true;
}

}

/**
 * @brief Tests any two collision shapes against each other, whatever their types.
 * @param a The first shape (const Shape&).
 * @param b The second shape (const Shape&).
 * @param contactPoint Output, the point of contact, valid only when hasPoint is set (Point2D&).
 * @param hasPoint Output, true when contactPoint holds a meaningful value (bool&).
 * @return True if the two shapes overlap (bool).
 */
bool Intersects(const Shape &a, const Shape &b, Point2D &contactPoint, bool &hasPoint) {

    hasPoint = false; // default to no contact point, unless we find one

    // --------------- a is a LINE
    if (a.t == ShapeType::kLine){
        if (b.t == ShapeType::kLine){
            hasPoint = LineLine(a.shape.line, b.shape.line, contactPoint);
            return hasPoint;
        }
        else if (b.t == ShapeType::kCircle){
            hasPoint = LineCircle(a.shape.line, b.shape.circle, contactPoint);
            return hasPoint;
        }
        else if (b.t == ShapeType::kRect){
            hasPoint = LineRect(a.shape.line, b.shape.rect, contactPoint);
            return hasPoint;
        }
    }

    // --------------- a is a RECT
    else if (a.t == ShapeType::kRect){
        if (b.t == ShapeType::kLine){
            hasPoint = LineRect(b.shape.line, a.shape.rect, contactPoint);
            return hasPoint;
        }
        else if (b.t == ShapeType::kCircle){
            return CircleRect(b.shape.circle, a.shape.rect);
        }
        else if (b.t == ShapeType::kRect){
            return RectRect(a.shape.rect, b.shape.rect);
        }
    }

    // --------------- a is a CIRCLE
    else if (a.t == ShapeType::kCircle){
        if (b.t == ShapeType::kLine){
            hasPoint = LineCircle(b.shape.line, a.shape.circle, contactPoint);
            return hasPoint;
        }
        else if (b.t == ShapeType::kCircle){
            return CircleCircle(a.shape.circle, b.shape.circle);
        }
        else if (b.t == ShapeType::kRect){
            return CircleRect(a.shape.circle, b.shape.rect);
        }
    }

    return false;
}

}  // namespace CMPUT350
