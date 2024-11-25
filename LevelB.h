#ifndef LEVELB_H
#define LEVELB_H

#include "Scene.h"

class LevelB : public Scene {
public:
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};

#endif

