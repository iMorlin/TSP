#pragma once

#include <QPointF>
#include <QVector>

class CAntColony
{
public:
    CAntColony(QVector<QPointF> points);
    void run(int userIndex);

    const QVector<int> GetBestPath()
        { return bestPath; }

private:
    void initializePheromones();

    QVector<int> findPath(int userIndex);
    int selectNextPoint(int current, const QVector<bool> &visited);
    double calculateCost(const QVector<int> &path);
    void updatePheromones(const QVector<QVector<int>> &antPaths, const QVector<double> &pathCosts);
    QVector<int> findBestPath(const QVector<QVector<int>> &antPaths, const QVector<double> &pathCosts);

private:
    int startIndex;
    QVector<QPointF> points;
    QVector<QVector<double>> m_vPheromones;
    QVector<int> bestPath;
};

