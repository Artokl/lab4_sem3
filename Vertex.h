#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

// Нужно заранее объявить шаблон Edge<T> (forward-declaration),
// чтобы Vertex<T> мог хранить вектора Edge<T>.
template <typename T>
struct Edge;

template <typename T>
struct Vertex
{
    T name;                   // «Имя» вершины: может быть std::string, int, и т.д.
    std::vector<Edge<T>> in;  // входящие рёбра
    std::vector<Edge<T>> out; // исходящие рёбра

    Vertex() = default;

    explicit Vertex(const T& n)
        : name(n)
    {
    }
};

#endif // VERTEX_H
