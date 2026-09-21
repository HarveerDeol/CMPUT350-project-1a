#include "GameEngine.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"
// Creates the game window capped at 30 frames per second, loads
// the font embedded in FontData.h, and builds the DrawContext and
// GameContext.
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name)),
      mFont(std::make_shared<sf::Font>()),
      mDrawContext(mWindow, mFont),
      mContext(std::make_shared<GameContext>()) {

    // set framerate
    mWindow->setFramerateLimit(30);
    
    // Font loading code
    if (!mFont->openFromMemory(&_font, _font_len))
    {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    // set up GameContext
    mContext->mEngineView = this;
    mContext->ScreenContext = &mDrawContext;

}

GameEngine::~GameEngine() {
    // Cleanup resources
    mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    // add to pending objects to be added at the start of the next frame
    mPendingObjects.push_back(gameObject);
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        for (size_t i=0; i < mGameObjects.size();){
            if (mGameObjects[i]->IsAlive()){
                i++;
            }
            else{ // dead
                // swap with last element, pop out to avoid O(n^2) time (which would be repeatedly shifting objects)
                std::swap(mGameObjects[i],mGameObjects.back());
                mGameObjects.pop_back();
                // doesn't increment because we need to check the object it got swapped with
            }
        }

        // 1. Activate and initialize any objects added during the last frame
        for (size_t i=0; i < mPendingObjects.size();){
            mGameObjects.push_back(mPendingObjects[i]);
            mPendingObjects[i]->Initialize(mContext.get());
            i++;
        }
        // clean up pending to add new objects later
        mPendingObjects.clear();

        // 2. Process events
        ProcessEvents(mContext.get());

        // 3. Update game objects

        // 4. Process collision events

        // 5. Late updates

        // Clear window
        mWindow->clear();

        // 6. Render background

        // 7. Render foreground

        // Actually render to window
        mWindow->display();
    }
}

bool GameEngine::ProcessEvents(GameContext *context)
{
	while (const std::optional event = mWindow->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			mWindow->close();
		}
		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
		{
			if (keyPressed->unicode < 128)
			{
				char key = static_cast<char>(keyPressed->unicode);
				for (auto& obj : mGameObjects)
				{
					obj->HandleKeyEvent(context, key);
				}
			}
		}
	}
	return mWindow->isOpen();
}

}  // namespace CMPUT350
