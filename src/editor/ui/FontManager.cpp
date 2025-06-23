#include "FontManager.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

// Define this to use fallback icons if FontAwesome is not available
#ifndef USE_FALLBACK_ICONS
#define USE_FALLBACK_ICONS
#endif

FontManager& FontManager::getInstance() {
    static FontManager instance;
    return instance;
}

bool FontManager::initialize() {
    if (m_initialized) {
        return true;
    }
    
    ImGuiIO& io = ImGui::GetIO();
    
    // Load default font first
    m_defaultFont = io.Fonts->AddFontDefault();
    if (!m_defaultFont) {
        std::cerr << "Failed to load default font" << std::endl;
        return false;
    }
    
    // Try to load FontAwesome from common locations
    std::vector<std::string> fontPaths = {
        "assets/fonts/fa-solid-900.ttf",
        "fonts/fa-solid-900.ttf",
        "../fonts/fa-solid-900.ttf",
        "lib/fonts/fa-solid-900.ttf",
        "/usr/share/fonts/truetype/font-awesome/fa-solid-900.ttf"
    };
    
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (std::filesystem::exists(path)) {
            std::cout << "Attempting to load FontAwesome from: " << path << std::endl;
            if (loadIconFont(path, 16.0f)) {
                std::cout << "Successfully loaded FontAwesome from: " << path << std::endl;
                fontLoaded = true;
                break;
            }
        }
    }
    
    // If no FontAwesome found, create a merged font with the default font for fallback
    if (!fontLoaded) {
        std::cout << "FontAwesome not found, using default font for icons" << std::endl;
        m_iconFont = m_defaultFont; // Use default font as fallback
    }
    
    // Build font atlas
    if (!io.Fonts->Build()) {
        std::cerr << "Failed to build font atlas" << std::endl;
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool FontManager::loadIconFont(const std::string& fontPath, float fontSize) {
    ImGuiIO& io = ImGui::GetIO();
    
    // FontAwesome 6 icon ranges (broader range to cover all icons)
    static const ImWchar icons_ranges[] = { 0xf000, 0xf8ff, 0 }; // Extended FontAwesome range
    
    ImFontConfig config;
    config.MergeMode = true;  // Merge with default font
    config.GlyphMinAdvanceX = fontSize;
    config.OversampleH = 3; // Better quality
    config.OversampleV = 1;
    config.PixelSnapH = true;
    
    // Merge FontAwesome icons into the default font
    m_iconFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, &config, icons_ranges);
    
    if (m_iconFont) {
        std::cout << "Successfully loaded FontAwesome with " << (0xf8ff - 0xf000 + 1) << " possible glyphs" << std::endl;
        // Set the merged font as the default font
        m_defaultFont = m_iconFont;
    } else {
        std::cerr << "Failed to load FontAwesome from: " << fontPath << std::endl;
    }
    
    return m_iconFont != nullptr;
}

ImFont* FontManager::loadFont(const std::string& fontPath, float fontSize) {
    ImGuiIO& io = ImGui::GetIO();
    return io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
}

void FontManager::pushIconFont() {
    if (m_iconFont) {
        ImGui::PushFont(m_iconFont);
    }
}

void FontManager::popIconFont() {
    if (m_iconFont) {
        ImGui::PopFont();
    }
}

void FontManager::cleanup() {
    m_defaultFont = nullptr;
    m_iconFont = nullptr;
    m_initialized = false;
}
