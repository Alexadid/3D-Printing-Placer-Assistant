#include <iostream>
#include "parser.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <STL file path>" << std::endl;
        return 1;
    }

    std::string stlPath = argv[1];

    STLFileType type = determineSTLFileType(stlPath);

    switch (type)
    {
        case STLFileType::ASCII:
        {
            ASCII_STLFile stlFile;

            // Attempt to parse the STL file and identify problematic vertices
            stlFile.parse(stlPath);

            auto problematicVertices = stlFile.identifySupportChallenges();

            // Print the problematic vertices
            for (const auto& vertex : problematicVertices)
            {
                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
            }

            // Print the problematic vertices
            for (const auto& vertex : problematicVertices)
            {
                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
            }

            break;
        }
        case STLFileType::Binary:
        {
            Binary_STLFile stlFile;

            // Attempt to parse the STL file and identify problematic vertices
            stlFile.parse(stlPath);

            auto problematicVertices = stlFile.identifySupportChallenges();

            // Print the problematic vertices
            for (const auto& vertex : problematicVertices)
            {
                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
            }

            // Print the problematic vertices
            for (const auto& vertex : problematicVertices)
            {
                std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
            }

            break;
        }
        default:
        {
            std::cerr << "Unable to determine the STL file type." << std::endl;
            break;
        }
    }   

    return 0;
}
