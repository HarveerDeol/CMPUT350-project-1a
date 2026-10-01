#include "GameContext.h"
#include "DrawContext.h"
#include "Stars.h"

/**
 * @brief Constructs a starfield of randomly placed stars within a bounding region.
 * @param numStars Number of stars to generate (int).
 * @param bounds Rectangular region the stars are scattered within and scroll inside of (CMPUT350::Rect).
 */
Stars::Stars(int numStars, CMPUT350::Rect bounds)
    : mBounds(bounds), gen(rd())
{
    std::uniform_int_distribution<int> mXRand(bounds.topLeft.x, bounds.topLeft.x + bounds.width);
    std::uniform_int_distribution<int> mYRand(bounds.topLeft.y, bounds.topLeft.y + bounds.height);
    // Generate random star positions within the bounds
    for (int x = 0; x < numStars; x++)
    {
        mStarPositions.emplace_back(mXRand(gen), mYRand(gen));
    }
}

/**
 * @brief Draws a scrolling, twinkling starfield behind the rest of the scene.
 * @param context Provides the draw context used to render (CMPUT350::GameContext*).
 * @return Nothing (void).
 */
void Stars::RenderBackground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor c[4] = {
        CMPUT350::Colors::white, CMPUT350::Colors::cyan, CMPUT350::Colors::magenta, CMPUT350::Colors::white
    };
    static int skip = 0;
    int curr = 0;
    skip++;
    for (auto& star : mStarPositions)
    {
        curr++;
        if ((curr + skip / 5) % (mStarPositions.size() / 20) != 0)
            context->ScreenContext->DrawCircle(star, 1.0f, c[(curr) % 4]);
        star.y += 3;
        if (star.y > mBounds.topLeft.y + mBounds.height)
            star.y -= mBounds.height;
    }
}
