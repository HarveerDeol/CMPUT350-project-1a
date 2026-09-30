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
    context->ScreenContext->DrawRect(mBounds, CMPUT350::Colors::magenta);
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
