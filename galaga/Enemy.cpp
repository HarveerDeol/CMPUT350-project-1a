#include "Enemy.h"

#include <algorithm>
#include <cmath>

#include "Bezier.h"
#include "Bullet.h"
#include "NotificationManager.h"
#include "Player.h"

namespace {
constexpr float kPi = 3.14159265358979f;

/// @brief Wraps an angle into the range [-pi, pi]. @param a Angle in radians (float). @return The equivalent wrapped angle (float).
float NormalizeAngle(float a)
{
    while (a > kPi) a -= 2.0f * kPi;
    while (a < -kPi) a += 2.0f * kPi;
    return a;
}
}

int Enemy::sLastPointsAwarded = 0;

/**
 * @brief Constructs an enemy that flies along an entry path into its formation slot.
 * @param formationPos Where this enemy hovers once in formation (CMPUT350::Point2D).
 * @param path Cubic Bezier control points (3k+1 of them) for the entry flight, ideally ending at formationPos (std::vector<CMPUT350::Point2D>).
 */
Enemy::Enemy(CMPUT350::Point2D formationPos, const std::vector<CMPUT350::Point2D>& path)
    : mLocation(formationPos),
      mFormationPos(formationPos),
      mBaseColor(CMPUT350::Colors::white),
      mDamagedColor(CMPUT350::Colors::red),
      mBounds(CMPUT350::Point2D(0, 0), 18.0f)
{
    mShapes.push_back(CMPUT350::Rect(-18.0f, -18.0f, 36.0f, 36.0f));

    StartPath(path, kEntrySpeed);
    if (!mPathValid)
    {
        EnterFormation();
    }
}

/**
 * @brief Sets health, scoring and colours. Call from a subclass constructor.
 * @param health Starting (and maximum) health, at least 1 (int).
 * @param pointsInFormation Points awarded when shot while in formation (int).
 * @param pointsAttacking Points awarded when shot while entering, attacking or returning (int).
 * @param baseColor Colour at full health (CMPUT350::RGBColor).
 * @param damagedColor Colour at 1 health remaining; health in between blends the two (CMPUT350::RGBColor).
 */
void Enemy::SetStats(int health, int pointsInFormation, int pointsAttacking,
                     const CMPUT350::RGBColor& baseColor, const CMPUT350::RGBColor& damagedColor)
{
    mHealth = std::max(1, health);
    mMaxHealth = mHealth;
    mPointsInFormation = pointsInFormation;
    mPointsAttacking = pointsAttacking;
    mBaseColor = baseColor;
    mDamagedColor = damagedColor;
}

/**
 * @brief Replaces the collision shapes and bounding box (local coordinates around GetLocation).
 * @param shapes The collision shapes making up the enemy: lines, circles or axis-aligned rectangles (std::vector<CMPUT350::Shape>).
 * @param bounds Axis-aligned box that fully contains every shape (CMPUT350::Rect).
 */
void Enemy::SetCollision(const std::vector<CMPUT350::Shape>& shapes, const CMPUT350::Rect& bounds)
{
    mShapes = shapes;
    mBounds = bounds;
}

/**
 * @brief Builds the arc-length table for a Bezier path and starts following it.
 * @param pts Cubic Bezier control points, 3k+1 of them (std::vector<CMPUT350::Point2D>).
 * @param speed Distance travelled per frame in pixels (float).
 */
void Enemy::StartPath(const std::vector<CMPUT350::Point2D>& pts, float speed)
{
    mPathValid = false;
    mPathLengths.clear();
    if (pts.size() < 4 || (pts.size() - 1) % 3 != 0)
    {
        return;
    }

    mPathPts = pts;
    mPathSpeed = speed;
    mPathDistance = 0.0f;
    mPathMaxT = static_cast<float>((pts.size() - 1) / 3);

    const int samples = static_cast<int>(mPathMaxT) * kSamplesPerSegment;
    mPathLengths.reserve(samples + 1);
    mPathLengths.push_back(0.0f);

    CMPUT350::Point2D prev = CMPUT350::Bezier::GetPoint(mPathPts, 0.0f);
    for (int i = 1; i <= samples; i++)
    {
        float t = mPathMaxT * static_cast<float>(i) / static_cast<float>(samples);
        CMPUT350::Point2D p = CMPUT350::Bezier::GetPoint(mPathPts, t);
        mPathLengths.push_back(mPathLengths.back() + static_cast<float>(prev.Distance(p)));
        prev = p;
    }

    mLocation = mPathPts.front();
    mPathValid = true;
}

/**
 * @brief Moves the enemy one frame along the current path at constant speed and updates its rotation.
 * @return True when the end of the path has been reached (or there is no valid path), false otherwise (bool).
 */
bool Enemy::AdvancePath()
{
    if (!mPathValid)
    {
        return true;
    }

    const float total = mPathLengths.back();
    mPathDistance += mPathSpeed;
    const bool done = mPathDistance >= total;
    const float d = std::min(mPathDistance, total);

    auto it = std::upper_bound(mPathLengths.begin(), mPathLengths.end(), d);
    size_t hi = static_cast<size_t>(it - mPathLengths.begin());
    hi = std::min(std::max<size_t>(hi, 1), mPathLengths.size() - 1);
    const size_t lo = hi - 1;
    const float span = mPathLengths[hi] - mPathLengths[lo];
    const float frac = span > 0.0f ? (d - mPathLengths[lo]) / span : 0.0f;
    const float samples = static_cast<float>(mPathLengths.size() - 1);
    const float t = std::clamp(mPathMaxT * (static_cast<float>(lo) + frac) / samples, 0.0f, mPathMaxT);

    mLocation = CMPUT350::Bezier::GetPoint(mPathPts, t);

    CMPUT350::Point2D slope = CMPUT350::Bezier::GetSlope(mPathPts, t);
    if (slope.x != 0.0f || slope.y != 0.0f)
    {
        mRotation = std::atan2(slope.y, slope.x) - kPi / 2.0f;
    }
    return done;
}

/**
 * @brief Switches to the in-formation state, snapping to the formation slot.
 */
void Enemy::EnterFormation()
{
    mState = State::kInFormation;
    mFormationFrames = 0;
    mLocation = mFormationPos;
}

/**
 * @brief Starts the trip home after an attack: reappear above the screen over the slot and fly straight down into it.
 */
void Enemy::StartReturn()
{
    mState = State::kReturning;
    CMPUT350::Point2D top(mFormationPos.x, -kOffscreenMargin);
    CMPUT350::Point2D delta = mFormationPos - top;
    StartPath({top, top + delta * (1.0f / 3.0f), top + delta * (2.0f / 3.0f), mFormationPos}, kEntrySpeed);
}

/**
 * @brief Hovers left and right around the formation slot, synchronised with every other enemy via the global frame.
 * @param frame The current game frame from the GameContext (int).
 */
void Enemy::UpdateFormation(int frame)
{
    mFormationFrames++;
    const float ramp = std::min(1.0f, static_cast<float>(mFormationFrames) / static_cast<float>(kHoverRampFrames));
    const float phase = 2.0f * kPi * static_cast<float>(frame % kHoverPeriod) / static_cast<float>(kHoverPeriod);
    mLocation = mFormationPos + CMPUT350::Point2D(kHoverAmplitude * ramp * std::sin(phase), 0.0f);

    mRotation = NormalizeAngle(mRotation);
    mRotation += kRotationEase * NormalizeAngle(0.0f - mRotation);
}

/**
 * @brief Advances the enemy's state machine one frame.
 * @param context Provides the current frame for hover synchronisation (CMPUT350::GameContext*).
 */
void Enemy::Update(CMPUT350::GameContext* context)
{
    switch (mState)
    {
    case State::kEntering:
        if (AdvancePath())
        {
            EnterFormation();
        }
        break;
    case State::kInFormation:
        UpdateFormation(context->currentFrame);
        break;
    case State::kAttacking:
        mAttackFrame++;
        if (AdvancePath())
        {
            StartReturn();
        }
        break;
    case State::kReturning:
        if (AdvancePath())
        {
            EnterFormation();
        }
        break;
    }
}

/**
 * @brief Finishes off an enemy that was destroyed this frame and tells Galaga.
 * @param context Provides the notification manager (CMPUT350::GameContext*).
 */
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    if (!mDying || !mAlive)
    {
        return;
    }
    mAlive = false;
    sLastPointsAwarded = mShotByPlayer ? PointsForCurrentState() : 0;
    context->mNotificationManager->Notify(kDiedKey);
}

/**
 * @brief Points the enemy is worth right now: more when it is out of formation.
 * @return Points for the current state (int).
 */
int Enemy::PointsForCurrentState() const
{
    return mState == State::kInFormation ? mPointsInFormation : mPointsAttacking;
}

/**
 * @brief Starts an attack along a Bezier path. Called by subclasses from Attack().
 * @param pts Cubic Bezier control points; the first must be the enemy's current location, and the path should end off the bottom of the screen (std::vector<CMPUT350::Point2D>).
 * @param speed Distance travelled per frame in pixels (float).
 * @param target The player's position when the attack began (CMPUT350::Point2D).
 */
void Enemy::BeginAttack(const std::vector<CMPUT350::Point2D>& pts, float speed, const CMPUT350::Point2D& target)
{
    mTarget = target;
    mAttackFrame = 0;
    mState = State::kAttacking;
    StartPath(pts, speed);
    if (!mPathValid)
    {
        StartReturn(); // invalid path: just go home rather than getting stuck
    }
}

/**
 * @brief Fires an enemy bullet from the enemy's current location.
 * @param context Provides the engine view to add the bullet to, and this enemy as the bullet's parent (CMPUT350::GameContext*).
 * @param direction Direction of travel; the bullet normalizes it (CMPUT350::Point2D).
 *
 * Must be called from an engine callback (Update/LateUpdate) so context->CurrObject is this enemy.
 * The bullet destroys itself if this enemy dies.
 */
void Enemy::FireBullet(CMPUT350::GameContext* context, const CMPUT350::Point2D& direction)
{
    auto bullet = std::make_shared<Bullet>(mLocation, direction, false, context->CurrObject);
    context->mEngineView->AddGameObject(bullet);
}

/**
 * @brief The colour to draw with: base colour at full health, blending to the damaged colour as health drops.
 * @return Colour for the enemy's current health (CMPUT350::RGBColor).
 */
CMPUT350::RGBColor Enemy::CurrentColor() const
{
    float t = mMaxHealth > 1 ? static_cast<float>(mMaxHealth - mHealth) / static_cast<float>(mMaxHealth - 1) : 0.0f;
    auto mix = [t](uint8_t a, uint8_t b) { return static_cast<uint8_t>(a + (b - a) * t); };
    return CMPUT350::RGBColor(mix(mBaseColor.r, mDamagedColor.r),
                              mix(mBaseColor.g, mDamagedColor.g),
                              mix(mBaseColor.b, mDamagedColor.b));
}

/**
 * @brief Enemies have nothing to draw behind other objects.
 * @param context Unused (CMPUT350::GameContext*).
 */
void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

/**
 * @brief Default appearance: a square in the current health colour. Subclasses override this with their own shape.
 * @param context Provides the draw context (CMPUT350::GameContext*).
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(CMPUT350::Rect(-18.0f, -18.0f, 36.0f, 36.0f), CurrentColor());
}

/**
 * @brief Reacts to a collision. Player bullets cost one health, touching the player destroys the enemy, everything else is ignored.
 * @param obj The object this enemy collided with (std::shared_ptr<CMPUT350::CollisionObject>).
 */
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (mDying)
    {
        return;
    }

    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr)
    {
        if (bullet->IsPlayerBullet())
        {
            mHealth--;
            if (mHealth <= 0)
            {
                mDying = true;
                mShotByPlayer = true;
            }
        }
        return;
    }

    if (std::dynamic_pointer_cast<Player>(obj) != nullptr)
    {
        mDying = true; // ramming the player destroys the enemy but scores nothing
        mShotByPlayer = false;
    }
}

/**
 * @brief Removes the enemy silently (no notification, no points). Used by Galaga when clearing the screen.
 */
void Enemy::Kill()
{
    mAlive = false;
}

/**
 * @brief Reports whether this enemy is still active.
 * @return True if alive, false if the engine should remove it (bool).
 */
bool Enemy::IsAlive() const
{
    return mAlive;
}

/**
 * @brief Reports whether the enemy is settled in its formation slot.
 * @return True only in the in-formation state (bool).
 */
bool Enemy::IsInFormation() const
{
    return mState == State::kInFormation;
}

/**
 * @brief Returns the enemy's current state.
 * @return The state machine state (Enemy::State).
 */
Enemy::State Enemy::GetState() const
{
    return mState;
}

/**
 * @brief Returns the enemy's remaining health.
 * @return Health points left (int).
 */
int Enemy::GetHealth() const
{
    return mHealth;
}

/**
 * @brief Points awarded by the most recent death notification. Galaga reads this inside ReceiveNotification(kDiedKey).
 * @return Points for the enemy that just died, 0 if it was not shot (int).
 */
int Enemy::LastPointsAwarded()
{
    return sLastPointsAwarded;
}

/**
 * @brief Returns the bounding box in local coordinates, centred on GetLocation.
 * @return The collision bounds (const CMPUT350::Rect&).
 */
const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}

/// @brief Returns the point all rotation and drawing is relative to. @return The enemy's current centre in screen coordinates (CMPUT350::Point2D).
CMPUT350::Point2D Enemy::GetLocation() const { return mLocation; }

/// @brief Returns the enemy's heading. @return Rotation in radians; 0 means facing down the screen (float).
float Enemy::GetRotation() const { return mRotation; }

/// @brief Returns the collision shapes in local coordinates relative to GetLocation. @return The shapes used for collision (const std::vector<CMPUT350::Shape>&).
const std::vector<CMPUT350::Shape>& Enemy::GetShapes() { return mShapes; }