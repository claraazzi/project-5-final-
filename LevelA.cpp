#include "LevelA.h"
#include "Utility.h"
#include "Map.h"
#include <string> // For std::to_string

extern int g_lives; // Declare the global lives variable

unsigned int LEVEL_1_DATA[] = {
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
    0, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 0, 1,
    1, 2, 1, 1, 3, 3, 0, 0, 2, 1, 1, 3, 2, 1, 0,
    2, 0, 1, 1, 3, 2, 1, 1, 2, 0, 0, 3, 3, 2, 1,
    3, 2, 2, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 0,
};

constexpr char JUMP_SFX_FILEPATH[] = "assets/audio/jump.wav";

void LevelA::initialise() {
    m_game_state.next_scene_id = -1;

    // Load the map with the same tileset and data
    GLuint map_texture_id = Utility::load_texture("assets/images/Tile_10.png");
    m_game_state.map = new Map(14, 5, LEVEL_1_DATA, map_texture_id, 1.0f, 2, 2);

    // Initialize player
    GLuint player_texture_id = Utility::load_texture("assets/images/player.png");
    int player_walking_animation[4][4] = {
        { 1, 5, 9, 13 }, // DOWN
        { 3, 7, 11, 15 }, // UP
        { 2, 6, 10, 14 }, // LEFT
        { 0, 4, 8, 12 }   // RIGHT
    };
    glm::vec3 player_acceleration = glm::vec3(0.0f, -4.905f, 0.0f);
    m_game_state.player = new Entity(
        player_texture_id,          // Texture ID
        4.0f,                       // Speed
        player_acceleration,        // Acceleration
        6.0f,                       // Jump Power
        player_walking_animation,   // Walking animation frames
        0.0f,                       // Animation Time
        4,                          // Animation Frames
        0,                          // Animation Index
        4,                          // Animation Cols
        4,                          // Animation Rows
        0.5f, 0.5f,                 // Width, Height
        PLAYER                      // Entity Type
    );
    m_game_state.player->set_position(glm::vec3(1.0f, -2.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));

    // Initialize one enemy
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy.png");
    m_game_state.enemies = new Entity[1];
    m_game_state.enemies[0] = Entity(
        enemy_texture_id,          // Texture ID
        1.0f,                      // Speed
        0.7f, 0.7f,                // Width, Height
        ENEMY,                     // Entity Type
        SPINNER,                   // AI Type
        IDLE                       // AI State
    );
    m_game_state.enemies[0].set_position(glm::vec3(5.0f, -2.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(1.0f, 1.0f, 0.0f));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.bgm = Mix_LoadMUS("assets/audio/galaxyloop.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(50.0f);
    m_game_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
}

void LevelA::update(float delta_time) {
    m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

    // Update the single enemy
    if (m_game_state.enemies[0].is_active()) {
        m_game_state.enemies[0].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

        // Check collision with the enemy
        if (m_game_state.player->check_collision(&m_game_state.enemies[0])) {
            if (m_game_state.player->get_velocity().y < 0) {
                m_game_state.enemies[0].deactivate(); // Defeat the enemy
                m_game_state.next_scene_id = 2;       // Transition to LevelB
            } else {
                // Collision from the side (lose condition)
                g_lives--; // Decrement lives
                if (g_lives > 0) {
                    initialise(); // Restart the level if lives remain
                } else {
                    m_game_state.next_scene_id = -1; // Trigger "Game Over"
                }
            }
        }
    }
}

void LevelA::render(ShaderProgram* program) {
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    m_game_state.enemies[0].render(program);
    
    //Display remaining lives
    Utility::draw_text(program, Utility::load_texture("assets/fonts/font1.png"),
                       "Lives: " + std::to_string(g_lives), 0.5f, 0.1f,
                       glm::vec3(-3.5f, 3.0f, 0.0f));
    
    // Lose condition: Game over
    if (g_lives <= 0) {
        glm::vec3 player_position = m_game_state.player->get_position();
        Utility::draw_text(program, Utility::load_texture("assets/fonts/font1.png"), "YOU LOSE", 0.6f, 0.1f,
                           glm::vec3(player_position.x - 2.5f, player_position.y + 0.5f, 0.0f));
        SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
        SDL_Delay(3000); // Pause for 3 seconds
        exit(0);
    }
    
    // Win condition: Enemy is defeated
    if (!m_game_state.enemies[0].is_active()) {
        glm::vec3 player_position = m_game_state.player->get_position();
        Utility::draw_text(program, Utility::load_texture("assets/fonts/font1.png"),
                           "NEXT LEVEL", 0.5f, 0.1f, // Adjusted size and spacing
                           glm::vec3(player_position.x - 2.5f, player_position.y + 0.5f, 0.0f));
        SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
        SDL_Delay(3000); // Pause for 3 seconds before transitioning
        m_game_state.next_scene_id = 2; // Transition to LevelB
    }
}













