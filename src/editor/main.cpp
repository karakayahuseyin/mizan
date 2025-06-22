/**
 * Copyright (c) 2025 Hüseyin Karakaya
 * This file is part of the Mizan project. Licensed under the MIT License (MIT).
 */

#include "Application.h"
#include <iostream>

int main() {
    Application app;
    
    if (!app.init()) {
        std::cerr << "Failed to initialize application\n";
        return -1;
    }

    app.run();
    
    return 0;
}
