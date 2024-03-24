#include "parser.hpp"
#include <cmath>


// Constructor with default parameters allowing for easy instantiation
Vertex::Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

// Overload the << operator to enable easy printing of Vertex objects
std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
    os << "Vertex: " << v.x << ", " << v.y << ", " << v.z;
    return os;
}

// facet structure represents a single triangular facet of the STL model
facet::facet(const Vertex& normal, const Vertex& v1, const Vertex& v2, const Vertex& v3)
    : normal(normal), vertices({v1, v2, v3}) {}

// Implementation for ASCII_STLFile
void ASCII_STLFile::parse(const std::string& filepath)
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
            facets.push_back(facet(normal, vertices[0], vertices[1], vertices[2]));
        }
    }

    buildGraph();
}

void ASCII_STLFile::printVertices() const
{
    // Iterate over all facets
    for (const auto& facet : facets)
    {
        // Print the normal
        std::cout << "facet normal: " << facet.normal << std::endl;
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

void ASCII_STLFile::buildGraph()
{
    std::unordered_map<Vertex, size_t, VertexHash, VertexEqual> vertexIds;
    size_t currentId = 0;

    for (const auto& facet : facets)
    {
        std::array<size_t, 3> ids;
        for (int i = 0; i < 3; ++i)
        {
            const auto& v = facet.vertices[i];
            if (vertexIds.find(v) == vertexIds.end())
            {
                vertexIds[v] = currentId;
                graph.vertices[currentId] = v;
                ids[i] = currentId;
                currentId++;
            } else
            {
                ids[i] = vertexIds[v];
            }
        }

        // Add edges for each facet
        for (int i = 0; i < 3; ++i)
        {
            graph.edges.emplace(ids[i], Edge{ids[i], ids[(i + 1) % 3]});
        }
    }
}

void ASCII_STLFile::printGraphStats() const
{
    std::cout << "Graph Stats:" << std::endl;
    std::cout << "Total vertices: " << graph.vertices.size() << std::endl;
    std::cout << "Total edges: " << graph.edges.size() << std::endl;

    // Results may be contrintuitive unless you consider that the STL is built up out of triangles
    for (const auto& vertexPair : graph.vertices)
    {
        auto range = graph.edges.equal_range(vertexPair.first);
        std::cout << "Vertex " << vertexPair.first << " (" << vertexPair.second << ") is connected to: ";
        for (auto it = range.first; it != range.second; ++it)
        {
            std::cout << it->second.endVertexId << " ";
        }
        std::cout << std::endl;
    }
}

void ASCII_STLFile::identifySupportChallenges() const
{
    float minY = std::numeric_limits<float>::max();
    //float maxY = std::numeric_limits<float>::min();
    for (const auto& vertexPair : graph.vertices)
    {
        minY = std::min(minY, vertexPair.second.y);
        //maxY = std::max(maxY, vertexPair.second.y);
    }

    //std::cout << minY << std::endl;
    //std::cout << maxY << std::endl;

    // Threshold to consider a vertex as part of the model's bottom
    const float bottomThreshold = minY;
    //const float bottomThresholdTolerance = (maxY - minY) / 100000.0;
    //std::cout << bottomThresholdTolerance << std::endl;

    std::vector<size_t> problematicVertices;

    for (const auto& vertexPair : graph.vertices)
    {
        const auto& vertex = vertexPair.second;

        // Skip vertices at the model's bottom
        if (vertex.y <= bottomThreshold) continue;

        bool hasLowerNeighbor = false;

        // Check connections for a lower neighbor
        auto range = graph.edges.equal_range(vertexPair.first);
        for (auto it = range.first; it != range.second && !hasLowerNeighbor; ++it)
        {
            const auto& neighborVertex = graph.vertices.at(it->second.endVertexId);
            if (neighborVertex.y < vertex.y)
            {
                hasLowerNeighbor = true;
            }
        }

        // If a vertex lacks lower neighbors and isn't at the bottom, it's potentially problematic
        if (!hasLowerNeighbor)
        {
            problematicVertices.push_back(vertexPair.first);
        }
    }

    // Output or handle problematic vertices
    std::cout << "Vertices potentially needing support: ";
    for (auto id : problematicVertices)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
}

// Implementation for Binary_STLFile
void Binary_STLFile::parse(const std::string& filepath)
{
    // Open the file for binary reading
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Could not open file " << filepath << std::endl;
        // Early return if file can't be opened
        return;
    }

     // Ignore the header
    file.ignore(HEADER_SIZE);
    // Variable to store the number of triangles
    unsigned int triangleCount;
    // Read the triangle count
    file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));

    //std::cout << triangleCount << std::endl;

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
        facets.push_back(facet(normal, v1, v2, v3));

        //std::cout << facets[i].normal << std::endl;

        // Skip the attribute byte count (unused)
        file.ignore(ATTRIBUTE_SIZE);
    }

    buildGraph();
}

void Binary_STLFile::printVertices() const
{
    // Iterate over all facets
    for (const auto& facet : facets)
    {
        // Print the normal
        std::cout << "facet normal: " << facet.normal << std::endl;
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

void Binary_STLFile::buildGraph()
{
    std::unordered_map<Vertex, size_t, VertexHash, VertexEqual> vertexIds;
    size_t currentId = 0;

    for (const auto& facet : facets)
    {
        std::array<size_t, 3> ids;
        for (int i = 0; i < 3; ++i)
        {
            const auto& v = facet.vertices[i];
            if (vertexIds.find(v) == vertexIds.end())
            {
                vertexIds[v] = currentId;
                graph.vertices[currentId] = v;
                ids[i] = currentId;
                currentId++;
            } else
            {
                ids[i] = vertexIds[v];
            }
        }

        // Add edges for each facet
        for (int i = 0; i < 3; ++i)
        {
            graph.edges.emplace(ids[i], Edge{ids[i], ids[(i + 1) % 3]});
        }
    }
}

void Binary_STLFile::printGraphStats() const
{
    std::cout << "Graph Stats:" << std::endl;
    std::cout << "Total vertices: " << graph.vertices.size() << std::endl;
    std::cout << "Total edges: " << graph.edges.size() << std::endl;

    // Results may be contrintuitive unless you consider that the STL is built up out of triangles
    for (const auto& vertexPair : graph.vertices)
    {
        auto range = graph.edges.equal_range(vertexPair.first);
        std::cout << "Vertex " << vertexPair.first << " (" << vertexPair.second << ") is connected to: ";
        for (auto it = range.first; it != range.second; ++it)
        {
            std::cout << it->second.endVertexId << " ";
        }
        std::cout << std::endl;
    }
}

void Binary_STLFile::identifySupportChallenges() const
{
    float minY = std::numeric_limits<float>::max();
    //float maxY = std::numeric_limits<float>::min();
    for (const auto& vertexPair : graph.vertices)
    {
        minY = std::min(minY, vertexPair.second.y);
        //maxY = std::max(maxY, vertexPair.second.y);
    }

    //std::cout << minY << std::endl;
    //std::cout << maxY << std::endl;

    // Threshold to consider a vertex as part of the model's bottom
    const float bottomThreshold = minY;
    //const float bottomThresholdTolerance = (maxY - minY) / 100000.0;
    //std::cout << bottomThresholdTolerance << std::endl;

    std::vector<size_t> problematicVertices;

    for (const auto& vertexPair : graph.vertices)
    {
        const auto& vertex = vertexPair.second;

        // Skip vertices at the model's bottom
        if (vertex.y <= bottomThreshold) continue;

        bool hasLowerNeighbor = false;

        // Check connections for a lower neighbor
        auto range = graph.edges.equal_range(vertexPair.first);
        for (auto it = range.first; it != range.second && !hasLowerNeighbor; ++it)
        {
            const auto& neighborVertex = graph.vertices.at(it->second.endVertexId);
            if (neighborVertex.y < vertex.y)
            {
                hasLowerNeighbor = true;
            }
        }

        // If a vertex lacks lower neighbors and isn't at the bottom, it's potentially problematic
        if (!hasLowerNeighbor)
        {
            problematicVertices.push_back(vertexPair.first);
        }
    }

    // Output or handle problematic vertices
    std::cout << "Vertices potentially needing support: ";
    for (auto id : problematicVertices)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
}
