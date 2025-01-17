#pragma once

#include <QWidget>

constexpr const int NUM_POINTS      = 100;
constexpr const int RADIUS          = 400;
constexpr const int MIN_CONNECTIONS = 2;
constexpr const int MAX_CONNECTIONS = 6;
constexpr const int PRICE           = 10;

namespace Ui {
class CMainWinWidget;
}

class CMainWinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CMainWinWidget(QWidget *parent = nullptr);
    ~CMainWinWidget();

protected:
     void paintEvent(QPaintEvent *event) override;
     void mousePressEvent(QMouseEvent *event) override;

private:
    void generatePoints();
    void connectPoints();
    double calculateCost();

private:
    Ui::CMainWinWidget *ui;
    int userIndex = -1;

    QVector<QPointF> points;        // список точек
    QVector<QVector<int>> adjList;  // Список смежности
    QVector<int> path;              // Путь от первой точки к заданной
};
