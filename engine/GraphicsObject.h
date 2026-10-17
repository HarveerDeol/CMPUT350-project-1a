#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class GameContext;

class GraphicsObject : public GameObject {
public:
    /// @brief Renders the background layer. @param context The current game context (GameContext*).
    virtual void RenderBackground(GameContext *context);
    /// @brief Renders the foreground layer. @param context The current game context (GameContext*).
    virtual void RenderForeground(GameContext *context);
    /// @brief Gets the object's rotation. @return Rotation in radians (float).
    virtual float GetRotation() const = 0;
    /// @brief Gets the object's position. @return Position in screen coordinates (Point2D).
    virtual Point2D GetLocation() const = 0;
};

}  // namespace CMPUT350

#endif
