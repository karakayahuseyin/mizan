#ifndef SOLID_H
#define SOLID_H

#include <vector>
#include "Shell.h"

namespace BREP {

class Solid {
private:
    std::vector<std::shared_ptr<Shell>> m_shells;

public:
    void addShell(std::shared_ptr<Shell> shell);
    const std::vector<std::shared_ptr<Shell>>& getShells() const;
    bool isValid() const;
};

} // namespace BREP

#endif // SOLID_H