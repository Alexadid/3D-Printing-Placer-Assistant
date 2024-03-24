#include <iostream>
#include "parser.hpp"

int main()
{
    // Example usage for ASCII STL file
    ASCII_STLFile asciiStl;
    // Parse the ASCII STL file
    asciiStl.parse("../Models/Test_sword_ASCII.stl");
    //std::cout << "ASCII STL Vertices:" << std::endl;
    // Print the vertices
    //asciiStl.printVertices();
    // Print graph's statistics
    //asciiStl.printGraphStats();
    // Examplary usage of function to identify problematic points
    asciiStl.identifySupportChallenges();


    
    // Example usage for Binary STL file
    Binary_STLFile binaryStl;
    // Parse the Binary STL file
    binaryStl.parse("../Models/Test_sword_Binary.stl");
    //std::cout << "Binary STL Vertices:" << std::endl;
    // Print the vertices
    //binaryStl.printVertices();

    binaryStl.identifySupportChallenges();
    

    return 0;
}