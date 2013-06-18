#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif

#include <cstdlib>
#include <iostream>
#include "blob_finder.hpp"

using namespace bias;

int main(int argc, char** argv)
{        
    std::cout << std::endl;
    std::cout << "--------------------------------------- " << std::endl;
    std::cout << "  Blob Finder Demo                      " << std::endl;
    std::cout << "--------------------------------------- " << std::endl;

    bool success = true;

    BlobFinder blobFinder; 

    success = blobFinder.setupCamera();
    if (!success)
    {
        std::cout << "* failed to setup camera" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    success = blobFinder.run();
    if (!success)
    {
        std::cout << "* failure during blob detection" << std::endl;
    }

    success = blobFinder.cleanUp();
    if (!success)
    {
        std::cout << "* failed to clean up camera" << std::endl;
        std::exit(EXIT_FAILURE);
    }

	return 0;
}

