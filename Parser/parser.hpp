#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <limits>
#include <thread>

// Vertex structure to represent a single vertex in 3D space
struct Vertex
{
    // Coordinates of the vertex
    float x, y, z;
    // Constructor with default parameters allowing for easy instantiation
    Vertex(float x = 0.0, float y = 0.0, float z = 0.0);
};

// Hash function for Vertex
struct VertexHash
{
    size_t operator()(const Vertex& v) const
    {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }
};

// Equality comparison for Vertex
struct VertexEqual
{
    bool operator()(const Vertex& v1, const Vertex& v2) const
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }
};

// Overload the << operator to enable easy printing of Vertex objects
std::ostream& operator<<(std::ostream& os, const Vertex& v);

// facet structure represents a single triangular facet of the STL model
struct facet
{
    // Normal vector of the facet
    Vertex normal;
    // Array of vertices (triangle) making up the facet
    std::array<Vertex, 3> vertices;
    // Constructor initializes the facet with its normal and vertices
    facet(const Vertex& normal, const Vertex& v1, const Vertex& v2, const Vertex& v3);
};

// Edge structure to represent a single edge via 2 unique ids (hashed)
struct Edge
{
    size_t startVertexId;
    size_t endVertexId;
};

// Graph class dedicated to representing the 3D model in a way more suitable for analysis
class Graph
{
public:
    // Maps vertex ID to Vertex
    std::unordered_map<size_t, Vertex> vertices;
    // Maps a vertex ID to its edges for quick lookup
    std::unordered_multimap<size_t, Edge> edges;
};

enum class STLFileType
{
    ASCII,
    Binary,
    Unknown
};

STLFileType determineSTLFileType(const std::string&);

// Abstract base class representing an STL file
class STLFile
{
public:
    // Pure virtual function for parsing the STL file
    virtual void parse(const std::string& filepath) = 0;
    // Pure virtual function for printing the vertices of the STL file
    virtual void printVertices() const = 0;
    // Pure virtual function to turn a vector of verticese into a graph
    virtual void buildGraph() = 0;
    // Pure virtual function to print graph's statistics
    virtual void printGraphStats() const = 0;
    // Pure virtual function to identify vertices, which may need supports
    virtual std::vector<Vertex> identifySupportChallenges() const = 0;
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~STLFile() {}
};

// Derived class for handling ASCII STL files
class ASCII_STLFile : public STLFile
{
private:
    // Container for all the facets parsed from the file
    std::vector<facet> facets;
    Graph graph;

public:
    // A function to parse the 3D model by saving facets into a vector
    // and building a graph accordingly
    void parse(const std::string& filepath) override;
    void printVertices() const override;
    void buildGraph() override;
    void printGraphStats() const override;
    std::vector<Vertex> identifySupportChallenges() const override;
};

// Constant values for parsing binary STLs
const int HEADER_SIZE       = 80;
const int FACET_SIZE        = 50;
const int ATTRIBUTE_SIZE    = 2;

// Derived class for handling Binary STL files
class Binary_STLFile : public STLFile
{
private:
    std::vector<facet> facets;
    Graph graph;

public:
    void parse(const std::string&) override;
    void printVertices() const override;
    void buildGraph() override;
    void printGraphStats() const override;
    std::vector<Vertex> identifySupportChallenges() const override;
};

#endif // PARSER_HPP
