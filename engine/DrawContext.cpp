#include "DrawContext.h"

namespace CMPUT350 {

/**
 * @brief Creates a drawing context for a window and font.
 * @param window The window to draw into (std::shared_ptr<sf::RenderWindow>).
 * @param font The font used for text (std::shared_ptr<sf::Font>).
 */
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font), mOffset(0, 0), mRotation(0.0f) {}

/** @brief Sets the position offset used by drawing operations. @param p The offset (Point2D). */
void DrawContext::SetContextOffset(Vector2D p) { mOffset = p; }

/** @brief Sets the rotation used by drawing operations. @param rotation Rotation in radians (float). */
void DrawContext::SetContextRotation(float rotation) { mRotation = rotation; }

/** @brief Applies the current offset and rotation to a point. @param p The point to transform (Point2D). @return The transformed point (Point2D). */
Point2D DrawContext::Transform(Point2D p) {
    // work out the sine and cosine once, both lines of the rotation need them
    float sinRotation = std::sin(mRotation);
    float cosRotation = std::cos(mRotation);

    // rotate the point around the origin by mRotation
    float rotatedX = p.x * cosRotation - p.y * sinRotation;
    float rotatedY = p.x * sinRotation + p.y * cosRotation;

    // slide the rotated point out
    return Point2D(rotatedX + mOffset.x, rotatedY + mOffset.y);
}

/** @brief Reverses the current offset and rotation for a point. @param p The point to transform back (Point2D). @return The local point (Point2D). */
Point2D DrawContext::ReverseTransform(Point2D p) {
    float sinRotation = std::sin(mRotation);
    float cosRotation = std::cos(mRotation);

    // undo the slide
    float shiftedX = p.x - mOffset.x;
    float shiftedY = p.y - mOffset.y;

    // rotate back the other way
    float localX = shiftedX * cosRotation + shiftedY * sinRotation;
    float localY = -shiftedX * sinRotation + shiftedY * cosRotation;

    return Point2D(localX, localY);
}

/** @brief Transforms a rectangle and returns its enclosing axis-aligned bounds. @param r The rectangle to transform (Rect). @return The transformed bounds (Rect). */
Rect DrawContext::Transform(Rect r) {
    // get the four corners of the rectangle
    Point2D topLeft = r.topLeft;
    Point2D topRight = {r.topLeft.x + r.width, r.topLeft.y};
    Point2D bottomLeft = {r.topLeft.x, r.topLeft.y + r.height};
    Point2D bottomRight = {r.topLeft.x + r.width, r.topLeft.y + r.height};

    // transform each corner
    topLeft = Transform(topLeft);
    topRight = Transform(topRight);
    bottomLeft = Transform(bottomLeft);
    bottomRight = Transform(bottomRight);

    // find the new bounding box
    float minX = std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float maxX = std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float minY = std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
    float maxY = std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});

    return Rect(minX, minY, maxX - minX, maxY - minY);
}


/**
 * @brief Draws text centered at a point.
 * @param text The text to draw (const std::string&).
 * @param pixelSize Text size in pixels (int).
 * @param p The text center (Point2D).
 * @param c Text color (RGBColor).
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    p = Transform(p);
    sf::Text drawText(*mFont);
    drawText.setString(text);
    drawText.setCharacterSize(static_cast<unsigned int>(pixelSize));

    sf::Color myColor(c.r, c.g, c.b);
    drawText.setFillColor(myColor);

    sf::FloatRect bounds = drawText.getLocalBounds();
    drawText.setOrigin({
        bounds.position.x + bounds.size.x / 2.0f,
        bounds.position.y + bounds.size.y / 2.0f
    });

    drawText.setPosition({p.x, p.y});

    mWindow->draw(drawText);
}

/**
 * @brief Draws text with its top-left corner at a point.
 * @param text The text to draw (const std::string&).
 * @param pixelSize Text size in pixels (int).
 * @param p The text position (Point2D).
 * @param c Text color (RGBColor).
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    p = Transform(p);
    sf::Text drawText(*mFont);
    drawText.setString(text);
    drawText.setCharacterSize(static_cast<unsigned int>(pixelSize));

    sf::Color myColor(c.r, c.g, c.b);
    drawText.setFillColor(myColor);

    drawText.setPosition({p.x, p.y});

    mWindow->draw(drawText);
}

/** @brief Draws a filled circle. @param p The circle center (Point2D). @param radius The circle radius (float). @param c The circle color (RGBColor). */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    p = Transform(p);
    sf::CircleShape circle(radius);

    sf::Color myColor(c.r, c.g, c.b);
    circle.setFillColor(myColor);

    circle.setOrigin({radius, radius});
    circle.setPosition({p.x, p.y});

    mWindow->draw(circle);
}

/** @brief Draws a filled rectangle. @param r The rectangle to draw (Rect). @param c The fill color (RGBColor). */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    r.topLeft = Transform(r.topLeft);
    sf::RectangleShape rect({r.width, r.height});

    sf::Color myColor(c.r, c.g, c.b);
    rect.setFillColor(myColor);

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setRotation(sf::radians(mRotation));

    mWindow->draw(rect);
}

/** @brief Draws a rectangle outline. @param r The rectangle to outline (Rect). @param width Outline width in pixels (float). @param c The outline color (RGBColor). */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    r.topLeft = Transform(r.topLeft);
    sf::RectangleShape rect({r.width, r.height});

    sf::Color myColor(c.r, c.g, c.b);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(myColor);
    rect.setOutlineThickness(width);

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setRotation(sf::radians(mRotation));

    mWindow->draw(rect);
}

/** @brief Draws a line between two points. @param from The start point (Point2D). @param to The end point (Point2D). @param width Line width in pixels (float). @param c The line color (RGBColor). */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    from = Transform(from);
    to = Transform(to);
    float length = from.Distance(to);
    float angle = std::atan2(to.y - from.y, to.x - from.x) * 180.0f / 3.14159265f;

    sf::RectangleShape line({length, width});

    sf::Color myColor(c.r, c.g, c.b);
    line.setFillColor(myColor);

    line.setOrigin({0.0f, width / 2.0f});
    line.setPosition({from.x, from.y});
    line.setRotation(sf::degrees(angle));

    mWindow->draw(line);
}

/** @brief Gets the window width in pixels. @return The window width (int). */
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

/** @brief Gets the window height in pixels. @return The window height (int). */
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}   // namespace CMPUT350