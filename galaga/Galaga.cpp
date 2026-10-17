#include "Galaga.h"
#include "Enemies.h"
#include "Enemy.h"
#include "Player.h"
#include "Stars.h"
#include "NotificationManager.h"
#include "Keyboard.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr char kCoin[] = "game.coin";
constexpr char kStart[] = "game.start";
constexpr char kEnemyDied[] = "enemy.died";

constexpr int kStartupFrames = 60;    // two seconds showing the level number
constexpr int kGameOverFrames = 120;  // four seconds showing game over
constexpr int kRespawnFrames = 45;
constexpr int kWaveCount = 5;
constexpr int kShipsPerWave = 8;
constexpr int kFramesBetweenWaves = 30;
}

/**
 * @brief Creates the Galaga game manager.
 * @param width Game window width in pixels (float).
 * @param height Game window height in pixels (float).
 */
Galaga::Galaga(float width, float height) : mWidth(width), mHeight(height)
{
}

/**
 * @brief Registers game notifications and key mappings.
 * @param context The current game context (CMPUT350::GameContext*).
 */
void Galaga::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;

    context->mNotificationManager->Register(context->CurrObject, kCoin);
    context->mNotificationManager->Register(context->CurrObject, kStart);
    context->mNotificationManager->Register(context->CurrObject, kEnemyDied);

    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, kCoin);
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, kStart);

    // WASD and the arrow keys both work
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, "player.move-left");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Left, "player.move-left");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, "player.move-right");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Right, "player.move-right");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, "player.fire");

    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, "player.stop-left");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Left, "player.stop-left");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, "player.stop-right");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Right, "player.stop-right");
    context->mEngineView->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, mWidth, mHeight)));
}
/**
 * @brief Advances the game state and timers for one frame.
 * @param context The current game context (CMPUT350::GameContext*).
 */
void Galaga::Update(CMPUT350::GameContext* context)
{
    // state machine
    mContext = context;

    if (mState == State::kStartup) // waiting for the startup timer to expire
    {
        mStateTimer--;
        if (mStateTimer <= 0) // spawn the player and start the waves
        {
            if (mPlayer.expired()){
                SpawnPlayer();
            }
            ResetWaveProgress();
            mState = State::kWaves;
        }
    }
    else if (mState == State::kWaves)
    {
        if (mWaveIndex < kWaveCount)
        {
            if (mSpawnTimer > 0)
            {
                --mSpawnTimer;
            }
            else
            {
                SpawnNextEnemy();
            }
        }
        else if (AreAllEnemiesInFormation())
        {
            mState = State::kPlaying;
        }
    }
    else if (mState == State::kPlaying){
        if (mPlayer.expired()) { // player has been killed, check if they have lives left
            mLives--;
            if (mLives > 0) {
                mState = State::kRespawn;
                mStateTimer = kRespawnFrames;
            }
            else {
                mStateTimer = kGameOverFrames;
                mState = State::kGameOver;
            }
        }
        else if (mEnemiesAlive <= 0) { // all enemies have been killed, start the next wave
            mLevel++;
            mStateTimer = kStartupFrames;
            mState = State::kStartup;
        }
        else
        {
            SelectAttacker();
        }
    }
    else if (mState == State::kRespawn)
    {
        if (AreAttackCyclesComplete())
        {
            --mStateTimer;
            if (mStateTimer <= 0)
            {
                SpawnPlayer();
                mState = State::kPlaying;
            }
        }
    }
    else if (mState == State::kGameOver)
    {
        mStateTimer--;
        if (mStateTimer <= 0)
        {
            ClearObjects();
            mState = State::kWaiting;
        }
    }
}

/**
 * @brief Draws the score and current game-state message.
 * @param context The current game context (CMPUT350::GameContext*).
 */
void Galaga::RenderUI(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* gui = context->GUIContext;

    // draw score
    gui->DrawText("Score: " + std::to_string(mScore), 24, CMPUT350::Point2D(10.0f, 10.0f), CMPUT350::Colors::white);

    // draw lives
    if (mState != State::kWaiting)
    {
        gui->DrawText("LIVES " + std::to_string(mLives), 24, CMPUT350::Point2D(mWidth - 200.0f, 10.0f), CMPUT350::Colors::white);
    }

    CMPUT350::Point2D center(mWidth / 2.0f, mHeight / 2.0f);

    if (mState == State::kWaiting)
    {
        if (mCoins > 0)
        {
            gui->DrawCenteredText("PRESS 1 TO START", 48, center, CMPUT350::Colors::white);
        }
        else
        {
            gui->DrawCenteredText("INSERT COIN", 48, center, CMPUT350::Colors::white);
        }
    }
    else if (mState == State::kStartup)
    {
        gui->DrawCenteredText("LEVEL " + std::to_string(mLevel), 48, center, CMPUT350::Colors::white);
    }
    else if (mState == State::kRespawn)
    {
        gui->DrawCenteredText("LIVES " + std::to_string(mLives), 48, center, CMPUT350::Colors::white);
    }
    else if (mState == State::kGameOver)
    {
        gui->DrawCenteredText("GAME OVER", 48, center, CMPUT350::Colors::white);
    }
}

/**
 * @brief Handles coin, start, and enemy-death notifications.
 * @param key The notification name (const std::string&).
 */
void Galaga::ReceiveNotification(const std::string& key)
{
    if (key == kCoin)
    {
        mCoins++;
    }
    else if (key == kStart)
    {
        if (mCoins > 0 && mState == State::kWaiting)
        {
            mCoins--;
            mScore = 0;
            mLives = 3;
            mLevel = 1;
            mStateTimer = kStartupFrames;
            ResetWaveProgress();
            mState = State::kStartup;
        }
    }
    else if (key == kEnemyDied)
    {
        mEnemiesAlive--;
        mScore += Enemy::LastPointsAwarded();
    }
}

/**
 * @brief Creates and queues a player at the bottom center of the window.
 */
void Galaga::SpawnPlayer()
{
    auto player = std::make_shared<Player>(CMPUT350::Point2D(mWidth / 2.0f, mHeight - 100.0f));
    mContext->mEngineView->AddGameObject(player);
    mPlayer = player;
}

/**
 * @brief Removes active game objects and restores the starfield.
 */
void Galaga::ClearObjects()
{
    for (auto it = mContext->mEngineView->begin(); it != mContext->mEngineView->end(); ++it)
    {
        if (it->get() != this)
        {
            (*it)->Kill();
        }
    }
    mEnemiesAlive = 0;

    // add stars back in after clearing everything
    mContext->mEngineView->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, mWidth, mHeight)));
}

/**
 * @brief Resets the counters used to spawn the current level's waves.
 */
void Galaga::ResetWaveProgress()
{
    mWaveIndex = 0;
    mShipIndex = 0;
    mSpawnTimer = 0;
    mEnemiesAlive = 0;
}

/**
 * @brief Creates and queues the next enemy with its formation slot and entry path.
 */
void Galaga::SpawnNextEnemy()
{
    const int wave = mWaveIndex;
    const int ship = mShipIndex;
    const float slotX = mWidth * (static_cast<float>(ship) + 0.5f) / static_cast<float>(kShipsPerWave);
    const float slotY = 110.0f + static_cast<float>(wave) * 52.0f;
    const CMPUT350::Point2D formation(slotX, slotY);

    // Make the right-side approach the exact horizontal mirror of a left approach.
    const bool fromRight = ((wave + ship) % 2) != 0;
    const CMPUT350::Point2D leftTarget(fromRight ? mWidth - slotX : slotX, slotY);
    std::vector<CMPUT350::Point2D> path = {
        CMPUT350::Point2D(-60.0f, -50.0f),
        CMPUT350::Point2D(mWidth * 0.2f, -50.0f),
        CMPUT350::Point2D(leftTarget.x - 55.0f, leftTarget.y - 80.0f),
        leftTarget
    };
    if (fromRight)
    {
        for (auto& point : path)
        {
            point.x = mWidth - point.x;
        }
    }

    std::shared_ptr<Enemy> enemy;
    switch (ship % 3)
    {
    case 0:
        enemy = std::make_shared<Enemy1>(formation, path);
        break;
    case 1:
        enemy = std::make_shared<Enemy2>(formation, path);
        break;
    default:
        enemy = std::make_shared<Enemy3>(formation, path);
        break;
    }
    mContext->mEngineView->AddGameObject(enemy);
    ++mEnemiesAlive;

    ++mShipIndex;
    if (mShipIndex == kShipsPerWave)
    {
        mShipIndex = 0;
        ++mWaveIndex;
        if (mWaveIndex < kWaveCount)
        {
            mSpawnTimer = kFramesBetweenWaves;
        }
    }
    else
    {
        mSpawnTimer = 6 + ((wave + ship) % 3);
    }
}

/**
 * @brief Checks whether all living enemies have reached formation.
 * @return True when every living enemy is in formation (bool).
 */
bool Galaga::AreAllEnemiesInFormation() const
{
    int livingEnemiesSeen = 0;
    for (const auto& object : *mContext->mEngineView)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy == nullptr || !enemy->IsAlive())
        {
            continue;
        }
        ++livingEnemiesSeen;
        if (!enemy->IsInFormation())
        {
            return false;
        }
    }
    // Pending additions are not in the engine iterator until the next frame.
    return livingEnemiesSeen == mEnemiesAlive;
}

/**
 * @brief Checks whether all living enemies have finished attacking and returning.
 * @return True when no enemy is attacking or returning (bool).
 */
bool Galaga::AreAttackCyclesComplete() const
{
    for (const auto& object : *mContext->mEngineView)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy == nullptr || !enemy->IsAlive())
        {
            continue;
        }
        const Enemy::State enemyState = enemy->GetState();
        if (enemyState == Enemy::State::kAttacking || enemyState == Enemy::State::kReturning)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Selects and starts an attack for a deterministic formation enemy.
 */
void Galaga::SelectAttacker()
{
    const auto player = std::dynamic_pointer_cast<Player>(mPlayer.lock());
    if (player == nullptr)
    {
        return;
    }

    const int attackInterval = std::max(12, 36 - (mLevel - 1) * 2);
    if (mContext->currentFrame % attackInterval != 0)
    {
        return;
    }

    const int attackCap = std::min(4, 1 + (mLevel - 1) / 2);
    int activeAttackCycles = 0;
    std::shared_ptr<Enemy> candidate;
    float candidateDistance = 0.0f;
    CMPUT350::Point2D candidateLocation;
    const CMPUT350::Point2D playerLocation = player->GetLocation();

    for (const auto& object : *mContext->mEngineView)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy == nullptr || !enemy->IsAlive())
        {
            continue;
        }

        const Enemy::State enemyState = enemy->GetState();
        if (enemyState == Enemy::State::kAttacking || enemyState == Enemy::State::kReturning)
        {
            ++activeAttackCycles;
            continue;
        }
        if (!enemy->IsInFormation())
        {
            continue;
        }

        const CMPUT350::Point2D location = enemy->GetLocation();
        const float distance = std::abs(location.x - playerLocation.x);
        const bool isBetter = candidate == nullptr || distance < candidateDistance ||
            (distance == candidateDistance && (location.x < candidateLocation.x ||
             (location.x == candidateLocation.x && location.y < candidateLocation.y)));
        if (isBetter)
        {
            candidate = enemy;
            candidateDistance = distance;
            candidateLocation = location;
        }
    }

    if (activeAttackCycles < attackCap && candidate != nullptr)
    {
        candidate->Attack(mContext, playerLocation);
    }
}
