//
//  Utility.h
//  SDLProject
//
//  Created by Clara Azzi on 11/23/24.
//  Copyright © 2024 ctg. All rights reserved.
//

#ifndef UTILITY_H
#define UTILITY_H

#include "ShaderProgram.h"
#include <string>

class Utility {
public:
    // Load texture from file
    static GLuint load_texture(const char* filepath);

    // Draw text using a font texture
    static void draw_text(ShaderProgram* program, GLuint font_texture_id, const std::string& text,
                          float font_size, float spacing, glm::vec3 position);
};

#endif // UTILITY_H
