#include <memory>

#include "GameEngine.h"
#include "Galaga.h"

/**
 * @brief Creates the engine and hands control to the Galaga game object.
 * @return 0 on normal exit (int).
 */
int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    engine.AddGameObject(std::make_shared<Galaga>(768.0f, 1024.0f));
    engine.Run();
    return 0;
}