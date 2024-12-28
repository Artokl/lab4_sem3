#ifndef EDGE_H
#define EDGE_H

#include "Vertex.h"

// По «скриншоту» Edge хранит два объекта Vertex<T> (start, finish).

template <typename T>
struct Edge
{
    Vertex<T> start;
    Vertex<T> finish;
    double weight;

    Edge()
        : weight(0.0)
    {
    }

    Edge(const Vertex<T>& s, const Vertex<T>& f, double w)
        : start(s), finish(f), weight(w)
    {
    }
};

#endif // EDGE_H
