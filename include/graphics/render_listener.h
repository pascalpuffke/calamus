#pragma once

namespace calamus {

class RenderListener {
public:
    virtual void on_frame() = 0;
};

}
