#include "triglav.h"
#include "config.h"
#include <cstring>

int main(int argc, char * argv[])
{
    triglav::agent_reading_input_files a;

    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <file>\n";
    }
    else
    {
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1],"--version"))
        {
            std::cout << PACKAGE_STRING << "\n";
            std::cout << "Copyright by Pawel Biernacki\n";
            std::cout << "Vantaa 2021\n";
        }
        else
        {
            if (a.parse(argv[1]))
            {
                return -1;
            }
            a.execute();
        }
    }

    return 0;
}
