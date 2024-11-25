//
//  Scene.h
//  SDLProject
//
//  Created by Clara Azzi on 11/23/24.
//  Copyright © 2024 ctg. All rights reserved.
//
// Scene.h
#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "Map.h"
#include "ShaderProgram.h"
#include "Utility.h"
#include <SDL_mixer.h>

struct GameState {
    Entity* player;
    Entity* enemies;
    Map* map;
    Mix_Music* bgm;
    int lives;
    Mix_Chunk* jump_sfx;

    int next_scene_id = -1; // Tracks which scene to load next
};

class Scene {
protected:
    GameState m_game_state;

public:
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;

    // Add a getter for m_game_state
    GameState& get_game_state() {
        return m_game_state;
    }
};



#endif

