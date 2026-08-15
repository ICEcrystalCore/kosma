//
// Created by Jun on 2025/8/16.
//

#include "KSEditor.h"

#ifdef IS_WIN
int wmain(int argc, wchar_t* argv[])
{
    Kosma::Editor editor(argc, argv);
    editor.run();
    return 0;
}
#else
int main(int argc, char** argv)
{
    Kosma::Editor editor(argc, argv);
    editor.run();
    return 0;
}
#endif
