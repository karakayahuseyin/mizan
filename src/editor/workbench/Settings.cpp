#include "Settings.h"
#include <imgui.h>

Settings& Settings::getInstance() {
    static Settings instance;
    return instance;
}

void Settings::renderSettingsWindow() {
    if (!m_showSettingsWindow) {
        return;
    }
    
    if (ImGui::Begin("Settings", &m_showSettingsWindow)) {
        // Grid settings section
        if (ImGui::CollapsingHeader("Grid Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show Grid", &m_gridEnabled);
            
            float gridColor[3] = { m_gridColor.x, m_gridColor.y, m_gridColor.z };
            if (ImGui::ColorEdit3("Grid Color", gridColor)) {
                m_gridColor = glm::vec3(gridColor[0], gridColor[1], gridColor[2]);
            }
            
            if (ImGui::SliderFloat("Grid Size", &m_gridSize, 5.0f, 50.0f, "%.1f")) {
                // Grid size changed, need to regenerate grid
            }
            
            if (ImGui::SliderFloat("Grid Spacing", &m_gridSpacing, 0.1f, 2.0f, "%.2f")) {
                // Grid spacing changed, need to regenerate grid
            }
        }
        
        // Background settings section
        if (ImGui::CollapsingHeader("Background Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            float bgColor[3] = { m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z };
            if (ImGui::ColorEdit3("Background Color", bgColor)) {
                m_backgroundColor = glm::vec3(bgColor[0], bgColor[1], bgColor[2]);
            }
        }
    }
    ImGui::End();
}
