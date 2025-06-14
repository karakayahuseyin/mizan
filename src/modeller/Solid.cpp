#include "Solid.h"

namespace BREP {

void Solid::addShell(std::shared_ptr<Shell> shell) {
    if (shell) {
        m_shells.push_back(shell);
    }
}

const std::vector<std::shared_ptr<Shell>>& Solid::getShells() const {
    return m_shells;
}

bool Solid::isValid() const {
    if (m_shells.empty()) return false;
    
    for (const auto& shell : m_shells) {
        if (!shell || !shell->isValid()) {
            return false;
        }
    }
    return true;
}

} // namespace BREP