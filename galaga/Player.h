#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "CollisionObject.h"

class Bullet; 

class Player : public CMPUT350::CollisionObject, public std::enable_shared_from_this<Player>
{
private:
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;
    bool mAlive;
    CMPUT350::GameContext* mContext = nullptr;
    std::vector<CMPUT350::Shape> mShapes;

    bool mMovingLeft = false;
    bool mMovingRight = false;

    std::weak_ptr<Bullet> mBullets[2]; // tracks the player's two bullet slots

public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
};

#endif
