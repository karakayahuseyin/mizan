#include "Application.h"
#include <iostream>

int main() {
    Application app;
    
    if (!app.init()) {
        std::cerr << "Failed to initialize application\n";
        return -1;
    }

    // Create test objects after initialization
    app.addTestObjects();

    app.run();
    
    return 0;
}
