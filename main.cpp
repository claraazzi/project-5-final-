/**
* Author: [Clara EL Azzi]
* Assignment: Rise of the AI
* Date due: 2024-11-9, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Scene.h"

Scene* g_current_scene = nullptr; // Pointer to the current scene
Menu g_menu;
bool g_in_menu = true; // Tracks if we are in the menu screen

#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <cmath>
#include <vector>

// ————— VARIABLES ————— //
SDL_Window* g_display_window;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;
float g_previous_ticks = 0.0f, g_accumulator = 0.0f;
GLuint g_font_texture_id;

enum AppStatus { RUNNING, TERMINATED };
AppStatus g_app_status = RUNNING;

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;
constexpr char GAME_WINDOW_NAME[] = "Rise of the AI!";
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr char FONT_FILEPATH[] = "assets/fonts/font1.png";
int g_lives = 3; // Global variable to track lives


LevelA levelA; // Instance of LevelA
LevelB levelB; // Instance of LevelB
LevelC levelC; // Instance of LevelC

void initialise();
void process_input();
void update();
void render();
void shutdown();

void initialise() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // Initialize view and projection matrices
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(0.1922f, 0.549f, 0.9059f, 1.0f);

    // Initialize audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    // Initialize menu and font texture
    g_menu.initialise();
    g_font_texture_id = Utility::load_texture(FONT_FILEPATH);

    if (!g_font_texture_id) {
        std::cerr << "Failed to load font texture from " << FONT_FILEPATH << std::endl;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the initial scene to LevelA
    g_current_scene = &levelA;
    g_current_scene->initialise();
}

void process_input() {
    if (g_in_menu) {
        // Pass the menu's `process_input` function the `g_in_menu` flag
        g_menu.process_input(g_in_menu);
        return; // Skip further input processing while in the menu
    }

    // Reset movement
    g_current_scene->get_game_state().player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_SPACE:
                        // Ensure the player is on the ground before allowing a jump
                        if (g_current_scene->get_game_state().player->get_collided_bottom()) {
                            g_current_scene->get_game_state().player->jump();
                            Mix_PlayChannel(-1, g_current_scene->get_game_state().jump_sfx, 0); // Play jump sound
                        }
                        break;
                }
                break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_LEFT]) {
        g_current_scene->get_game_state().player->move_left();
    } else if (key_state[SDL_SCANCODE_RIGHT]) {
        g_current_scene->get_game_state().player->move_right();
    }

    if (glm::length(g_current_scene->get_game_state().player->get_movement()) > 1.0f) {
        g_current_scene->get_game_state().player->normalise_movement();
    }
}

void update() {
    if (g_in_menu) return;

    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;
    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);

        // Check if the player has lost a life
        if (!g_current_scene->get_game_state().player->is_active()) {
            g_lives--; // Decrement lives

            if (g_lives > 0) {
                // Restart the current scene if lives remain
                g_current_scene->initialise();
            } else {
                // All lives lost: Game Over
                Utility::draw_text(&g_shader_program, g_font_texture_id, "YOU LOSE", 1.0f, 0.1f,
                                   glm::vec3(-3.0f, 0.0f, 0.0f));
                SDL_GL_SwapWindow(g_display_window);
                SDL_Delay(3000); // Pause for 3 seconds
                shutdown();
                exit(0); // Terminate the program
            }
        }

        // Check for scene transition
        if (g_current_scene->get_game_state().next_scene_id == 2) {
            g_current_scene = &levelB; // Transition to LevelB
            g_current_scene->initialise();
        } else if (g_current_scene->get_game_state().next_scene_id == 3) {
            g_current_scene = &levelC; // Transition to LevelC
            g_current_scene->initialise();
        }

        delta_time -= FIXED_TIMESTEP;
    }
    g_accumulator = delta_time;

    g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-g_current_scene->get_game_state().player->get_position().x, 0.0f, 0.0f));
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (g_in_menu) {
        // Reset the view and projection matrices for the menu
        g_view_matrix = glm::mat4(1.0f);
        g_shader_program.set_view_matrix(g_view_matrix);

        g_menu.render(&g_shader_program, g_font_texture_id);
        SDL_GL_SwapWindow(g_display_window);
        return;
    }

    // Render the game scene
    g_shader_program.set_view_matrix(g_view_matrix);
    g_current_scene->render(&g_shader_program);

    // Render lives count
   // Utility::draw_text(&g_shader_program, g_font_texture_id,
                      // "Lives: " + std::to_string(g_lives), 0.5f, 0.1f,
                      // glm::vec3(-4.5f, 3.0f, 0.0f)); // Position text as needed

    SDL_GL_SwapWindow(g_display_window);
}



void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialise();

    while (g_app_status == RUNNING) {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}






