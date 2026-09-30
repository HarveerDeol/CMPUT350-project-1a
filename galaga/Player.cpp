#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include <algorithm>

Player::Player(CMPUT350::Point2D loc) : mLocation(loc), mBounds(loc, 20.0f), mAlive(true)
{
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

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
        // #18: fire a bullet upward
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

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

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

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
        std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && bullet->IsPlayerBullet())
    {
        // do nothing, player bullets don't hurt the player
    }
}

void Player::Kill()
{
    mAlive = false;
}

bool Player::IsAlive() const
{
    return mAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    return mBounds;
}
