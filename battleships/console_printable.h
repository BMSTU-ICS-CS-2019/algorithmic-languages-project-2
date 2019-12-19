#pragma once

#include <iostream>

using std::cout;
using std::endl;

namespace battleships  {
    class ConsolePrintable {

    public:
        virtual void print_to_console() const noexcept = 0;
    };
}
