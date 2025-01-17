#include "cantcolony.h"
#include "cmainwinwidget.h"

#include <QRandomGenerator>

constexpr const double ALPHA = 1.0; // Влияние феромонов
constexpr const double BETA = 2.0;  // Влияние расстояния
constexpr const double EVAPORATION_RATE = 0.5;
constexpr const double Q = 100.0; // Количество феромона, добавляемого на путь
constexpr const int NUM_ANTS = 10;
constexpr const int NUM_ITERATIONS = 100;


/**
 * @brief Крнструктор Меровьиной колонии
 * @param points - точки которые нужно пройти
 */
CAntColony::CAntColony(QVector<QPointF> points) : points(points)
{
    initializePheromones();
}

/**
 * @brief Запуск Алгоритма
 */
void CAntColony::run()
{
    for(int iteration = 0; iteration < NUM_ITERATIONS; ++iteration)
    {
        QVector<QVector<int>> antPaths(NUM_ANTS);
        QVector<double> pathCosts(NUM_ANTS, 0.0);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            antPaths[ant] = findPath();
            pathCosts[ant] = calculateCost(antPaths[ant]);
        }

        updatePheromones(antPaths, pathCosts);
        bestPath = findBestPath(antPaths, pathCosts);
    }
}

/**
 * @brief Инициализация феромонов
 */
void CAntColony::initializePheromones()
{
    QVector<QVector<double>> pheromones(NUM_POINTS, QVector<double>(NUM_POINTS, 1.0));
    m_vPheromones = pheromones;
}

/**
 * @brief Пойск пути
 * @return путь
 */
QVector<int> CAntColony::findPath()
{
    QVector<int> path;
    QVector<bool> visited(NUM_POINTS, false);
    int currentPoint = 0; // Начинаем с первой точки
    path.append(currentPoint);
    visited[currentPoint] = true;

    for(int i = 1; i < NUM_POINTS; ++i)
    {
        currentPoint = selectNextPoint(currentPoint, visited);
        path.append(currentPoint);
        visited[currentPoint] = true;
    }

    return path;
}

/**
 * @brief Дистанция между точками
 * @param a - первая точка
 * @param b - вторая точка
 * @return возвращает значение растояния
 */
double distance(const QPointF &a, const QPointF &b) {
    return sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2));
}

int CAntColony::selectNextPoint(int current, const QVector<bool> &visited)
{
    QVector<double> probabilities(NUM_POINTS, 0.0);
    double sum = 0.0;

    for (int next = 0; next < NUM_POINTS; ++next)
    {
        if(!visited[next])
        {
            probabilities[next] = pow(m_vPheromones[current][next], ALPHA) /
                                  pow(distance(points[current], points[next]), BETA);
            sum += probabilities[next];
        }
    }

    for(int next = 0; next < NUM_POINTS; ++next)
    {
        probabilities[next] /= sum;
    }

    double randomValue = QRandomGenerator::global()->generateDouble();
    double cumulativeProbability = 0.0;

    for(int next = 0; next < NUM_POINTS; ++next)
    {
        cumulativeProbability += probabilities[next];
        if(cumulativeProbability >= randomValue)
            return next;
    }

    return 0; // На всякий случай
}

/**
 * @brief Просчитать стоимость
 * @param path - путь
 * @return занчение стоимости
 */
double CAntColony::calculateCost(const QVector<int> &path)
{
    double cost = 0.0;
    for(int i = 0; i < path.size() - 1; ++i)
    {
        cost += distance(points[path[i]], points[path[i + 1]]);
    }

    // Замкнем путь
    cost += distance(points[path.back()], points[path.front()]);
    return cost * 10; // Умножаем на стоимость
}

/**
 * @brief Обновление феромонов
 * @param antPaths - пути муравьев
 * @param pathCosts - стоимость пути
 */
void CAntColony::updatePheromones(const QVector<QVector<int> > &antPaths, const QVector<double> &pathCosts)
{
    for(int i = 0; i < NUM_POINTS; ++i)
    {
        for(int j = 0; j < NUM_POINTS; ++j)
        {
            m_vPheromones[i][j] *= (1.0 - EVAPORATION_RATE);
        }
    }

    for(int ant = 0; ant < NUM_ANTS; ++ant)
    {
        double contribution = Q / pathCosts[ant];
        for(int i = 0; i < antPaths[ant].size() - 1; ++i)
        {
            int from = antPaths[ant][i];
            int to = antPaths[ant][i + 1];
            m_vPheromones[from][to] += contribution;
        }
    }
}

/**
 * @brief Поиск лучшего пути
 * @param antPaths - пути муравьев
 * @param pathCosts - стоимость пути
 * @return лучший путь
 */
QVector<int> CAntColony::findBestPath(const QVector<QVector<int> > &antPaths, const QVector<double> &pathCosts)
{
    double minCost = std::numeric_limits<double>::max();
    QVector<int> bestPath;

    for(int i = 0; i < NUM_ANTS; ++i)
    {
        if(pathCosts[i] < minCost)
        {
            minCost = pathCosts[i];
            bestPath = antPaths[i];
        }
    }

    return bestPath;
}
