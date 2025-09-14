//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSEDITOR_H
#define KOSMA_KSEDITOR_H

#include "KSApplication.h"

namespace Kosma {

class Editor : public Core::Application {
public:
    Editor(int argc, char** argv);

    void initialize() override;

    bool isRunning() const override;

    void update() override;

    void shutdown() override;
};

}

#endif  // KOSMA_KSEDITOR_H
