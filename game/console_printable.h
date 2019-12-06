#ifndef ALGORITHMIC_LANGUAGES_PROJECT_2_CONSOLE_PRINTABLE_H
#define ALGORITHMIC_LANGUAGES_PROJECT_2_CONSOLE_PRINTABLE_H

#include <iostream>

using std::cout;
using std::endl;

namespace battleships {
    class ConsolePrintable {

    public:
        virtual void print_to_console() const noexcept = 0;
    };
}


#endif //ALGORITHMIC_LANGUAGES_PROJECT_2_CONSOLE_PRINTABLE_H
