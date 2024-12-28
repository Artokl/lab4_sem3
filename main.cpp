#include <iostream>
#include "Graph.h"
#include "DynamicArray.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QPolygonF>
#include <cmath>

#include "tests.h"
#include "Path.h"

// --------------------- GraphScene Class Implementation ---------------------

class GraphScene : public QGraphicsScene {
    Q_OBJECT

public:
    GraphScene(Graph<int> *graph, QObject *parent = nullptr)
        : QGraphicsScene(parent), graph(graph), currentVertexId(0),
          startVertex(nullptr), selectedStartVertex(nullptr), selectedEndVertex(nullptr) {}

    void setGraph(Graph<int> *newGraph) {
        this->graph = newGraph;
        this->currentVertexId = 0;
        this->startVertex = nullptr;
        this->selectedStartVertex = nullptr;
        this->selectedEndVertex = nullptr;
    }

signals:
    void verticesSelected(int start, int end);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        QPointF pos = event->scenePos();
        QGraphicsItem *item = itemAt(pos, QTransform());

        if (event->button() == Qt::LeftButton) {
            if (!item) {
                // (1) Добавляем вершину
                auto *vertex = addEllipse(pos.x() - 15, pos.y() - 15, 30, 30,
                                          QPen(Qt::black), QBrush(Qt::blue));
                vertex->setData(0, currentVertexId);

                // Добавляем текст номера вершины с чёрным цветом
                auto *vertexLabel = addText(QString::number(currentVertexId));
                vertexLabel->setDefaultTextColor(Qt::black); // Устанавливаем цвет текста
                vertexLabel->setPos(pos.x() - 5, pos.y() - 25);

                // **Добавляем данные для текстового элемента**
                vertexLabel->setData(0, currentVertexId); // Добавлено

                // Добавляем в логический граф
                graph->addVertex(currentVertexId++);
            }
            else if (item->type() == QGraphicsEllipseItem::Type) {
                // (2) Добавляем ребро (дугу), если уже выбрана стартовая вершина
                auto *clickedVertex = qgraphicsitem_cast<QGraphicsEllipseItem *>(item);

                if (!startVertex) {
                    // Ещё не выбрали стартовую — выбираем
                    startVertex = clickedVertex;
                    startVertex->setBrush(Qt::red);
                }
                else {
                    // У нас уже есть startVertex, теперь endVertex
                    auto *endVertex = clickedVertex;
                    if (startVertex != endVertex) {
                        int startId = startVertex->data(0).toInt();
                        int endId   = endVertex->data(0).toInt();

                        bool ok;
                        int weight = QInputDialog::getInt(
                                         nullptr,
                                         "Вес дуги",
                                         "Введите вес дуги:",
                                         1, // значение по умолчанию
                                         1, // мин
                                         100, // макс
                                         1, // шаг
                                         &ok
                                     );
                        if (!ok) {
                            startVertex->setBrush(Qt::blue);
                            startVertex = nullptr;
                            return;
                        }

                        // В логическом графе
                        graph->addEdge(startId, endId, weight);

                        // Рисуем (линия + стрелка + текст)
                        QPointF startCenter = startVertex->rect().center() + startVertex->scenePos();
                        QPointF endCenter   = endVertex->rect().center() + endVertex->scenePos();
                        // 1) Линия
                        auto *lineItem = addLine(QLineF(startCenter, endCenter), QPen(Qt::black));
                        lineItem->setData(0, startId);
                        lineItem->setData(1, endId);

                        // 2) Стрелка (полигон)
                        QLineF lineVector(startCenter, endCenter);
                        double angle = std::atan2(-lineVector.dy(), lineVector.dx());
                        QPointF arrowP1 = endCenter - QPointF(std::sin(angle - M_PI / 3) * 10,
                                                              std::cos(angle - M_PI / 3) * 10);
                        QPointF arrowP2 = endCenter - QPointF(std::sin(angle + M_PI / 3) * 10,
                                                              std::cos(angle + M_PI / 3) * 10);

                        auto *arrowItem = addPolygon(QPolygonF() << endCenter << arrowP1 << arrowP2,
                                                     QPen(Qt::black), QBrush(Qt::black));
                        arrowItem->setData(0, startId);
                        arrowItem->setData(1, endId);

                        // 3) Текст веса дуги с чёрным цветом
                        QPointF textPos = (startCenter + endCenter) / 2;
                        auto *weightText = addText(QString::number(weight));
                        weightText->setDefaultTextColor(Qt::black); // Устанавливаем цвет текста
                        weightText->setPos(textPos);
                        weightText->setData(0, startId);
                        weightText->setData(1, endId);
                    }
                    // Сбрасываем стартовую вершину
                    startVertex->setBrush(Qt::blue);
                    startVertex = nullptr;
                }
            }
        }
        else if (event->button() == Qt::RightButton) {
            // Выбор пары вершин правой кнопкой
            if (item && item->type() == QGraphicsEllipseItem::Type) {
                auto *vertex = qgraphicsitem_cast<QGraphicsEllipseItem *>(item);

                if (!selectedStartVertex) {
                    selectedStartVertex = vertex;
                    selectedStartVertex->setBrush(Qt::green);
                }
                else if (!selectedEndVertex) {
                    selectedEndVertex = vertex;
                    selectedEndVertex->setBrush(Qt::yellow);

                    // Генерируем сигнал
                    emit verticesSelected(selectedStartVertex->data(0).toInt(),
                                          selectedEndVertex->data(0).toInt());

                    // Сбрасываем
                    selectedStartVertex = nullptr;
                    selectedEndVertex   = nullptr;
                }
                else {
                    // Уже были выбраны 2 вершины — сбрасываем
                    selectedStartVertex->setBrush(Qt::blue);
                    selectedEndVertex->setBrush(Qt::blue);

                    selectedStartVertex = vertex;
                    selectedEndVertex   = nullptr;

                    selectedStartVertex->setBrush(Qt::green);
                }
            }
        }
    }

private:
    Graph<int> *graph;
    int currentVertexId;
    QGraphicsEllipseItem *startVertex;
    QGraphicsEllipseItem *selectedStartVertex;
    QGraphicsEllipseItem *selectedEndVertex;
};

// --------------------- MainWindow Class Implementation ---------------------

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QWidget(parent),
          graph(new Graph<int>()),
          scene(new GraphScene(graph, this)),
          selectedStartVertex(-1),
          selectedEndVertex(-1)
    {
        auto *layout = new QVBoxLayout(this);

        // QGraphicsView + сцена
        view = new QGraphicsView(scene, this);
        layout->addWidget(view);

        // (1) Белый фон
        scene->setBackgroundBrush(Qt::white);

        // (2) Кнопка "Очистить"
        auto *clearButton = new QPushButton("Очистить", this);
        layout->addWidget(clearButton);
        connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearScene);

        // (3) Кнопка "Алгоритм Дейкстры"
        auto *dijkstraButton = new QPushButton("Алгоритм Дейкстры", this);
        layout->addWidget(dijkstraButton);
        connect(dijkstraButton, &QPushButton::clicked, this, &MainWindow::runDijkstra);

        // (4) Кнопка "Загрузить граф"
        auto *loadGraphButton = new QPushButton("Загрузить граф", this);
        layout->addWidget(loadGraphButton);
        connect(loadGraphButton, &QPushButton::clicked, this, &MainWindow::loadGraphFromFile);

        // (5) Кнопка "Удалить вершину"
        auto *removeVertexButton = new QPushButton("Удалить вершину", this);
        layout->addWidget(removeVertexButton);
        connect(removeVertexButton, &QPushButton::clicked, this, &MainWindow::removeVertex);

        // Сигнал от сцены (выбор вершин правой кнопкой)
        connect(scene, &GraphScene::verticesSelected, this, [this](int start, int end) {
            this->selectedStartVertex = start;
            this->selectedEndVertex   = end;
        });

        setLayout(layout);
        setWindowTitle("Редактор графа");
        resize(800, 600);
    }

private slots:
    void removeVertex()
    {
        bool ok;
        int vertexId = QInputDialog::getInt(this,
                                            tr("Удалить вершину"),
                                            tr("ID вершины:"),
                                            0,    // Значение по умолчанию
                                            0,    // Мин
                                            9999, // Макс
                                            1,    // Шаг
                                            &ok);
        if (!ok) {
            return; // Пользователь отменил ввод
        }

        // (1) Удаляем вершину из логического графа
        graph->removeVertex(vertexId);

        // (2) Удаляем со сцены всё, что имеет data(0) == vertexId или data(1) == vertexId
        QList<QGraphicsItem*> toRemove;
        for (auto *it : scene->items()) {
            // Линия
            if (auto *line = qgraphicsitem_cast<QGraphicsLineItem*>(it)) {
                if (line->data(0).toInt() == vertexId || line->data(1).toInt() == vertexId) {
                    toRemove.push_back(line);
                }
            }
            // Эллипс (вершина)
            else if (auto *ell = qgraphicsitem_cast<QGraphicsEllipseItem*>(it)) {
                if (ell->data(0).toInt() == vertexId) {
                    toRemove.push_back(ell);
                }
            }
            // Полигон (стрелка)
            else if (auto *poly = qgraphicsitem_cast<QGraphicsPolygonItem*>(it)) {
                if (poly->data(0).toInt() == vertexId || poly->data(1).toInt() == vertexId) {
                    toRemove.push_back(poly);
                }
            }
            // Текст (вес или номер вершины)
            else if (auto *txt = qgraphicsitem_cast<QGraphicsTextItem*>(it)) {
                // **Изменено: Проверяем только data(0), так как номера вершин теперь имеют data(0)**
                if (txt->data(0).toInt() == vertexId || txt->data(1).toInt() == vertexId) {
                    toRemove.push_back(txt);
                }
            }
        }
        for (auto *item : toRemove) {
            scene->removeItem(item);
            delete item;
        }
    }

    void runDijkstra() {
        if (selectedStartVertex == -1 || selectedEndVertex == -1) {
            QMessageBox::warning(this, "Ошибка",
                                 "Выберите начальную и конечную вершины для поиска!");
            return;
        }

        // Вызываем dijkstraPath
        Path<int> item = graph->dijkstraPath(selectedStartVertex, selectedEndVertex);
        if (item.GetPath().get_size() < 2) {
            QMessageBox::information(this, "Результат", "Кратчайший путь не найден!");
            return;
        }

        // Подсвечиваем рёбра (линии, стрелки), у которых (data(0) == u, data(1) == v)
        for (size_t i = 0; i < item.GetPath().get_size() - 1; ++i) {
            int u = item.GetPath()[i];
            int v = item.GetPath()[i + 1];

            // Проходим по всем items
            for (auto *it : scene->items()) {
                // Для линий
                if (auto *line = qgraphicsitem_cast<QGraphicsLineItem*>(it)) {
                    if (line->data(0).toInt() == u && line->data(1).toInt() == v) {
                        line->setPen(QPen(Qt::red, 3));
                    }
                }
                // Для стрелки (полигон)
                else if (auto *poly = qgraphicsitem_cast<QGraphicsPolygonItem*>(it)) {
                    if (poly->data(0).toInt() == u && poly->data(1).toInt() == v) {
                        poly->setPen(QPen(Qt::red, 3));
                        poly->setBrush(QBrush(Qt::red));
                    }
                }
                // Текст можно не трогать, но при желании можно покрасить
            }
        }

        // Формируем строку пути
        QString pathStr = "Найден кратчайший путь: ";
        for (int i = 0; i < item.GetPath().get_size(); ++i) {
            pathStr += QString::number(item.GetPath()[i]);
            if (i != item.GetPath().get_size() - 1)
                pathStr += "->";
        }

        // Выводим сообщение с путём
        QMessageBox::information(this, "Результат", pathStr);
    }

    void clearScene() {
        scene->clear();
        // Пересоздаём граф
        graph = new Graph<int>();
        scene->setGraph(graph);

        selectedStartVertex = -1;
        selectedEndVertex   = -1;
        connect(scene, &GraphScene::verticesSelected, this, [this](int start, int end) {
            this->selectedStartVertex = start;
            this->selectedEndVertex   = end;
        });
    }

    void addArrowWithWeight(QPointF start, QPointF end, int weight) {
        // (используется при загрузке из файла)
        QLineF line(start, end);

        // 1) Линия
        auto *lineItem = scene->addLine(line, QPen(Qt::black));
        // Кто вызывает этот метод — пусть передаёт ID вершин,
        // тогда и тут можно setData(0, from), setData(1, to).
        // Покажем пример чуть ниже, где вы вызываете addArrowWithWeight.

        // 2) Стрелка
        double angle = std::atan2(-line.dy(), line.dx());
        QPointF arrowP1 = end - QPointF(std::sin(angle - M_PI / 3) * 10,
                                        std::cos(angle - M_PI / 3) * 10);
        QPointF arrowP2 = end - QPointF(std::sin(angle + M_PI / 3) * 10,
                                        std::cos(angle + M_PI / 3) * 10);
        auto *arrowItem = scene->addPolygon(QPolygonF() << end << arrowP1 << arrowP2,
                                            QPen(Qt::black), QBrush(Qt::black));

        // 3) Текст веса дуги с чёрным цветом
        QPointF textPos = (start + end) / 2 + QPointF(0, -10);
        auto *txt = scene->addText(QString::number(weight));
        txt->setDefaultTextColor(Qt::black); // Устанавливаем цвет текста
        txt->setPos(textPos);

        // **Устанавливаем данные для текстового элемента**
        txt->setData(0, -1); // -1 обозначает, что это текст веса, а не номер вершины
        // Можно использовать другой подход для идентификации, если необходимо
    }

    void loadGraphFromFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл графа", "", "Текстовые файлы (*.txt)");
        if (fileName.isEmpty()) {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл!");
            return;
        }

        QTextStream in(&file);

        // Первая строка — список вершин, разделённых запятой
        QString line = in.readLine();
        QStringList vertices = line.split(',');
        clearScene();

        // Располагаем вершины по окружности
        QVector<QGraphicsEllipseItem*> vertexItems;
        QVector<QPointF> vertexPositions;

        // 1) Добавляем вершины
        for (int i = 0; i < vertices.size(); ++i) {
            int vertexId = vertices[i].toInt();
            graph->addVertex(vertexId);

            double angle = i * (2 * M_PI / vertices.size());
            QPointF pos(300 + 200 * std::cos(angle), 300 + 200 * std::sin(angle));

            auto *ellipse = scene->addEllipse(pos.x() - 15, pos.y() - 15, 30, 30,
                                              QPen(Qt::black), QBrush(Qt::blue));
            ellipse->setData(0, vertexId);

            // Добавляем текст номера вершины с чёрным цветом
            auto *vertexLabel = scene->addText(QString::number(vertexId));
            vertexLabel->setDefaultTextColor(Qt::black); // Устанавливаем цвет текста
            vertexLabel->setPos(pos.x() - 5, pos.y() - 25);

            // **Добавляем данные для текстового элемента**
            vertexLabel->setData(0, vertexId); // Добавлено

            vertexItems.append(ellipse);
            vertexPositions.append(pos);
        }

        // 2) Рёбра
        while (!in.atEnd()) {
            line = in.readLine();
            QStringList edgeData = line.split(',');
            if (edgeData.size() != 3) {
                QMessageBox::warning(this, "Ошибка", "Неверный формат строки: " + line);
                continue;
            }

            int fromVertex = edgeData[0].toInt();
            int toVertex   = edgeData[1].toInt();
            int weight     = edgeData[2].toInt();

            // Добавляем в логический граф
            graph->addEdge(fromVertex, toVertex, weight);

            // Ищем позиции
            QPointF startPos, endPos;
            for (int i = 0; i < vertexItems.size(); ++i) {
                if (vertexItems[i]->data(0).toInt() == fromVertex) {
                    startPos = vertexPositions[i];
                }
                if (vertexItems[i]->data(0).toInt() == toVertex) {
                    endPos = vertexPositions[i];
                }
            }

            // Рисуем
            QLineF lineF(startPos, endPos);
            // 1) Линия
            auto *lineItem = scene->addLine(lineF, QPen(Qt::black));
            lineItem->setData(0, fromVertex);
            lineItem->setData(1, toVertex);

            // 2) Стрелка
            double angle = std::atan2(-lineF.dy(), lineF.dx());
            QPointF arrowP1 = endPos - QPointF(std::sin(angle - M_PI / 3) * 10,
                                              std::cos(angle - M_PI / 3) * 10);
            QPointF arrowP2 = endPos - QPointF(std::sin(angle + M_PI / 3) * 10,
                                              std::cos(angle + M_PI / 3) * 10);
            auto *arrowItem = scene->addPolygon(QPolygonF() << endPos << arrowP1 << arrowP2,
                                                QPen(Qt::black), QBrush(Qt::black));
            arrowItem->setData(0, fromVertex);
            arrowItem->setData(1, toVertex);

            // 3) Текст веса дуги с чёрным цветом
            QPointF textPos = (startPos + endPos) / 2 + QPointF(0, -10);
            auto *txt = scene->addText(QString::number(weight));
            txt->setDefaultTextColor(Qt::black); // Устанавливаем цвет текста
            txt->setPos(textPos);
            txt->setData(0, -1); // -1 обозначает, что это текст веса

            // Если нужно, можно добавить дополнительную логику для идентификации текста веса
        }

        file.close();
        scene->setGraph(graph);

        // Сигнал при выборе пар вершин (правой кнопкой)
        connect(scene, &GraphScene::verticesSelected, this, [this](int start, int end) {
            this->selectedStartVertex = start;
            this->selectedEndVertex   = end;
        });
    }

private:
    Graph<int> *graph;
    GraphScene *scene;
    QGraphicsView *view;
    int selectedStartVertex;
    int selectedEndVertex;
};

// --------------------- main() ---------------------
int main(int argc, char *argv[]) {
    // Запускаем ваши тесты (Dijkstra и т.д.)
    TestDijkstra();

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

//визуал
//удалять вершины
//граф - это НЕ Dictionary
//разобраться, как работает алгоритм Дейкстры
//добавить готовые Пресеты (выпадающий список)

#include "main.moc"
