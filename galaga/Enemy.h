#ifndef ENEMY_H
#define ENEMY_H

#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"
#include "MathUtil.h"

/**
 * Base class for every Galaga enemy. It owns everything the enemies have in
 * common (flying along Bezier paths at constant speed, hovering in formation,
 * health, damage colours, collisions, death notification and scoring). Concrete
 * enemies only choose their stats and appearance, and implement Attack().
 *
 * Life cycle:  kEntering -> kInFormation -> kAttacking -> kReturning -> kInFormation ...
 */
class Enemy : public CMPUT350::CollisionObject
{
public:
    enum class State { kEntering, kInFormation, kAttacking, kReturning };

    /// Notification key sent (immediately) when an enemy is destroyed.
    static constexpr const char* kDiedKey = "enemy.died";

    Enemy(CMPUT350::Point2D formationPos, const std::vector<CMPUT350::Point2D>& path);
    /// @brief Destroys the enemy through its base type.
    ~Enemy() override = default;

    // GameObject Functions
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    using CMPUT350::CollisionObject::CollisionEnter; // keep the (obj, point) overload visible
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    // Enemy interface
    /// Asks the enemy to leave formation and attack. Returns false if it cannot right now.
    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) = 0;

    bool IsInFormation() const;
    State GetState() const;
    int GetHealth() const;

    /// Points awarded by the most recent death notification (0 if the enemy was not shot).
    static int LastPointsAwarded();

protected:
    // ---- Configuration helpers for subclasses (call from the subclass constructor) ----
    void SetStats(int health, int pointsInFormation, int pointsAttacking,
                  const CMPUT350::RGBColor& baseColor, const CMPUT350::RGBColor& damagedColor);
    void SetCollision(const std::vector<CMPUT350::Shape>& shapes, const CMPUT350::Rect& bounds);

    // ---- Behaviour helpers for subclasses ----
    void BeginAttack(const std::vector<CMPUT350::Point2D>& pts, float speed, const CMPUT350::Point2D& target);
    void FireBullet(CMPUT350::GameContext* context, const CMPUT350::Vector2D& direction);
    CMPUT350::RGBColor CurrentColor() const;

    CMPUT350::Point2D mLocation;          // current centre, in screen coordinates
    CMPUT350::Point2D mTarget;            // where the player was when the attack began
    int mAttackFrame = 0;                 // frames since the current attack began

    static constexpr float kEntrySpeed = 9.0f;   // pixels per frame while entering / returning
    static constexpr float kOffscreenMargin = 60.0f;

private:
    // ---- Constant-speed Bezier following ----
    void StartPath(const std::vector<CMPUT350::Point2D>& pts, float speed);
    bool AdvancePath();

    // ---- State transitions ----
    void EnterFormation();
    void StartReturn();
    void UpdateFormation(int frame);
    int PointsForCurrentState() const;

    static constexpr int kSamplesPerSegment = 64;   // arc-length table resolution
    static constexpr float kHoverAmplitude = 20.0f; // pixels
    static constexpr int kHoverPeriod = 90;         // frames for one full sway
    static constexpr int kHoverRampFrames = 15;     // fade the sway in after arriving
    static constexpr float kRotationEase = 0.25f;   // fraction of angle error removed per frame

    State mState = State::kEntering;
    CMPUT350::Point2D mFormationPos;
    float mRotation = 0.0f;
    int mFormationFrames = 0;             // frames spent in formation since last arrival

    // Current path being followed
    std::vector<CMPUT350::Point2D> mPathPts;
    std::vector<float> mPathLengths;      // cumulative arc length at each table sample
    float mPathMaxT = 0.0f;
    float mPathSpeed = 0.0f;
    float mPathDistance = 0.0f;
    bool mPathValid = false;

    // Stats
    int mHealth = 1;
    int mMaxHealth = 1;
    int mPointsInFormation = 50;
    int mPointsAttacking = 100;
    CMPUT350::RGBColor mBaseColor;
    CMPUT350::RGBColor mDamagedColor;

    // Death handling
    bool mAlive = true;
    bool mDying = false;
    bool mShotByPlayer = false;

    CMPUT350::Rect mBounds;
    std::vector<CMPUT350::Shape> mShapes;

    static int sLastPointsAwarded;
};

#endif // ENEMY_H