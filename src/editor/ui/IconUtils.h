#ifndef ICONUTILS_H
#define ICONUTILS_H

#include <imgui.h>
#include <string>

/**
 * Utility functions for creating UI elements with icons
 */
namespace IconUtils {
    
    /**
     * Create a button with an icon and text
     * @param icon Unicode icon string
     * @param text Button text
     * @param size Button size (optional)
     * @return true if button was clicked
     */
    bool IconButton(const char* icon, const char* text, const ImVec2& size = ImVec2(0, 0));
    
    /**
     * Create a button with only an icon
     * @param icon Unicode icon string
     * @param tooltip Tooltip text to show on hover
     * @param size Button size (optional)
     * @return true if button was clicked
     */
    bool IconOnlyButton(const char* icon, const char* tooltip = nullptr, const ImVec2& size = ImVec2(0, 0));
    
    /**
     * Create a menu item with an icon
     * @param icon Unicode icon string
     * @param text Menu item text
     * @param shortcut Keyboard shortcut text (optional)
     * @param selected Whether the item is selected
     * @param enabled Whether the item is enabled
     * @return true if menu item was clicked
     */
    bool IconMenuItem(const char* icon, const char* text, const char* shortcut = nullptr, bool selected = false, bool enabled = true);
    
    /**
     * Create a selectable item with an icon
     * @param icon Unicode icon string
     * @param text Selectable text
     * @param selected Whether the item is selected
     * @param flags ImGuiSelectableFlags
     * @param size Item size (optional)
     * @return true if item was clicked
     */
    bool IconSelectable(const char* icon, const char* text, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
    
    /**
     * Render just an icon (useful for inline icons)
     * @param icon Unicode icon string
     */
    void Icon(const char* icon);
    
    /**
     * Get the width of an icon for layout calculations
     * @param icon Unicode icon string
     * @return Width in pixels
     */
    float GetIconWidth(const char* icon);
    
    /**
     * Create a collapsing header with an icon
     * @param icon Unicode icon string
     * @param text Header text
     * @param flags ImGuiTreeNodeFlags
     * @return true if header is open
     */
    bool IconCollapsingHeader(const char* icon, const char* text, ImGuiTreeNodeFlags flags = 0);
}

#endif // ICONUTILS_H
