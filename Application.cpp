#include "Engine.h"
#include "test/Test.h"


int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(720, 720);
    
    sre::TestManager::get()->startTests();

    // Starts the game loop
    Engine::start();

    return 0;
}