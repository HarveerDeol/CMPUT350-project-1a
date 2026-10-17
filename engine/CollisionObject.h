#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>

#include "GraphicsObject.h"
#include "MathUtil.h"
#include <vector>

namespace CMPUT350 {

enum class ShapeType {
    kLine, kCircle, kRect
};
union ShapeUnion {
    /// @brief Stores a line collision shape. @param l The line (Line).
    ShapeUnion(Line l) : line(l) {}
    /// @brief Stores a circle collision shape. @param c The circle (Circle).
    ShapeUnion(Circle c) : circle(c) {}
    /// @brief Stores a rectangle collision shape. @param r The rectangle (Rect).
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};
struct Shape {
    /// @brief Creates a shape from a line. @param l The line (const Line&).
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    /// @brief Creates a shape from a circle. @param c The circle (const Circle&).
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    /// @brief Creates a shape from a rectangle. @param r The rectangle (const Rect&).
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};
bool Intersects(const Shape &a, const Shape &b, Point2D &contactPoint, bool &hasPoint);

class CollisionObject : public GraphicsObject {
public:
    /// @brief Handles a collision with another object. @param obj The other object (const std::shared_ptr<CollisionObject>&).
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;
    /// @brief Handles a collision and its contact point. @param obj The other object (const std::shared_ptr<CollisionObject>&). @param collisionPoint The contact point (const Point2D&).
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint) { CollisionEnter(obj); }
    /// @brief Gets the object's collision bounds. @return The bounds (const Rect&).
    virtual const Rect &GetBounds() = 0;
    // Return the internal shapes in the object. Shapes can be:
    // lines, points/circles, or axis-aligned rectangles
    /// @brief Gets the object's local collision shapes. @return The collision shapes (const std::vector<Shape>&).
    virtual const std::vector<Shape> &GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
