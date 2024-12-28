#ifndef LAB4_TESTS
#define LAB4_TESTS

#include "Graph.h"      // Ваш класс Graph
#include <cassert>

// Пример теста, проверяющего алгоритм Дейкстры
void TestDijkstra() {
    // Создаём граф (целочисленные «имена» вершин).
    Graph<int> graph;

    // Добавляем вершины
    graph.addVertex(0);
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addVertex(4);

    // Добавляем рёбра (как в примере)
    // (start, finish, weight)
    graph.addEdge(0, 1, 10);
    graph.addEdge(0, 2, 3);
    graph.addEdge(1, 2, 1);
    graph.addEdge(1, 3, 2);
    graph.addEdge(2, 1, 4);
    graph.addEdge(2, 3, 8);
    graph.addEdge(2, 4, 2);
    graph.addEdge(3, 4, 7);
    graph.addEdge(4, 3, 9);

    // --- Тест 1: кратчайший путь 0 -> 3 ---
    {
        auto result = graph.dijkstraPath(0, 3);
        auto dist = result.GetDistances();   // DynamicArray<int>
        auto path = result.GetPath();       // DynamicArray<int>

        // Проверяем, что расстояние до вершины 3 равно 9
        assert(dist[3] == 9);

        // Проверяем длину самого пути: должно быть 4 вершины
        assert(path.get_size() == 4);

        // Проверяем, что сам путь: 0 -> 2 -> 1 -> 3
        assert(path[0] == 0);
        assert(path[1] == 2);
        assert(path[2] == 1);
        assert(path[3] == 3);
    }

    // --- Тест 2: кратчайший путь 0 -> 4 ---
    {
        auto result = graph.dijkstraPath(0, 4);
        auto dist = result.GetDistances();
        auto path = result.GetPath();

        // Проверяем, что расстояние до вершины 4 равно 5
        assert(dist[4] == 5);

        // Проверяем длину пути: 3 вершины
        assert(path.get_size() == 3);

        // Проверяем сам путь: 0 -> 2 -> 4
        assert(path[0] == 0);
        assert(path[1] == 2);
        assert(path[2] == 4);
    }

    // --- Тест 3: кратчайший путь 0 -> 1 ---
    {
        auto result = graph.dijkstraPath(0, 1);
        auto dist = result.GetDistances();
        auto path = result.GetPath();

        // Проверяем, что расстояние до вершины 1 равно 7
        assert(dist[1] == 7);

        // Проверяем длину пути: 3 вершины
        assert(path.get_size() == 3);

        // Проверяем сам путь: 0 -> 2 -> 1
        assert(path[0] == 0);
        assert(path[1] == 2);
        assert(path[2] == 1);
    }
}

#endif // LAB4_TESTS
