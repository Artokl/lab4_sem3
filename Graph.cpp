#include "Graph.h"
#include <stdexcept>
#include <iostream>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_map>

// -----------------------------------------------------------------
//  Вспомогательные методы
template<typename T>
Vertex<T>* Graph<T>::findVertex(const T& name)
{
    for (auto& v : vertices)
    {
        if (v.name == name)
            return &v;
    }
    return nullptr;
}

template<typename T>
const Vertex<T>* Graph<T>::findVertexConst(const T& name) const
{
    for (auto& v : vertices)
    {
        if (v.name == name)
            return &v;
    }
    return nullptr;
}

// -----------------------------------------------------------------
//  Добавление/удаление вершин
template<typename T>
void Graph<T>::addVertex(const T& name)
{
    if (containsVertex(name))
    {
        std::cerr << "Вершина '" << name << "' уже существует!\n";
        return;
    }
    vertices.emplace_back(name);
}

template<typename T>
void Graph<T>::removeVertex(const T& name)
{
    Vertex<T>* vPtr = findVertex(name);
    if (!vPtr)
    {
        std::cerr << "Вершина '" << name << "' не найдена. Удалять нечего.\n";
        return;
    }

    // Удаляем рёбра из общего списка edges
    edges.erase(std::remove_if(edges.begin(), edges.end(),
        [&](const Edge<T>& e){
            return (e.start.name == name || e.finish.name == name);
        }), edges.end());

    // Удаляем эти же рёбра из in/out остальных вершин
    for (auto& vx : vertices)
    {
        vx.out.erase(std::remove_if(vx.out.begin(), vx.out.end(),
            [&](const Edge<T>& e){
                return (e.start.name == name || e.finish.name == name);
            }), vx.out.end());

        vx.in.erase(std::remove_if(vx.in.begin(), vx.in.end(),
            [&](const Edge<T>& e){
                return (e.start.name == name || e.finish.name == name);
            }), vx.in.end());
    }

    // Удаляем саму вершину
    vertices.erase(std::remove_if(vertices.begin(), vertices.end(),
        [&](const Vertex<T>& vx){
            return vx.name == name;
        }), vertices.end());
}

// -----------------------------------------------------------------
//  Добавление/удаление рёбер
template<typename T>
void Graph<T>::addEdge(const T& startName, const T& finishName, double weight)
{
    Vertex<T>* s = findVertex(startName);
    Vertex<T>* f = findVertex(finishName);
    if (!s || !f)
        throw std::runtime_error("Не найдена вершина при добавлении ребра.");

    Edge<T> e(*s, *f, weight);
    edges.push_back(e);

    // Добавляем в out / in
    s->out.push_back(e);
    f->in.push_back(e);
}

template<typename T>
void Graph<T>::removeEdge(const T& startName, const T& finishName)
{
    // Удаляем из общего списка edges
    edges.erase(std::remove_if(edges.begin(), edges.end(),
        [&](const Edge<T>& e){
            return (e.start.name == startName && e.finish.name == finishName);
        }), edges.end());

    // Удаляем из out
    Vertex<T>* s = findVertex(startName);
    if (s)
    {
        s->out.erase(std::remove_if(s->out.begin(), s->out.end(),
            [&](const Edge<T>& e){
                return (e.start.name == startName && e.finish.name == finishName);
            }), s->out.end());
    }

    // Удаляем из in
    Vertex<T>* f = findVertex(finishName);
    if (f)
    {
        f->in.erase(std::remove_if(f->in.begin(), f->in.end(),
            [&](const Edge<T>& e){
                return (e.start.name == startName && e.finish.name == finishName);
            }), f->in.end());
    }
}

// -----------------------------------------------------------------
//  Проверка, существует ли вершина
template<typename T>
bool Graph<T>::containsVertex(const T& name) const
{
    return (findVertexConst(name) != nullptr);
}

// -----------------------------------------------------------------
//  Алгоритм Дейкстры
template<typename T>
Path<T> Graph<T>::dijkstraPath(const T& startName, const T& finishName)
{
    // Если нет одной из вершин, возвращаем путь с -1
    if (!containsVertex(startName) || !containsVertex(finishName))
    {
        DynamicArray<int> distArr;
        DynamicArray<T> pathArr;
        // Заполним distArr -1 (на каждую вершину)
        int n = static_cast<int>(vertices.size());
        for (int i = 0; i < n; ++i)
            distArr.push_back(-1);

        return Path<T>(distArr, pathArr);
    }

    // Словари: имя -> расстояние, имя -> предыдущая вершина
    std::unordered_map<T, double> dist;
    std::unordered_map<T, T> prev;

    const double INF = std::numeric_limits<double>::infinity();

    // Инициализация
    for (auto& v : vertices)
        dist[v.name] = INF;
    dist[startName] = 0.0;

    // priority_queue c компаратором (min-heap)
    auto cmp = [](const std::pair<double, T>& a,
                  const std::pair<double, T>& b)
    {
        return a.first > b.first;
    };
    std::priority_queue<
        std::pair<double, T>,
        std::vector<std::pair<double, T>>,
        decltype(cmp)
    > pq(cmp);

    pq.push({0.0, startName});

    while (!pq.empty())
    {
        auto [curDist, curName] = pq.top();
        pq.pop();

        if (curDist > dist[curName])
            continue;
        if (curName == finishName)
            break;

        Vertex<T>* curV = findVertex(curName);
        if (!curV)
            continue;

        for (auto& edge : curV->out)
        {
            T neighName = edge.finish.name;
            double alt = curDist + edge.weight;

            if (alt < dist[neighName])
            {
                dist[neighName] = alt;
                prev[neighName] = curName;
                pq.push({alt, neighName});
            }
        }
    }

    // Заполняем Path
    DynamicArray<int> distArr;
    DynamicArray<T> pathArr;

    // distArr по порядку вершин
    for (auto& v : vertices)
    {
        double d = dist[v.name];
        if (d == INF)
            distArr.push_back(-1);
        else
            distArr.push_back(static_cast<int>(d));
    }

    // Если финиш недостижим
    if (dist[finishName] == INF)
    {
        return Path<T>(distArr, pathArr);
    }

    // Восстанавливаем путь
    {
        T current = finishName;
        while (true)
        {
            pathArr.push_back(current);
            if (current == startName)
                break;
            current = prev[current];
        }
        // Разворачиваем (reverse)
        int left = 0;
        int right = static_cast<int>(pathArr.get_size()) - 1;
        while (left < right)
        {
            T tmp = pathArr[left];
            pathArr[left] = pathArr[right];
            pathArr[right] = tmp;
            ++left;
            --right;
        }
    }

    return Path<T>(distArr, pathArr);
}

// -----------------------------------------------------------------
//  DFS
template<typename T>
void Graph<T>::depthFirstSearch(const T& startName)
{
    Vertex<T>* startV = findVertex(startName);
    if (!startV)
    {
        std::cerr << "DFS: вершина '" << startName << "' не найдена!\n";
        return;
    }

    std::vector<T> visited;
    visited.reserve(vertices.size());

    dfsUtil(*startV, visited);

    // Вывод (пример)
    std::cout << "DFS order: ";
    for (auto& nm : visited)
        std::cout << nm << " ";
    std::cout << std::endl;
}

template<typename T>
void Graph<T>::dfsUtil(Vertex<T>& v, std::vector<T>& visited)
{
    visited.push_back(v.name);

    for (auto& edge : v.out)
    {
        Vertex<T>* nextV = findVertex(edge.finish.name);
        if (!nextV)
            continue;
        if (std::find(visited.begin(), visited.end(), nextV->name) == visited.end())
        {
            dfsUtil(*nextV, visited);
        }
    }
}

// -----------------------------------------------------------------
//  BFS
template<typename T>
void Graph<T>::breadthFirstSearch(const T& startName)
{
    Vertex<T>* startV = findVertex(startName);
    if (!startV)
    {
        std::cerr << "BFS: вершина '" << startName << "' не найдена!\n";
        return;
    }

    std::vector<T> visited;
    visited.reserve(vertices.size());
    std::queue<Vertex<T>*> q;

    visited.push_back(startV->name);
    q.push(startV);

    while (!q.empty())
    {
        Vertex<T>* cur = q.front();
        q.pop();

        for (auto& edge : cur->out)
        {
            Vertex<T>* neighbor = findVertex(edge.finish.name);
            if (!neighbor)
                continue;

            if (std::find(visited.begin(), visited.end(), neighbor->name) == visited.end())
            {
                visited.push_back(neighbor->name);
                q.push(neighbor);
            }
        }
    }

    // Вывод (пример)
    std::cout << "BFS order: ";
    for (auto& nm : visited)
        std::cout << nm << " ";
    std::cout << std::endl;
}

// -----------------------------------------------------------------
//  (Если хотите явно инстанцировать для string)
template class Graph<std::string>;
template class Graph<int>;
