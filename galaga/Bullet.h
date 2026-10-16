#ifndef BULLET_H
#define BULLET_H

#include <memory>

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D& collisionPoint) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

private:
    static constexpr float kSpeed = 600.0f; // pixels per second, tuned by feel later
    static constexpr float kHalfWidth = 2.0f; // half the width of the bullet's bounding box
    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mPrevLocation;
    CMPUT350::Point2D mHeading; // normalized direction of travel
    CMPUT350::Rect mBounds;
    static constexpr int kDeathFrames = 4;

    bool mIsPlayerBullet;
    bool mAlive = true;
    CMPUT350::Point2D mImpactPoint;
    bool mHasImpact = false;
    int mDeathTimer = -1;

    std::weak_ptr<CMPUT350::GameObject> mParent;
    std::vector<CMPUT350::Shape> mShapes;
};
#endif // BULLET_H
