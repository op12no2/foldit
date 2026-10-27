//{{{}}}
// main.cpp : foldit - a terminal folding editor

#include <clocale>
#include <cstdio>
#include <cstring>
#include "editor.h"

int main(int argc, char **argv)
{
    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        fprintf(stderr, "usage: foldit file\n\nF1 in the editor lists the keys.\n");
        return argc == 2 ? 0 : 1;
    }

    setlocale(LC_ALL, "");
    Config cfg;
    cfg.load();

    Editor ed(cfg);
    std::string err;
    if (!ed.open_file(argv[1], err))
    {
        fprintf(stderr, "foldit: %s\n", err.c_str());
        return 1;
    }
    ed.run();
    return 0;
}
