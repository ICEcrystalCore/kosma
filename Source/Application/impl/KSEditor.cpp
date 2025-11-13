//
// Created by Jun on 2025/8/30.
//

#include "KSEditor.h"

namespace Kosma {

Editor::Editor(int argc, char** argv)
    : Application(argc, argv)
{
}

Editor::Editor(int argc, wchar_t** argv)
    : Application(argc, argv)
{
}

void Editor::initialize()
{
}

bool Editor::isRunning() const
{
    return false;
}

void Editor::update()
{
}

void Editor::shutdown()
{
}

}