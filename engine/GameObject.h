

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

class GameObject {
public:
    /// @brief Destroys the game object through its base type.
    virtual ~GameObject() = default;
    /// @brief Initializes the object before it enters the game. @param context The current game context (GameContext*).
    virtual void Initialize(GameContext *context);
    /// @brief Updates the object for one frame. @param context The current game context (GameContext*).
    virtual void Update(GameContext *context);
    /// @brief Performs the object's late update for one frame. @param context The current game context (GameContext*).
    virtual void LateUpdate(GameContext *context);
    /// @brief Renders the object's user interface. @param context The current game context (GameContext*).
    virtual void RenderUI(GameContext *context);
    /// @brief Reports whether the object is alive. @return True while the object remains active (bool).
    virtual bool IsAlive() const;
    /// @brief Marks the object as no longer alive.
    virtual void Kill();
    /// @brief Receives a notification. @param key The notification name (const std::string&).
    virtual void ReceiveNotification(const std::string& key);
private:
    bool mAlive = true;
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
