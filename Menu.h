//
//  Menu.h
//  SDLProject
//
//  Created by Clara Azzi on 11/23/24.
//  Copyright © 2024 ctg. All rights reserved.
//

#ifndef MENU_H
#define MENU_H

#include "ShaderProgram.h"
#include <string>

class Menu {
public:
    void initialise();                      // Setup menu state
    void process_input(bool& start_game);   // Process user input to start the game
    void render(ShaderProgram* program, GLuint font_texture_id); // Render menu text
};

#endif
