#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include "NotificationManager.h"
#include <algorithm>

namespace {
constexpr char kMoveLeftNotification[] = "player.move-left";
constexpr char kMoveRightNotification[] = "player.move-right";
constexpr char kFireNotification[] = "player.fire";
constexpr char kStopLeftNotification[] = "player.stop-left";
constexpr char kStopRightNotification[] = "player.stop-right";

constexpr float kMoveSpeed = 6.0f;
}

/**
 * @brief Constructs the player ship centered at the given location.
 * @param loc Center point of the player (CMPUT350::Point2D).
 */
Player::Player(CMPUT350::Point2D loc) : mLocation(loc), mBounds(CMPUT350::Point2D(0, 0), 20.0f), mAlive(true)

{
    mShapes.push_back(CMPUT350::Rect(-6.0f, -20.0f, 12.0f, 40.0f));   // nose and body
    mShapes.push_back(CMPUT350::Rect(-20.0f, -18.0f, 14.0f, 38.0f));  // left wing
    mShapes.push_back(CMPUT350::Rect(6.0f, -18.0f, 14.0f, 38.0f));    // right wing
}

/**
 * @brief No setup required; the constructor fully initializes the player.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;
    context->mNotificationManager->Register(context->CurrObject, kMoveLeftNotification);
    context->mNotificationManager->Register(context->CurrObject, kMoveRightNotification);
    context->mNotificationManager->Register(context->CurrObject, kFireNotification);
    context->mNotificationManager->Register(context->CurrObject, kStopLeftNotification);
    context->mNotificationManager->Register(context->CurrObject, kStopRightNotification);
}

/**
 * @brief The player has no per-frame logic outside of key-driven movement/firing.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::Update(CMPUT350::GameContext* context)
{
    mContext = context;

    float dx = 0.0f;
    if (mMovingLeft)
    {
        dx -= kMoveSpeed;
    }
    if (mMovingRight)
    {
        dx += kMoveSpeed;
    }

    if (dx == 0.0f)
    {
        return;
    }

    mLocation.x += dx;
    float screenWidth = static_cast<float>(context->ScreenContext->GetWindowWidth());
    mLocation.x = std::clamp(mLocation.x, 20.0f, screenWidth - 20.0f);
}

/**
 * @brief No late-update behaviour needed for the player.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * @brief Handles player actions delivered by the notification manager.
 * @param key The notification identifying the requested player action.
 */
void Player::ReceiveNotification(const std::string& key)
{
    if (mContext == nullptr)
    {
        return;
    }

    if (key == kFireNotification)
    {
        // Find an open bullet slot means the weak_ptr is expired or it was unassigned 
        for (auto& slot : mBullets)
        {
            if (slot.expired())
            {
                auto bullet = std::make_shared<Bullet>(
                    mLocation, CMPUT350::Point2D(0.0f, -1.0f), true, shared_from_this()); // heading straight up, player = true
                mContext->mEngineView->AddGameObject(bullet);
                slot = bullet; // weak_ptr now tracks this bullet's lifetime
                break; // only fire one bullet per press, even if both slots are open
            }
        }
        return;
    }

    if (key == kMoveLeftNotification)
    {
        mMovingLeft = true;
    }
    else if (key == kMoveRightNotification)
    {
        mMovingRight = true;
    }
    else if (key == kStopLeftNotification)
    {
        mMovingLeft = false;
    }
    else if (key == kStopRightNotification)
    {
        mMovingRight = false;
    }
}

/**
 * @brief The player has nothing to draw behind other objects.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

/**
 * @brief Draws the player as a small ship built from layered rectangles.
 * @param context Provides the draw context used to render (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* screen = context->ScreenContext;
    float x = 0.0f;
    float y = 0.0f;
    
    // collision box for debugging
    //screen->FrameRect(mBounds, 1.0f, CMPUT350::Colors::yellow);

    // main body
    screen->DrawRect(CMPUT350::Rect(x - 6.0f, y - 15.0f, 12.0f, 35.0f), CMPUT350::Colors::white);
    
    // tip
    screen->DrawRect(CMPUT350::Rect(x - 2.5f, y - 20.0f, 5.0f, 5.0f), CMPUT350::Colors::cyan);

    // inner colored wings
    screen->DrawRect(CMPUT350::Rect(x - 11.0f, y - 5.0f, 5.0f, 25.0f), CMPUT350::Colors::cyan);
    screen->DrawRect(CMPUT350::Rect(x + 6.0f, y - 5.0f, 5.0f, 25.0f), CMPUT350::Colors::cyan);

    // outer wings
    screen->DrawRect(CMPUT350::Rect(x - 16.0f, y - 1.0f, 5.0f, 18.0f), CMPUT350::Colors::grey);
    screen->DrawRect(CMPUT350::Rect(x + 11.0f, y - 1.0f, 5.0f, 18.0f), CMPUT350::Colors::grey);

    // front/forward cyan tips
    screen->DrawRect(CMPUT350::Rect(x - 20.0f, y - 18.0f, 4.0f, 9.0f), CMPUT350::Colors::cyan);
    screen->DrawRect(CMPUT350::Rect(x + 16.0f, y - 18.0f, 4.0f, 9.0f), CMPUT350::Colors::cyan);

    // white front crossbars connecting the tips
    screen->DrawRect(CMPUT350::Rect(x - 20.0f, y - 13.0f, 14.0f, 4.0f), CMPUT350::Colors::white);
    screen->DrawRect(CMPUT350::Rect(x + 6.0f, y - 13.0f, 14.0f, 4.0f), CMPUT350::Colors::white);

    // outer lower wingtips
    screen->DrawRect(CMPUT350::Rect(x - 20.0f, y + 8.0f, 4.0f, 9.0f), CMPUT350::Colors::white);
    screen->DrawRect(CMPUT350::Rect(x + 16.0f, y + 8.0f, 4.0f, 9.0f), CMPUT350::Colors::white);

    // red fuel light things on the back
    screen->DrawRect(CMPUT350::Rect(x - 19.25f, y + 17.0f, 2.0f, 1.0f), CMPUT350::Colors::red);
    screen->DrawRect(CMPUT350::Rect(x + 17.25f, y + 17.0f, 2.0f, 1.0f), CMPUT350::Colors::red);
}

/**
 * @brief Handles a collision with another collision object.
 * @param obj The object the player collided with (std::shared_ptr<CMPUT350::CollisionObject>).
 * @return Nothing (void).
 */
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && !bullet->IsPlayerBullet())
    {
        Kill();
    }
}

/**
 * @brief Marks the player as dead so the engine removes it next frame.
 * @return Nothing (void).
 */
void Player::Kill()
{
    mAlive = false;
}

/**
 * @brief Reports whether the player is still active.
 * @return True if alive, false if it should be removed by the engine (bool).
 */
bool Player::IsAlive() const
{
    return mAlive;
}

/**
 * @brief Returns the player's bounding box, used for collision checks.
 * @return Reference to the player's bounds, centered at its current location (const CMPUT350::Rect&).
 */
const CMPUT350::Rect& Player::GetBounds()
{
    return mBounds;
}

/// @brief Returns the collision shapes making up this object, in local coordinates relative to GetLocation. @return The shapes used for collision (const std::vector<CMPUT350::Shape>&).
const std::vector<CMPUT350::Shape> &Player::GetShapes() { return mShapes; }

/// @brief Returns the point all rotation and drawing is relative to. @return The centre of the object in screen coordinates (CMPUT350::Point2D).
CMPUT350::Point2D Player::GetLocation() const { return mLocation; }

/// @brief Returns the object's rotation. @return The rotation in radians, always 0 for this object (float).
float Player::GetRotation() const { return 0.0f; }

