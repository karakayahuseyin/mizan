#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <imgui.h>
#include <string>

/**
 * FontManager handles loading and managing fonts for the ImGui interface
 * including icon fonts like FontAwesome.
 */
class FontManager {
public:
    static FontManager& getInstance();
    
    /**
     * Initialize fonts - call this after ImGui context is created
     * but before the first frame is rendered
     */
    bool initialize();
    
    /**
     * Load FontAwesome icon font
     * @param fontPath Path to the FontAwesome font file
     * @param fontSize Size of the icon font
     * @return true if successful
     */
    bool loadIconFont(const std::string& fontPath, float fontSize = 16.0f);
    
    /**
     * Load a regular font
     * @param fontPath Path to the font file
     * @param fontSize Size of the font
     * @return ImFont* pointer or nullptr if failed
     */
    ImFont* loadFont(const std::string& fontPath, float fontSize = 16.0f);
    
    /**
     * Get the default font
     */
    ImFont* getDefaultFont() const { return m_defaultFont; }
    
    /**
     * Get the icon font
     */
    ImFont* getIconFont() const { return m_iconFont; }
    
    /**
     * Check if icon font is loaded
     */
    bool hasIconFont() const { return m_iconFont != nullptr; }
    
    /**
     * Push icon font for rendering
     */
    void pushIconFont();
    
    /**
     * Pop icon font after rendering
     */
    void popIconFont();
    
    /**
     * Cleanup resources
     */
    void cleanup();

private:
    FontManager() = default;
    ~FontManager() = default;
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    ImFont* m_defaultFont = nullptr;
    ImFont* m_iconFont = nullptr;
    bool m_initialized = false;
};

#endif // FONTMANAGER_H
