#include "Bezier.h"
#include <cassert>
#include <cmath>

namespace CMPUT350 {
static bool GetSegment(const std::vector<Point2D> &pts, float t, size_t &base, float &u);

/**
 * @brief Constructs a Bezier curve that keeps its own copy of the control points.
 * @param pts The control points. Each cubic segment uses 4, consecutive segments share an endpoint, so 3k+1 points give k segments (const std::vector<Point2D>&).
 */
Bezier::Bezier(const std::vector<Point2D> &pts) : mPoints(pts) {
}

/**
 * @brief Returns the position on the stored curve at parameter t.
 * @param t Position along the whole curve, from 0 to the number of segments (float).
 * @return The point on the curve, or the origin if the stored control points do not form at least one cubic segment (Point2D).
 */
Point2D Bezier::GetPoint(float t) const {
    return GetPoint(mPoints, t);
}

/**
 * @brief Returns the tangent of the stored curve at parameter t.
 * @param t Position along the whole curve, from 0 to the number of segments (float).
 * @return The tangent vector, not normalized, or the origin if the stored control points do not form at least one cubic segment (Point2D).
 */
Point2D Bezier::GetSlope(float t) const {
    return GetSlope(mPoints, t);
}

/**
 * @brief Returns the position on a curve at parameter t without constructing a Bezier object.
 * @param pts The control points defining the curve (const std::vector<Point2D>&).
 * @param t Position along the whole curve, from 0 to the number of segments (float).
 * @return The point on the curve, or the origin if pts does not form at least one cubic segment (Point2D).
 */
Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t) {
    float u;
    size_t base;
    if (!GetSegment(pts, t, base, u)) {
        return Point2D(0, 0);
    }

    float invU = 1 - u;

    float w0 = invU * invU * invU;
    float w1 = 3 * invU * invU * u;
    float w2 = 3 * invU * u * u;
    float w3 = u * u * u;

    return pts[base] * w0 + pts[base + 1] * w1 + pts[base + 2] * w2 + pts[base + 3] * w3;
}

/**
 * @brief Returns the tangent of a curve at parameter t without constructing a Bezier object.
 * @param pts The control points defining the curve (const std::vector<Point2D>&).
 * @param t Position along the whole curve, from 0 to the number of segments (float).
 * @return The tangent vector, not normalized, or the origin if pts does not form at least one cubic segment (Point2D). Use atan2 on it to get a heading in radians.
 */
Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t) {
    size_t base;
    float u;
    if (!GetSegment(pts, t, base, u))
        return Point2D(0, 0);

    float invU = 1 - u;

    float w0 = 3 * invU * invU;
    float w1 = 6 * invU * u;
    float w2 = 3 * u * u;

    return (pts[base + 1] - pts[base]) * w0 + (pts[base + 2] - pts[base + 1]) * w1 + (pts[base + 3] - pts[base + 2]) * w2;
}

/**
 * @brief Works out which cubic segment a parameter t falls in, and where within it.
 * @param pts The control points to index into (const std::vector<Point2D>&).
 * @param t Position along the whole curve, from 0 to the number of segments (float).
 * @param base Output, the index of the first of the four control points for that segment (size_t&).
 * @param u Output, position within the segment from 0 to 1 (float&).
 * @return True if pts holds at least one whole cubic segment and the outputs were set, false otherwise (bool).
 *
 * A t landing exactly on the final endpoint is treated as the end of the last
 * segment, so the caller never indexes past the end of pts.
 */
static bool GetSegment(const std::vector<Point2D> &pts, float t, size_t &base, float &u){
    
    if (pts.size() < 4) {
        return false;
    }
    size_t numSegments = (pts.size() - 1) / 3;

    assert(0 <= t && t <= float(numSegments));

    size_t currSegment = std::floor(t);
    u = t - float(currSegment);
    if (currSegment >= numSegments) {
        currSegment = numSegments - 1;
        u = 1.0f;
    }

    base = currSegment * 3;

    // base and u have been set, just return true to indicate success
    return true;
}

}  // namespace CMPUT350
