#include "GameEngine.h"
#include "CollisionObject.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    // Sample font loading code
    //	if (!mFont->openFromMemory(&_font, _font_len))
    //	{
    //		fprintf(stderr, "WARNING: Font did not load.\n");
    //	}
}

GameEngine::~GameEngine() {
    // Cleanup resources
    // mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    while (true)  // window is open
    {
        // 0. Remove any objects that are now dead

        // 1. Activate and initialize any objects added during the last frame

        // 2. Process events

        // 3. Update game objects
        for (auto& obj : mGameObjects) {
            obj->Update(&mContext);
        }

        // 4. Process collision events
        for (size_t i = 0; i < mGameObjects.size(); i++) {
            auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
            if (objA == nullptr) continue; // not a collision object, skip

            for (size_t j = i + 1; j < mGameObjects.size(); j++) { // j starts at i+1: avoids self-checks and duplicate pairs
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                if (objB == nullptr) continue;

                Rect overlap = objA->GetBounds();
                overlap &= objB->GetBounds(); // shrink to intersection region
                if (overlap.width > 0 && overlap.height > 0) { // positive area means they actually overlap
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

        // 6. Render background
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) {
                graphicsObj->RenderBackground(&mContext);
            }
        }

        // 7. Render foreground
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) {
                graphicsObj->RenderForeground(&mContext);
            }
        }

        // Actually render to window
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350
