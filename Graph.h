#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include "Vertex.h"
#include "Edge.h"
#include "Path.h"
#include "DynamicArray.h"

template<typename T>
class Graph
{
private:
    std::vector<Vertex<T>> vertices;  // Шаблонные вершины
    std::vector<Edge<T>> edges;       // Шаблонные рёбра

public:
    Graph() = default;
    ~Graph() = default;

    // -- Вспомогательные --
    Vertex<T>* findVertex(const T& name);
    const Vertex<T>* findVertexConst(const T& name) const;

    // -- Добавление/удаление вершин --
    void addVertex(const T& name);
    void removeVertex(const T& name);

    // -- Добавление/удаление рёбер --
    void addEdge(const T& startName, const T& finishName, double weight);
    void removeEdge(const T& startName, const T& finishName);

    // -- Проверка --
    bool containsVertex(const T& name) const;

    // -- Алгоритмы --
    Path<T> dijkstraPath(const T& startName, const T& finishName);

    // Обходы
    void depthFirstSearch(const T& startName);
    void breadthFirstSearch(const T& startName);

private:
    void dfsUtil(Vertex<T>& v, std::vector<T>& visited);
};

#endif // GRAPH_H
