#pragma once

#include "nuklear/nuklear.h"

class INKControl {
public:
        virtual void draw(nk_context* ctx) = 0;
};
