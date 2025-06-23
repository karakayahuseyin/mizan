#ifndef SETTINGS_H
#define SETTINGS_H

#include <glm/glm.hpp>

/**
 * Settings class manages application-wide configuration options
 * including visual settings like grid and background colors.
 */
class Settings {
public:
    static Settings& getInstance();
    
    // Grid settings
    bool isGridEnabled() const { return m_gridEnabled; }
    void setGridEnabled(bool enabled) { m_gridEnabled = enabled; }
    
    glm::vec3 getGridColor() const { return m_gridColor; }
    void setGridColor(const glm::vec3& color) { m_gridColor = color; }
    
    float getGridSize() const { return m_gridSize; }
    void setGridSize(float size) { m_gridSize = size; }
    
    float getGridSpacing() const { return m_gridSpacing; }
    void setGridSpacing(float spacing) { m_gridSpacing = spacing; }
    
    // Background settings
    glm::vec3 getBackgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const glm::vec3& color) { m_backgroundColor = color; }
    
    // Settings UI
    void renderSettingsWindow();
    bool isSettingsWindowOpen() const { return m_showSettingsWindow; }
    void setSettingsWindowOpen(bool open) { m_showSettingsWindow = open; }

private:
    Settings() = default;
    ~Settings() = default;
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    
    // Grid settings
    bool m_gridEnabled = true;
    glm::vec3 m_gridColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
    float m_gridSize = 20.0f; // Grid extends from -10 to +10
    float m_gridSpacing = 0.5f; // Distance between grid lines
    
    // Background settings
    glm::vec3 m_backgroundColor = glm::vec3(0.2f, 0.2f, 0.2f); // Dark gray
    
    // UI state
    bool m_showSettingsWindow = false;
};

#endif // SETTINGS_H
