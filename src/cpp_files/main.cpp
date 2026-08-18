#include <iostream>
#include "FilesScanner.cpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 1;
    }
    FilesScanner scanner(argv[1]);
    std::cout << "initialized successfully" << std::endl;
    scanner.scan();
    return 0;
}