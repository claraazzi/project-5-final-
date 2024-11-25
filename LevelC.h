
//
//  LevelC.h
//  SDLProject
//
//  Created by Clara Azzi on 11/23/24.
//  Copyright © 2024 ctg. All rights reserved.
//

#ifndef LEVELC_H
#define LEVELC_H

#include "Scene.h"

class LevelC : public Scene {
public:
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};

#endif
