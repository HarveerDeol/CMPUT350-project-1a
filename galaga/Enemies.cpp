#include "Enemies.h"

using CMPUT350::Point2D;
using CMPUT350::Rect;
using CMPUT350::Circle;

namespace {
/// @brief Which way is "away from the centre" for an enemy at x. @param x Enemy x position (float). @param width Window width (int). @return -1 if on the left half, +1 on the right (float).
float Outward(float x, int width)
{
    return x < static_cast<float>(width) / 2.0f ? -1.0f : 1.0f;
}

/// @brief True if an enemy at p is on screen and still above the player, so shooting makes sense. @param p Enemy position (Point2D). @param target Player position at attack start (Point2D). @return Whether firing is allowed (bool).
bool InFiringZone(const Point2D& p, const Point2D& target)
{
    return p.y > 0.0f && p.y < target.y - 60.0f;
}
}


/**
 * @brief Constructs a bee: 1 health, 50 points in formation / 100 attacking.
 * @param loc Formation slot (CMPUT350::Point2D).
 * @param path Entry Bezier control points (std::vector<CMPUT350::Point2D>).
 */
Enemy1::Enemy1(Point2D loc, const std::vector<Point2D>& path) : Enemy(loc, path)
{
    SetStats(1, 50, 100, CMPUT350::Colors::yellow, CMPUT350::Colors::yellow);
    SetCollision(Rect(-18.0f, -14.0f, 36.0f, 28.0f), Rect(-18.0f, -14.0f, 36.0f, 28.0f));
}

/**
 * @brief While attacking, fires an aimed shot at the player's attack-start position every 15 frames while above the player.
 * @param context Provides the engine view and current object for firing (CMPUT350::GameContext*).
 */
void Enemy1::LateUpdate(CMPUT350::GameContext* context)
{
    Enemy::LateUpdate(context);
    if (!IsAlive() || GetState() != State::kAttacking)
    {
        return;
    }
    if (mAttackFrame > 10 && mAttackFrame % 15 == 0 && InFiringZone(mLocation, mTarget))
    {
        FireBullet(context, mTarget - mLocation);
    }
}

/**
 * @brief Begins a wide looping dive: wind out away from the centre, swing over the player, exit the bottom.
 * @param context Provides the screen size (CMPUT350::GameContext*).
 * @param target The player's position (const CMPUT350::Point2D&).
 * @return True if the attack started, false if the enemy was not in formation (bool).
 */
bool Enemy1::Attack(CMPUT350::GameContext* context, const Point2D& target)
{
    if (GetState() != State::kInFormation)
    {
        return false;
    }
    const float W = static_cast<float>(context->ScreenContext->GetWindowWidth());
    const float H = static_cast<float>(context->ScreenContext->GetWindowHeight());
    const float o = Outward(mLocation.x, static_cast<int>(W));

    Point2D p0 = mLocation;
    Point2D p1 = p0 + Point2D(o * 120.0f, 80.0f);
    Point2D p2(target.x + o * 180.0f, target.y - 260.0f);
    Point2D p3(target.x, target.y - 120.0f);
    Point2D p4 = p3 * 2.0f - p2; // mirror of p2 about p3 keeps the join smooth
    Point2D p5(target.x - o * 140.0f, H + 40.0f);
    Point2D p6(target.x - o * 120.0f, H + 100.0f);

    BeginAttack({p0, p1, p2, p3, p4, p5, p6}, 8.0f, target);
    return true;
}

/**
 * @brief Draws a yellow body with blue wings, facing down in local space (rotation is applied by the engine).
 * @param context Provides the draw context (CMPUT350::GameContext*).
 */
void Enemy1::RenderForeground(CMPUT350::GameContext* context)
{
    auto* dc = context->ScreenContext;
    dc->DrawRect(Rect(-18.0f, -10.0f, 11.0f, 18.0f), CMPUT350::Colors::blue);
    dc->DrawRect(Rect(7.0f, -10.0f, 11.0f, 18.0f), CMPUT350::Colors::blue);
    dc->DrawCircle(Point2D(0.0f, 0.0f), 11.0f, CurrentColor());
    dc->DrawCircle(Point2D(-4.0f, 4.0f), 2.5f, CMPUT350::Colors::black);
    dc->DrawCircle(Point2D(4.0f, 4.0f), 2.5f, CMPUT350::Colors::black);
}


/**
 * @brief Constructs a butterfly: 1 health, 80 points in formation / 160 attacking.
 * @param loc Formation slot (CMPUT350::Point2D).
 * @param path Entry Bezier control points (std::vector<CMPUT350::Point2D>).
 */
Enemy2::Enemy2(Point2D loc, const std::vector<Point2D>& path) : Enemy(loc, path)
{
    SetStats(1, 80, 160, CMPUT350::Colors::magenta, CMPUT350::Colors::magenta);
    SetCollision(Circle(Point2D(0.0f, 0.0f), 16.0f), Rect(Point2D(0.0f, 0.0f), 16.0f));
}

/**
 * @brief While attacking, fires a three-way spread straight down every 25 frames while above the player.
 * @param context Provides the engine view and current object for firing (CMPUT350::GameContext*).
 */
void Enemy2::LateUpdate(CMPUT350::GameContext* context)
{
    Enemy::LateUpdate(context);
    if (!IsAlive() || GetState() != State::kAttacking)
    {
        return;
    }
    if (mAttackFrame > 10 && mAttackFrame % 25 == 0 && InFiringZone(mLocation, mTarget))
    {
        FireBullet(context, Point2D(-0.35f, 1.0f));
        FireBullet(context, Point2D(0.0f, 1.0f));
        FireBullet(context, Point2D(0.35f, 1.0f));
    }
}

/**
 * @brief Begins a zig-zag S-curve dive that crosses the screen once before passing the player and exiting the bottom.
 * @param context Provides the screen size (CMPUT350::GameContext*).
 * @param target The player's position (const CMPUT350::Point2D&).
 * @return True if the attack started, false if the enemy was not in formation (bool).
 */
bool Enemy2::Attack(CMPUT350::GameContext* context, const Point2D& target)
{
    if (GetState() != State::kInFormation)
    {
        return false;
    }
    const float W = static_cast<float>(context->ScreenContext->GetWindowWidth());
    const float H = static_cast<float>(context->ScreenContext->GetWindowHeight());
    const float o = Outward(mLocation.x, static_cast<int>(W));

    Point2D mid((mLocation.x + target.x) / 2.0f, (mLocation.y + target.y) / 2.0f);
    Point2D p0 = mLocation;
    Point2D p1(p0.x, p0.y + 140.0f);
    Point2D p2 = mid + Point2D(o * 260.0f, -60.0f);
    Point2D p3 = mid;
    Point2D p4 = p3 * 2.0f - p2;
    Point2D p5(target.x - o * 150.0f, target.y + 120.0f);
    Point2D p6(target.x - o * 150.0f, H + 100.0f);

    BeginAttack({p0, p1, p2, p3, p4, p5, p6}, 10.0f, target);
    return true;
}

/**
 * @brief Draws a magenta butterfly: two angled wings and a body.
 * @param context Provides the draw context (CMPUT350::GameContext*).
 */
void Enemy2::RenderForeground(CMPUT350::GameContext* context)
{
    auto* dc = context->ScreenContext;
    CMPUT350::RGBColor c = CurrentColor();
    dc->DrawLine(Point2D(0.0f, -2.0f), Point2D(-18.0f, -12.0f), 8.0f, c);
    dc->DrawLine(Point2D(0.0f, -2.0f), Point2D(18.0f, -12.0f), 8.0f, c);
    dc->DrawLine(Point2D(0.0f, 2.0f), Point2D(-14.0f, 14.0f), 8.0f, c);
    dc->DrawLine(Point2D(0.0f, 2.0f), Point2D(14.0f, 14.0f), 8.0f, c);
    dc->DrawCircle(Point2D(0.0f, 0.0f), 6.0f, CMPUT350::Colors::white);
}


/**
 * @brief Constructs a boss: 2 health, 150 points in formation / 400 attacking. Green at full health, purple after one hit.
 * @param loc Formation slot (CMPUT350::Point2D).
 * @param path Entry Bezier control points (std::vector<CMPUT350::Point2D>).
 */
Enemy3::Enemy3(Point2D loc, const std::vector<Point2D>& path) : Enemy(loc, path)
{
    SetStats(2, 150, 400, CMPUT350::Colors::green, CMPUT350::RGBColor(160, 40, 200));
    SetCollision(Rect(-20.0f, -16.0f, 40.0f, 32.0f), Rect(-20.0f, -16.0f, 40.0f, 32.0f));
}

/**
 * @brief While attacking, fires an aimed shot every 20 frames while above the player.
 * @param context Provides the engine view and current object for firing (CMPUT350::GameContext*).
 */
void Enemy3::LateUpdate(CMPUT350::GameContext* context)
{
    Enemy::LateUpdate(context);
    if (!IsAlive() || GetState() != State::kAttacking)
    {
        return;
    }
    if (mAttackFrame > 0 && mAttackFrame % 20 == 0 && InFiringZone(mLocation, mTarget))
    {
        FireBullet(context, mTarget - mLocation);
    }
}

/**
 * @brief Begins a slow heavy dive: drop straight down, then curve in on the player's x and exit the bottom.
 * @param context Provides the screen size (CMPUT350::GameContext*).
 * @param target The player's position (const CMPUT350::Point2D&).
 * @return True if the attack started, false if the enemy was not in formation (bool).
 */
bool Enemy3::Attack(CMPUT350::GameContext* context, const Point2D& target)
{
    if (GetState() != State::kInFormation)
    {
        return false;
    }
    const float H = static_cast<float>(context->ScreenContext->GetWindowHeight());

    Point2D p0 = mLocation;
    Point2D p1(p0.x, p0.y + 150.0f);
    Point2D p2(target.x, target.y - 200.0f);
    Point2D p3(target.x, H + 100.0f);

    BeginAttack({p0, p1, p2, p3}, 6.0f, target);
    return true;
}

/**
 * @brief Draws a wide body with horns and an eye; the body colour reflects remaining health.
 * @param context Provides the draw context (CMPUT350::GameContext*).
 */
void Enemy3::RenderForeground(CMPUT350::GameContext* context)
{
    auto* dc = context->ScreenContext;
    CMPUT350::RGBColor c = CurrentColor();
    dc->DrawRect(Rect(-20.0f, -10.0f, 40.0f, 24.0f), c);
    dc->DrawRect(Rect(-20.0f, -16.0f, 8.0f, 8.0f), c);
    dc->DrawRect(Rect(12.0f, -16.0f, 8.0f, 8.0f), c);
    dc->DrawCircle(Point2D(0.0f, 2.0f), 6.0f, CMPUT350::Colors::cyan);
}