#ifndef TOOLKIT_H
#define TOOLKIT_H

class Toolkit {
public:
    Toolkit() = default;
    ~Toolkit() = default;

    void initialize();
    void cleanup();
};

#endif // TOOLKIT_H