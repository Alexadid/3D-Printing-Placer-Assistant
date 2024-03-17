#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <sstream>

// Vertex structure to represent a single vertex in 3D space
struct Vertex
{
    // Coordinates of the vertex
    float x, y, z;
    // Constructor with default parameters allowing for easy instantiation
    Vertex(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

// Overload the << operator to enable easy printing of Vertex objects
std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
    os << "Vertex: " << v.x << ", " << v.y << ", " << v.z;
    return os;
}

// Facet structure represents a single triangular facet of the STL model
struct Facet
{
    // Normal vector of the facet
    Vertex normal;
    // Array of vertices (triangle) making up the facet
    std::array<Vertex, 3> vertices;
    // Constructor initializes the facet with its normal and vertices
    Facet(const Vertex& normal, const Vertex& v1, const Vertex& v2, const Vertex& v3)
        : normal(normal), vertices({v1, v2, v3}) {}
};

// Abstract base class representing an STL file
class STLFile
{
public:
    // Pure virtual function for parsing the STL file
    virtual void parse(const std::string& filepath) = 0;
    // Pure virtual function for printing the vertices of the STL file
    virtual void printVertices() const = 0;
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~STLFile() {}
};

// Derived class for handling ASCII STL files
class ASCII_STLFile : public STLFile
{
private:
    // Container for all the facets parsed from the file
    std::vector<Facet> facets;

public:
    // Override the parse method to handle ASCII STL files
    void parse(const std::string& filepath) override
    {
        // Open the file for reading
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Could not open file " << filepath << std::endl;
            // Early return if file can't be opened
            return;
        }

        // Variable to hold each line read from the file
        std::string line;
        // Read the file line by line
        while (std::getline(file, line))
        {
            // Use stringstream for parsing
            std::stringstream linestream(line);
            // Token to identify keywords in the line
            std::string token;
            // Extract the first word to determine line type
            linestream >> token;

            // If the line defines a facet normal
            if (token == "facet")
            {
                // Placeholder for "normal" keyword
                std::string normalToken;
                // Variable to store the normal vector
                Vertex normal;
                linestream >> normalToken >> normal.x >> normal.y >> normal.z;

                // Read the vertices of the facet
                // Skip "outer loop" line
                std::getline(file, line);
                // Array to store facet vertices
                std::array<Vertex, 3> vertices;
                // Loop to read each vertex
                for (int i = 0; i < 3; ++i)
                {
                    // Read the vertex line
                    std::getline(file, line);
                    // Use stringstream for parsing
                    std::stringstream vertexStream(line);
                    // Extract vertex coordinates
                    vertexStream >> token >> vertices[i].x >> vertices[i].y >> vertices[i].z;
                }

                // Skip "endloop"
                std::getline(file, line);
                // Skip "endfacet"
                std::getline(file, line);

                // Add the facet to the vector
                facets.push_back(Facet(normal, vertices[0], vertices[1], vertices[2]));
            }
        }
    }

    // Override the printVertices method to display vertices of the ASCII STL file
    void printVertices() const override
    {
        // Iterate over all facets
        for (const auto& facet : facets)
        {
            // Print the normal
            std::cout << "Facet normal: " << facet.normal << std::endl;
            // Iterate over the vertices of each facet
            for (const auto& vertex : facet.vertices)
            {
                // Print each vertex
                std::cout << vertex << std::endl;
            }

            // Add a newline for readability
            std::cout << std::endl;
        }
    }
};

// Derived class for handling Binary STL files
class Binary_STLFile : public STLFile
{
private:
    // Container for all the facets parsed from the file
    std::vector<Facet> facets;

public:
    // Override the parse method to handle Binary STL files
    void parse(const std::string& filepath) override
    {
        // Open the file for binary reading
        std::ifstream file(filepath, std::ios::binary);
        if (!file)
        {
            std::cerr << "Could not open file " << filepath << std::endl;
            // Early return if file can't be opened
            return;
        }

        // Array to hold the STL file header
        char header[80];
        // Read the header (unused)
        file.read(header, 80);
        // Variable to store the number of triangles
        unsigned int triangleCount;
        // Read the triangle count
        file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));

        // Read each triangle
        for (unsigned int i = 0; i < triangleCount; ++i)
        {
            // Variables to store the normal and vertices
            Vertex normal, v1, v2, v3;
            // Read the normal vector
            file.read(reinterpret_cast<char*>(&normal), sizeof(Vertex));
            // Read vertex 1
            file.read(reinterpret_cast<char*>(&v1), sizeof(Vertex));
            // Read vertex 2
            file.read(reinterpret_cast<char*>(&v2), sizeof(Vertex));
            // Read vertex 3
            file.read(reinterpret_cast<char*>(&v3), sizeof(Vertex));
            // Add the facet to the vector
            facets.push_back(Facet(normal, v1, v2, v3));

            // Skip the attribute byte count (unused)
            file.ignore(2);
        }
    }

    // Override the printVertices method to display vertices of the Binary STL file
    void printVertices() const override
    {
        // Iterate over all facets
        for (const auto& facet : facets)
        {
            // Print the normal
            std::cout << "Facet normal: " << facet.normal << std::endl;
            // Iterate over the vertices of each facet
            for (const auto& vertex : facet.vertices)
            {
                // Print each vertex
                std::cout << vertex << std::endl;
            }

            // Add a newline for readability
            std::cout << std::endl;
        }
    }
};

int main()
{
    // Example usage for ASCII STL file
    ASCII_STLFile asciiStl;
    // Parse the ASCII STL file
    asciiStl.parse("Cube-ASCII.stl");
    std::cout << "ASCII STL Vertices:" << std::endl;
    // Print the vertices
    asciiStl.printVertices();

    // Example usage for Binary STL file
    Binary_STLFile binaryStl;
    // Parse the Binary STL file
    binaryStl.parse("Cube-Binary.stl");
    std::cout << "Binary STL Vertices:" << std::endl;
    // Print the vertices
    binaryStl.printVertices();

    return 0;
}
