#ifndef WORKBENCH_H
#define WORKBENCH_H

#include "Toolkit.h"
#include "Viewport.h"

#include <memory>

class Workbench {
public:
    Workbench();
    ~Workbench();

private:
    std::unique_ptr<Toolkit> m_toolkit;
    std::unique_ptr<Viewport> m_viewport;

    void initialize();
    void cleanup();
};

#endif // WORKBENCH_H