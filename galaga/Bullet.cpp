#include "Bullet.h"

/**
 * @brief Constructs a bullet at a starting location, travelling in a given direction.
 * @param location Starting position of the bullet (CMPUT350::Point2D).
 * @param heading Direction of travel; normalized internally so speed is independent of input length (CMPUT350::Point2D).
 * @param player True if fired by the player, false if fired by an enemy (bool).
 */
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    : mLocation(location), mPrevLocation(location), mHeading(heading), mIsPlayerBullet(player)
{
    mHeading.Normalize(); // ensure movement speed is consistent regardless of the input vector's length
    mBounds = CMPUT350::Rect(mPrevLocation, mLocation); // zero-size box at spawn until first Update
}

/**
 * @brief Reports who fired this bullet.
 * @return True if this is a player bullet, false if an enemy bullet (bool).
 */
bool Bullet::IsPlayerBullet()
{
    return mIsPlayerBullet;
}

/**
 * @brief Records the object that fired this bullet, so it can be ignored on first collision.
 * @param shooter The firing object, stored as a weak_ptr so the bullet does not keep it alive (std::shared_ptr<CMPUT350::CollisionObject>).
 * @return Nothing (void).
 */
void Bullet::SetShooter(const std::shared_ptr<CMPUT350::CollisionObject>& shooter)
{
    mShooter = shooter;
}

/**
 * @brief No setup required beyond the constructor.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Bullet::Initialize(CMPUT350::GameContext* context)
{
    
}

/**
 * @brief Advances the bullet along its heading and rebuilds its bounding box.
 * @param context Provides access to screen dimensions for off-screen checks (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Bullet::Update(CMPUT350::GameContext* context)
{
    mPrevLocation = mLocation;
    mLocation += mHeading * (kSpeed / 30.0f);

    mBounds = CMPUT350::Rect(mPrevLocation, mLocation);
    mBounds.Inset(-kHalfWidth);
    // kill bullets once they've left the screen so they don't accumulate forever.
    float screenWidth = static_cast<float>(context->ScreenContext->GetWindowWidth());
    float screenHeight = static_cast<float>(context->ScreenContext->GetWindowHeight());
    if (mLocation.x < 0 || mLocation.x > screenWidth ||
        mLocation.y < 0 || mLocation.y > screenHeight)
    {
        Kill();
    }
}

/**
 * @brief No late-update behaviour needed for a bullet.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * @brief Bullets do not respond to keyboard input.
 * @param context Unused (CMPUT350::GameContext*).
 * @param key Unused (char).
 * @return Always false, since the key was not handled (bool).
 */
bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false; // bullets don't respond to input directly
}

/**
 * @brief Bullets have nothing to draw behind other objects.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

/**
 * @brief Draws the bullet as a short line from its previous to current position.
 * @param context Provides the draw context used to render (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = mIsPlayerBullet ? CMPUT350::Colors::cyan : CMPUT350::Colors::red;
    context->ScreenContext->DrawLine(mPrevLocation, mLocation, 2.0f, color);
}

/**
 * @brief Handles a collision with another collision object.
 * @param obj The object this bullet collided with (std::shared_ptr<CMPUT350::CollisionObject>).
 * @return Nothing (void).
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{

    if (auto shooter = mShooter.lock()) {
        if (shooter == obj) {
            return;
        }
    }

    Kill(); // bullets are consumed on their first real hit
}

/**
 * @brief Marks this bullet as dead so the engine removes it next frame.
 * @return Nothing (void).
 */
void Bullet::Kill()
{
    mAlive = false;
}

/**
 * @brief Reports whether this bullet is still active.
 * @return True if alive, false if it should be removed by the engine (bool).
 */
bool Bullet::IsAlive() const
{
    return mAlive;
}

/**
 * @brief Returns the bullet's current bounding box, used for collision checks.
 * @return Reference to the bullet's bounds, spanning its previous and current position (const CMPUT350::Rect&).
 */
const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}
