#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <string>
#include <vector>

namespace CMPUT350 {

class GameObject;

/**
 * @brief The restricted view of the engine that game objects are allowed to use.
 *
 * Objects reach this through GameContext. It deliberately exposes only
 * what objects need (adding objects, registering key notifications,
 * iterating objects), not the full GameEngine interface.
 */
class EngineView {
    using tGameObject = std::vector<std::shared_ptr<GameObject>>;

public:
    using const_iterator = tGameObject::const_iterator;

    /// @brief Destroys the restricted engine view.
    virtual ~EngineView() = default;

    /// @brief Queues an object to join the game at the start of the next frame.
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;

    /// @brief Sends `notification` through the notification manager when the key with this scancode is pressed down.
    virtual void InstallKeyDownNotification(int, const std::string& notification) = 0;

    /// @brief Sends `notification` through the notification manager when the key with this scancode is released.
    virtual void InstallKeyUpNotification(int, const std::string& notification) = 0;

    /// @brief Iterators over the objects currently in the engine.
    /// @return Const iterator to the first object.
    virtual const_iterator cbegin() const = 0;
    /// @return Const iterator past the last object.
    virtual const_iterator cend() const = 0;
    /// @return Const iterator to the first object.
    virtual const_iterator begin() const = 0;
    /// @return Const iterator past the last object.
    virtual const_iterator end() const = 0;
};

}  // namespace CMPUT350

#endif   