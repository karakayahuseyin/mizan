#include "IconUtils.h"
#include "FontManager.h"
#include <sstream>

namespace IconUtils {

bool IconButton(const char* icon, const char* text, const ImVec2& size) {
    // Create button text with icon and text
    std::stringstream ss;
    ss << icon << " " << text;
    std::string buttonText = ss.str();
    
    return ImGui::Button(buttonText.c_str(), size);
}

bool IconOnlyButton(const char* icon, const char* tooltip, const ImVec2& size) {
    bool clicked = ImGui::Button(icon, size);
    
    if (tooltip && ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
    
    return clicked;
}

bool IconMenuItem(const char* icon, const char* text, const char* shortcut, bool selected, bool enabled) {
    std::stringstream ss;
    ss << icon << " " << text;
    std::string menuText = ss.str();
    
    return ImGui::MenuItem(menuText.c_str(), shortcut, selected, enabled);
}

bool IconSelectable(const char* icon, const char* text, bool selected, ImGuiSelectableFlags flags, const ImVec2& size) {
    std::stringstream ss;
    ss << icon << " " << text;
    std::string selectableText = ss.str();
    
    return ImGui::Selectable(selectableText.c_str(), selected, flags, size);
}

void Icon(const char* icon) {
    ImGui::Text("%s", icon);
}

float GetIconWidth(const char* icon) {
    return ImGui::CalcTextSize(icon).x;
}

bool IconCollapsingHeader(const char* icon, const char* text, ImGuiTreeNodeFlags flags) {
    std::stringstream ss;
    ss << icon << " " << text;
    std::string headerText = ss.str();
    
    return ImGui::CollapsingHeader(headerText.c_str(), flags);
}

} // namespace IconUtils
