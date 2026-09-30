#ifndef BULLET_H
#define BULLET_H

#include <memory>

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player);
    bool IsPlayerBullet();

    // lets whoever fires this bullet register itself so the bullet can
    // ignore its very first collision with its own shooter 
    void SetShooter(const std::shared_ptr<CMPUT350::CollisionObject>& shooter);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

private:
    static constexpr float kSpeed = 600.0f; // pixels per second, tuned by feel later
    static constexpr float kHalfWidth = 2.0f; // half the width of the bullet's bounding box
    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mPrevLocation;
    CMPUT350::Point2D mHeading; // normalized direction of travel
    CMPUT350::Rect mBounds;

    bool mIsPlayerBullet;
    bool mAlive = true;

    std::weak_ptr<CMPUT350::CollisionObject> mShooter;
};
#endif // BULLET_H
