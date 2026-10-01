#include "Enemy.h"
#include "Bullet.h"

/**
 * @brief Constructs a stationary enemy centered at the given location.
 * @param loc Center point of the enemy (CMPUT350::Point2D).
 */
Enemy::Enemy(CMPUT350::Point2D loc) : mLocation(loc), mBounds(loc, 20.0f), mAlive(true)
{
}

/**
 * @brief No setup required; the constructor fully initializes the enemy.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

/**
 * @brief Enemies do not move, so there is nothing to update each frame.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Enemy::Update(CMPUT350::GameContext* context)
{
}

/**
 * @brief No late-update behaviour needed for a stationary enemy.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * @brief Enemies do not respond to keyboard input.
 * @param context Unused (CMPUT350::GameContext*).
 * @param key Unused (char).
 * @return Always false, since the key was not handled (bool).
 */
bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

/**
 * @brief Enemies have nothing to draw behind other objects.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

/**
 * @brief Draws the enemy as a filled yellow rectangle.
 * @param context Provides the draw context used to render (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::yellow);
}

/**
 * @brief Handles a collision with another collision object.
 * @param obj The object this enemy collided with (std::shared_ptr<CMPUT350::CollisionObject>).
 * @return Nothing (void).
 */
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && bullet->IsPlayerBullet())
    {
        Kill();
    }
}

/**
 * @brief Marks this enemy as dead so the engine removes it next frame.
 * @return Nothing (void).
 */
void Enemy::Kill()
{
    mAlive = false;
}

/**
 * @brief Reports whether this enemy is still active.
 * @return True if alive, false if it should be removed by the engine (bool).
 */
bool Enemy::IsAlive() const
{
    return mAlive;
}

/**
 * @brief Returns the enemy's bounding box, used for collision checks.
 * @return Reference to the enemy's bounds, centered at its spawn location (const CMPUT350::Rect&).
 */
const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}
