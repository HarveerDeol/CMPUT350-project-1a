#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include <algorithm>

/**
 * @brief Constructs the player ship centered at the given location.
 * @param loc Center point of the player (CMPUT350::Point2D).
 */
Player::Player(CMPUT350::Point2D loc) : mLocation(loc), mBounds(loc, 20.0f), mAlive(true)
{
}

/**
 * @brief No setup required; the constructor fully initializes the player.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::Initialize(CMPUT350::GameContext* context)
{
}

/**
 * @brief The player has no per-frame logic outside of key-driven movement/firing.
 * @param context Unused (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Player::Update(CMPUT350::GameContext* context)
{
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
 * @brief Handles player input: A/D to move horizontally, space to fire.
 * @param context Provides screen width for clamping and engine access for firing (CMPUT350::GameContext*).
 * @param key The ASCII key that was pressed (char).
 * @return True if the key was handled, false otherwise (bool).
 */
bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    const float moveSpeed = 10.0f;
    float dx = 0.0f;

    if (key == 'a' || key == 'A')
    {
        dx = -moveSpeed;
    }
    else if (key == 'd' || key == 'D')
    {
        dx = moveSpeed;
    }
        else if (key == ' ')
    {
        // Find an open bullet slot means the weak_ptr is expired or it was unassigned 
        for (auto& slot : mBullets)
        {
            if (slot.expired())
            {
                auto bullet = std::make_shared<Bullet>(
                    mLocation, CMPUT350::Point2D(0.0f, -1.0f), true); // heading straight up, player = true
                bullet->SetShooter(shared_from_this());
                context->mEngineView->AddGameObject(bullet);
                slot = bullet; // weak_ptr now tracks this bullet's lifetime
                break; // only fire one bullet per press, even if both slots are open
            }
        }
        return true;
    }
    else
    {
        return false;   // not a key we care about
    }

    mLocation.x += dx;
    float screenWidth = static_cast<float>(context->ScreenContext->GetWindowWidth());
    mLocation.x = std::clamp(mLocation.x, 20.0f, screenWidth - 20.0f);
    mBounds = CMPUT350::Rect(mLocation, 20.0f);
    return true;
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
    float x = mLocation.x;
    float y = mLocation.y;
    
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
    if (bullet != nullptr && bullet->IsPlayerBullet())
    {
        // do nothing, player bullets don't hurt the player
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
