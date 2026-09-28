#include "DrawContext.h"

namespace CMPUT350 {

//stores the shared window and font so every draw call has something to render onto
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

//draws text with its origin at its own center, so p marks the middle of the text rather than a corner
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
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

//draws text with its default origin so p marks the top-left corner of the text
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text drawText(*mFont);
    drawText.setString(text);
    drawText.setCharacterSize(static_cast<unsigned int>(pixelSize));

    sf::Color myColor(c.r, c.g, c.b);
    drawText.setFillColor(myColor);

    drawText.setPosition({p.x, p.y});

    mWindow->draw(drawText);
}

//draws a filled circle centered at p 
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);

    sf::Color myColor(c.r, c.g, c.b);
    circle.setFillColor(myColor);

    circle.setOrigin({radius, radius});
    circle.setPosition({p.x, p.y});

    mWindow->draw(circle);
}

//draws a filled rectangle using r.topLeft as the corner and r.width/height as its size
void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rect({r.width, r.height});

    sf::Color myColor(c.r, c.g, c.b);
    rect.setFillColor(myColor);

    rect.setPosition({r.topLeft.x, r.topLeft.y});

    mWindow->draw(rect);
}

//draws only the outline of a rectangle
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape rect({r.width, r.height});

    sf::Color myColor(c.r, c.g, c.b);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(myColor);
    rect.setOutlineThickness(width);

    rect.setPosition({r.topLeft.x, r.topLeft.y});

    mWindow->draw(rect);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
//builds a thin rotated rectangle to fake a width able line, since SFML has no native line width primitive.
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
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

//returns the window's current pixel width.
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

//returns the window's current pixel height.
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}   // namespace CMPUT350