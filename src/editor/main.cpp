/**
 * Copyright (c) 2025 Hüseyin Karakaya
 * This file is part of the Mizan project. Licensed under the MIT License (MIT).
 */

#include "Application.h"
#include "logger/Logger.h"

int main() {
    // Initialize logging
    Logger::info("Starting Mizan Editor...");
    
    // Optionally enable file logging
    if (Logger::enableFileLogging("/tmp/mizan_editor.log")) {
        Logger::info("File logging enabled");
    } else {
        Logger::warning("Could not enable file logging, continuing with console only");
    }
    
    Application app;
    
    if (!app.init()) {
        Logger::error("Failed to initialize application");
        return -1;
    }

    Logger::info("Application initialized successfully, starting main loop");
    app.run();
    
    Logger::info("Application shutting down");
    Logger::disableFileLogging();
    
    return 0;
}
