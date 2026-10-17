#include "GameObject.h"

namespace CMPUT350 {

/** @brief Initializes the game object. @param context The current game context (GameContext*). */
void GameObject::Initialize(GameContext *context) { return; }
/** @brief Updates the game object for one frame. @param context The current game context (GameContext*). */
void GameObject::Update(GameContext *context) { return; }
/** @brief Performs the object's late update for one frame. @param context The current game context (GameContext*). */
void GameObject::LateUpdate(GameContext *context) { return; }
/** @brief Renders the object's user interface. @param context The current game context (GameContext*). */
void GameObject::RenderUI(GameContext *contextrender) { return; }
/** @brief Receives a notification from the engine. @param key The notification name (const std::string&). */
void GameObject::ReceiveNotification(const std::string& key) { return; }
/** @brief Reports whether the object is alive. @return True while the object remains active (bool). */
bool GameObject::IsAlive() const { return mAlive; }
/** @brief Marks the object as no longer alive. */
void GameObject::Kill() {mAlive = false;}
}  // namespace CMPUT350
