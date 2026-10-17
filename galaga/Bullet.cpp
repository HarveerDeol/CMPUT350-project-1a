#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"

/**
 * @brief Constructs a bullet at a starting location, travelling in a given direction.
 * @param location Starting position of the bullet (CMPUT350::Point2D).
 * @param heading Direction of travel; normalized internally so speed is independent of input length (CMPUT350::Point2D).
 * @param player True if fired by the player, false if fired by an enemy (bool).
 * @param parent The object that fired this bullet (std::weak_ptr<CMPUT350::GameObject>).
 */
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent)
    : mLocation(location), mPrevLocation(location), mHeading(heading), mIsPlayerBullet(player), mParent(parent)
{
    mHeading.Normalize(); // ensure movement speed is consistent regardless of the input vector's length
    mBounds = CMPUT350::Rect(mPrevLocation - mLocation, CMPUT350::Point2D(0, 0)); // zero-size box at spawn until first Update
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
    // handle life updates
    if (mParent.expired()) {
        Kill();
        return;
    }
    if (mDeathTimer > 0) {
        mDeathTimer--;
        return;
    }
    if (mDeathTimer == 0) {
        Kill();
        return;
    }

    mPrevLocation = mLocation;
    mLocation += mHeading * (kSpeed / 30.0f);

    mBounds = CMPUT350::Rect(mPrevLocation - mLocation, CMPUT350::Point2D(0, 0));
    mBounds.Inset(-kHalfWidth);
    mShapes.clear();
    mShapes.push_back(CMPUT350::Line(mPrevLocation - mLocation, CMPUT350::Point2D(0, 0)));
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

    if (mHasImpact) {
        // circle grows over time to show impact 
        float progress = float(kDeathFrames - mDeathTimer) / float(kDeathFrames);
        float radius = 2.0f + 8.0f * progress;

        // draw circle of impact point
        context->ScreenContext->DrawCircle(mImpactPoint, radius, color);
    }
    else {
        context->ScreenContext->DrawLine(mPrevLocation - mLocation, CMPUT350::Point2D(0, 0), 2.0f, color);
    }
}

/**
 * @brief Handles a collision with another collision object.
 * @param obj The object this bullet collided with (std::shared_ptr<CMPUT350::CollisionObject>).
 * @param collisionPoint The point of collision (CMPUT350::Point2D).
 * @return Nothing (void).
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D& collisionPoint)
{
    if (mIsPlayerBullet)
    {
        if (std::dynamic_pointer_cast<Enemy>(obj) == nullptr) return;
    }
    else
    {
        if (std::dynamic_pointer_cast<Player>(obj) == nullptr) return;
    }

    if (auto parent = mParent.lock()){
        if (parent == obj) {
            return;
        }
    }
    if (!mHasImpact || collisionPoint.y > mImpactPoint.y) {
        mImpactPoint = collisionPoint;
        mHasImpact = true;
    }

    // start short timer to allow the death to be shown
    if (mDeathTimer < 0) {
        mDeathTimer = kDeathFrames;
    }
}

/**
 * @brief Handles a collision without a contact point.
 * @param obj The other collision object (const std::shared_ptr<CMPUT350::CollisionObject>&).
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (mIsPlayerBullet)
    {
        if (std::dynamic_pointer_cast<Enemy>(obj) == nullptr) return;
    }
    else
    {
        if (std::dynamic_pointer_cast<Player>(obj) == nullptr) return;
    }

    if (obj == mParent.lock()) {
        return;
    }

    if (mDeathTimer < 0) {
        mDeathTimer = kDeathFrames;
    }
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

/// @brief Returns the point all rotation and drawing is relative to. @return The centre of the object in screen coordinates (CMPUT350::Point2D).
CMPUT350::Point2D Bullet::GetLocation() const { return mLocation; }
/// @brief Returns the object's rotation. @return The rotation in radians, always 0 for this object (float).
float Bullet::GetRotation() const { return 0.0f; }

/// @brief Returns the collision shapes making up this bullet, in local coordinates relative to GetLocation. The line spanning the previous and current positions is rebuilt each frame in Update. @return The shapes used for collision (const std::vector<CMPUT350::Shape>&).
const std::vector<CMPUT350::Shape>& Bullet::GetShapes() { return mShapes; }
