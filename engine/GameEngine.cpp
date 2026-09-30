#include "GameEngine.h"
#include "CollisionObject.h"
#include "GraphicsObject.h"
#include "FontData.h"


namespace CMPUT350 {
/**
 * @brief Creates the game window and loads the resources shared by all drawing.
 * @param width Window width in pixels (unsigned int).
 * @param height Window height in pixels (unsigned int).
 * @param name Text shown in the window's title bar (const std::string&).
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name)),
      mFont(std::make_shared<sf::Font>()),
      mDrawContext(mWindow, mFont) {
    // Creates the game window capped at 30 frames per second, loads
    // the font embedded in FontData.h, and builds the DrawContext and
    // GameContext.

    // set framerate
    mWindow->setFramerateLimit(30);
    
    // Font loading code
    if (!mFont->openFromMemory(&_font, _font_len))
    {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    // set up GameContext
    mContext.mEngineView = this;
    mContext.ScreenContext = &mDrawContext;

}

/// @brief Closes the window as a failsafe, in case the engine is destroyed while it is still open. @return Nothing (void).
GameEngine::~GameEngine() {
    // Cleanup resources
    mWindow->close();
}

/**
 * @brief Queues an object to join the game at the start of the next frame.
 * @param gameObject The object to add (std::shared_ptr<GameObject>); the engine takes shared ownership.
 * @return Nothing (void).
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    // add to pending objects to be added at the start of the next frame
    mPendingObjects.push_back(gameObject);
}

/**
 * @brief Runs the game loop until the window closes.
 * @return Nothing (void).
 *
 * Each frame removes dead objects, admits objects queued last frame and
 * initializes them, processes input, updates every object, dispatches
 * collisions, runs late updates, then draws background and foreground.
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
            mPendingObjects[i]->Initialize(&mContext);
            i++;
        }
        // clean up pending to add new objects later
        mPendingObjects.clear();

        // 2. Process events
        ProcessEvents(&mContext);

        // 3. Update game objects
        for (auto& obj : mGameObjects) {
            obj->Update(&mContext);
        }

        // 4. Process collision events
        for (size_t i = 0; i < mGameObjects.size(); i++) {
            auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
            if (objA == nullptr) continue;

            for (size_t j = i + 1; j < mGameObjects.size(); j++) {
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                if (objB == nullptr) continue;

                Rect overlap = objA->GetBounds();
                overlap &= objB->GetBounds();
                if (overlap.width > 0 && overlap.height > 0) {
                    objA->CollisionEnter(objB);
                    objB->CollisionEnter(objA);
                }
            }
        }

        // 5. Late updates
        for (auto& obj : mGameObjects) {
            obj->LateUpdate(&mContext);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) graphicsObj->RenderBackground(&mContext);
        }

        // 7. Render foreground
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) graphicsObj->RenderForeground(&mContext);

        }

        // Actually render to window
        mWindow->display();
    }
}

/**
 * @brief Drains this frame's SFML event queue.
 * @param context The context passed to any object receiving a key event (GameContext*).
 * @return True if the window is still open after processing (bool).
 */
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
