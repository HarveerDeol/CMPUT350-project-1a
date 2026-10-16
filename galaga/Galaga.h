#ifndef GALAGA_H
#define GALAGA_H
#include <memory>
#include <string>
#include "GameObject.h"
#include "GameContext.h"

class Galaga : public CMPUT350::GameObject
{
public:
    Galaga(float width, float height);
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;
    void SpawnPlayer();
    void ClearObjects();

private:
    enum class State { kWaiting, kStartup, kWaves, kPlaying, kRespawn, kGameOver };

    void SpawnNextEnemy();
    bool AreAllEnemiesInFormation() const;
    bool AreAttackCyclesComplete() const;
    void SelectAttacker();
    void ResetWaveProgress();

    State mState = State::kWaiting;
    int mScore = 0;
    int mLives = 3;
    int mLevel = 1;
    int mCoins = 0;
    int mStateTimer = 0;
    int mEnemiesAlive = 0;
    int mWaveIndex = 0;
    int mShipIndex = 0;
    int mSpawnTimer = 0;
    float mWidth, mHeight;
    CMPUT350::GameContext* mContext = nullptr;
    std::weak_ptr<CMPUT350::GameObject> mPlayer;
};

#endif // GALAGA_H
