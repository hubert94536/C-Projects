#include "Vampire.h"
#include "Player.h"
#include "Arena.h"
#include "Game.h"
#include "History.h"
#include "globals.h"



int main()
{
    // Create a game
    // Use this instead to create a mini-game:   Game g(3, 5, 2);
    Game g(3, 5, 2);

    // Play the game
    g.play();
}

