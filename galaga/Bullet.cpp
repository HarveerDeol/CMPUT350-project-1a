#include "Bullet.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    : mLocation(location), mPrevLocation(location), mHeading(heading), mIsPlayerBullet(player)
{
    mHeading.Normalize(); // ensure movement speed is consistent regardless of the input vector's length
    mBounds = CMPUT350::Rect(mPrevLocation, mLocation); // zero-size box at spawn until first Update
}

bool Bullet::IsPlayerBullet()
{
    return mIsPlayerBullet;
}

void Bullet::SetShooter(const std::shared_ptr<CMPUT350::CollisionObject>& shooter)
{
    mShooter = shooter;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
    
}

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

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false; // bullets don't respond to input directly
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = mIsPlayerBullet ? CMPUT350::Colors::cyan : CMPUT350::Colors::red;
    context->ScreenContext->DrawLine(mPrevLocation, mLocation, 2.0f, color);
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{

    if (auto shooter = mShooter.lock()) {
        if (shooter == obj) {
            return;
        }
    }

    Kill(); // bullets are consumed on their first real hit
}

void Bullet::Kill()
{
    mAlive = false;
}

bool Bullet::IsAlive() const
{
    return mAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}
