#pragma once

#include "nuklear/nuklear.h"

class INKWindow {
public:
        virtual void draw(nk_context* ctx) = 0;
};
