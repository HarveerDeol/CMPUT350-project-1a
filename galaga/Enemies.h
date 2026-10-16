#ifndef ENEMIES_H
#define ENEMIES_H

#include "Enemy.h"

/// 1 health, blue wings on a yellow body wide looping dive that crosses over the player and fires aimed shots
class Enemy1 : public Enemy
{
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path);
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

/// 1 health, magenta zigzag S curve dive that fires three way spreads
class Enemy2 : public Enemy
{
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path);
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

/// boss 2 health green, turns purple when hit  slow heavy dive straight at the player firing aimed bursts.
class Enemy3 : public Enemy
{
public:
    Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path);
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif // ENEMIES_H