//
//  Menu.cpp
//  SDLProject
//
//  Created by Clara Azzi on 11/23/24.
//  Copyright © 2024 ctg. All rights reserved.
//

#include "Menu.h"
#include "Utility.h" // Assuming Utility.h has `draw_text` function
#include <SDL.h>


void Menu::initialise() {
    // Menu doesn't have a complex setup right now
}

void Menu::process_input(bool& in_menu) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            exit(0); // Quit the application
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            in_menu = false; // Exit the menu
        }
    }
}


void Menu::render(ShaderProgram* program, GLuint font_texture_id) {
    // Render the menu screen
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw menu text
    Utility::draw_text(program, font_texture_id, "Welcome to the Game!", 0.75f, -0.3f, glm::vec3(-4.0f, 1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press ENTER to Start", 0.5f, -0.25f, glm::vec3(-4.0f, -1.0f, 0.0f));
}
