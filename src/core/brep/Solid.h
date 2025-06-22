#ifndef SOLID_H
#define SOLID_H

#include <vector>
#include "Shell.h"

namespace BREP {

class Solid {
public:
    enum class PrimitiveType {
        Cube,
        Sphere,
        Cylinder,
        Pyramid,
        Cone,
        Torus
    };

    void addShell(std::shared_ptr<Shell> shell);
    const std::vector<std::shared_ptr<Shell>>& getShells() const;
    bool isValid() const;
private:
    std::vector<std::shared_ptr<Shell>> m_shells;
};

} // namespace BREP

#endif // SOLID_H