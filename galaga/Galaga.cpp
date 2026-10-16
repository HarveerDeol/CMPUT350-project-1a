#include "Galaga.h"
#include "Player.h"
#include "Stars.h"
#include "NotificationManager.h"

#include <SFML/Window/Keyboard.hpp>

namespace {
constexpr char kCoin[] = "game.coin";
constexpr char kStart[] = "game.start";
constexpr char kEnemyDied[] = "enemy.died";

constexpr int kStartupFrames = 60;    // two seconds showing the level number
constexpr int kGameOverFrames = 120;  // four seconds showing game over
}

Galaga::Galaga(float width, float height) : mWidth(width), mHeight(height)
{
}

void Galaga::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;

    context->mNotificationManager->Register(context->CurrObject, kCoin);
    context->mNotificationManager->Register(context->CurrObject, kStart);
    context->mNotificationManager->Register(context->CurrObject, kEnemyDied);

    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::Num5), kCoin);
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::Num1), kStart);

    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::A), "player.move-left");
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::D), "player.move-right");
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::Space), "player.fire");
    context->mEngineView->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, mWidth, mHeight)));
}
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
            mState = State::kWaves;
        }
    }
    else if (mState == State::kWaves)
    {
        // TODO: #47 spawn enemies in waves
        mState = State::kPlaying;
    }
    else if (mState == State::kPlaying){
        if (mPlayer.expired()) { // player has been killed, check if they have lives left
            mLives--;
            if (mLives > 0) {
                // TODO: #47 wait for all enemies to return to formation, then show the remaining lives before respawning
                SpawnPlayer();
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
    else if (mState == State::kGameOver)
    {
        gui->DrawCenteredText("GAME OVER", 48, center, CMPUT350::Colors::white);
    }
}

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
            mState = State::kStartup;
        }
    }
    else if (key == kEnemyDied)
    {
        mEnemiesAlive--;
        mScore += 100;
    }
}

void Galaga::SpawnPlayer()
{
    auto player = std::make_shared<Player>(CMPUT350::Point2D(mWidth / 2.0f, mHeight - 100.0f));
    mContext->mEngineView->AddGameObject(player);
    mPlayer = player;
}

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
