#include "GameEngine.h"
#include "CollisionObject.h"
#include "GraphicsObject.h"
#include "FontData.h"
#include "CollisionObject.h"


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
      mDrawContext(mWindow, mFont),
      mGUIContext(mWindow, mFont) {
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
    mContext.mNotificationManager = &mNotificationManager;
    mContext.GUIContext = &mGUIContext;

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

void GameEngine::InstallKeyDownNotification(int scancode, const std::string& notification) {
    mKeyDownNotifications[scancode] = notification;
}

void GameEngine::InstallKeyUpNotification(int scancode, const std::string& notification) {
    mKeyUpNotifications[scancode] = notification;
}

GameEngine::const_iterator GameEngine::cbegin() const {
    return mGameObjects.cbegin();
}

GameEngine::const_iterator GameEngine::cend() const {
    return mGameObjects.cend();
}

GameEngine::const_iterator GameEngine::begin() const {
    return mGameObjects.cbegin();
}

GameEngine::const_iterator GameEngine::end() const {
    return mGameObjects.cend();
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
        mCurrentFrame++;
        mContext.currentFrame = mCurrentFrame;
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
            mContext.CurrObject = mPendingObjects[i];
            mPendingObjects[i]->Initialize(&mContext);
            i++;
        }
        mContext.CurrObject.reset();
        // clean up pending to add new objects later
        mPendingObjects.clear();

        // 2. Process events
        ProcessEvents();

        // 3. Update game objects
        for (auto& obj : mGameObjects) {
            mContext.CurrObject = obj;
            obj->Update(&mContext);
        }

        // 4. Process collision events
        ProcessCollisions();
        
/*      old collision processing
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
        } */

        // 5. Late updates
        for (auto& obj : mGameObjects) {
            mContext.CurrObject = obj;
            obj->LateUpdate(&mContext);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) {
                mContext.CurrObject = obj;
                mDrawContext.SetContextOffset(graphicsObj->GetLocation());
                mDrawContext.SetContextRotation(graphicsObj->GetRotation());
                graphicsObj->RenderBackground(&mContext);
            }
        }

        // 7. Render foreground
        for (auto& obj : mGameObjects) {
            auto graphicsObj = std::dynamic_pointer_cast<GraphicsObject>(obj);
            if (graphicsObj != nullptr) {
                mContext.CurrObject = obj;
                mDrawContext.SetContextOffset(graphicsObj->GetLocation());
                mDrawContext.SetContextRotation(graphicsObj->GetRotation());
                graphicsObj->RenderForeground(&mContext);
            }

        }

        // reset draw context
        mDrawContext.SetContextOffset(Point2D(0, 0));
        mDrawContext.SetContextRotation(0.0f);

        // 8. Render UI
        for (auto& obj : mGameObjects) {
            mContext.CurrObject = obj;
            obj->RenderUI(&mContext);
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
bool GameEngine::ProcessEvents()
{
	while (const std::optional event = mWindow->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			mWindow->close();
		}
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            const auto it = mKeyDownNotifications.find(static_cast<int>(keyPressed->scancode));
            if (it != mKeyDownNotifications.end())
            {
                mNotificationManager.Notify(it->second);
            }
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            const auto it = mKeyUpNotifications.find(static_cast<int>(keyReleased->scancode));
            if (it != mKeyUpNotifications.end())
            {
                mNotificationManager.Notify(it->second);
            }
        }
	}
	return mWindow->isOpen();
}

Shape GameEngine::TransformShape(const Shape& shape) {
    if (shape.t == ShapeType::kLine) {
        //transform both ends
        Line line = shape.shape.line;
        return Shape(Line(mDrawContext.Transform(line.p1),
                          mDrawContext.Transform(line.p2)));
    }
    else if (shape.t == ShapeType::kCircle) {
        //only the centre moves
        Circle circle = shape.shape.circle;
        return Shape(Circle(mDrawContext.Transform(circle.center), circle.radius));
    }
    else if (shape.t == ShapeType::kRect) {
        return Shape(mDrawContext.Transform(shape.shape.rect));
    }
    return shape;
}

void GameEngine::ProcessCollisions(){
    for (size_t i = 0; i < mGameObjects.size(); i++) {
        auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
        if (objA == nullptr) continue;

        for (size_t j = i + 1; j < mGameObjects.size(); j++) {
            auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
            if (objB == nullptr) continue;

            mDrawContext.SetContextOffset(objA->GetLocation());
            mDrawContext.SetContextRotation(objA->GetRotation());
            Rect overlap = mDrawContext.Transform(objA->GetBounds());
            mDrawContext.SetContextOffset(objB->GetLocation());
            mDrawContext.SetContextRotation(objB->GetRotation());
            overlap &= mDrawContext.Transform(objB->GetBounds());

            
            if (overlap.width > 0 && overlap.height > 0) {
                mDrawContext.SetContextOffset(objA->GetLocation());
                mDrawContext.SetContextRotation(objA->GetRotation());
                std::vector<Shape> shapesA;
                for (const Shape &s : objA->GetShapes()) {
                    shapesA.push_back(TransformShape(s));
                }


                mDrawContext.SetContextOffset(objB->GetLocation());
                mDrawContext.SetContextRotation(objB->GetRotation());
                std::vector<Shape> shapesB;
                for (const Shape &s : objB->GetShapes()) {
                    shapesB.push_back(TransformShape(s));
                }

                Point2D contactPoint;
                bool hasPoint = false;
                bool hit = false;
                for (size_t a = 0; a < shapesA.size() && !hit; a++) {
                    for (size_t b = 0; b < shapesB.size() && !hit; b++) {
                        if (Intersects(shapesA[a], shapesB[b], contactPoint, hasPoint)) {
                            hit = true;
                        }
                    }
                }
                if (hit) {
                    if (hasPoint) {
                        //the same world point, expressed in each object's own local space
                        mDrawContext.SetContextOffset(objA->GetLocation());
                        mDrawContext.SetContextRotation(objA->GetRotation());
                        Point2D localA = mDrawContext.ReverseTransform(contactPoint);

                        mDrawContext.SetContextOffset(objB->GetLocation());
                        mDrawContext.SetContextRotation(objB->GetRotation());
                        Point2D localB = mDrawContext.ReverseTransform(contactPoint);

                        objA->CollisionEnter(objB, localA);
                        objB->CollisionEnter(objA, localB);
                    }
                    else {
                        objA->CollisionEnter(objB);
                        objB->CollisionEnter(objA);
                    }
                }
            }
        }
    }
    // reset draw context
    mDrawContext.SetContextOffset(Point2D(0, 0));
    mDrawContext.SetContextRotation(0.0f);
}

}  // namespace CMPUT350
