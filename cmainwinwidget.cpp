#include "cmainwinwidget.h"
#include "ui_cmainwinwidget.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QPointF>
#include <QPainter>
#include <QRandomGenerator64>
#include <QtMath>

#include "cantcolony.h"

/**
 * @brief Конструктор основного окна
 * @param parent
 */
CMainWinWidget::CMainWinWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMainWinWidget)
{
    ui->setupUi(this);

    generatePoints();
    connectPoints();
}

/**
 * @brief Диструктор
 */
CMainWinWidget::~CMainWinWidget()
{
    delete ui;
}

/**
 * @brief Событие рисования
 * @param event
 */
void CMainWinWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Отрисовка круга
    painter.drawEllipse(800/2 - RADIUS, 800/2 - RADIUS, RADIUS * 2, RADIUS * 2);

    // Отрисовка точек
    for(const auto &point : points)
    {
        painter.drawEllipse(point, 5, 5); // Рисуем точки радиусом 5
    }

    // Отрисовка соединений
    painter.setPen(Qt::blue);
    for(const auto &connections : adjList)
    {
        for(int neighbor : connections)
        {
            painter.drawLine(points[&connections - &adjList[0]], points[neighbor]);
        }
    }

    // Отрисовка пути
    if(userIndex != -1)
    {
        painter.setPen(Qt::red);
        for(size_t i = 0; i < path.size() - 1; ++i)
        {
            painter.drawLine(points[path[i]], points[path[i + 1]]);
        }

        // Расчет и отображение стоимости
        double cost = calculateCost();
        painter.drawText(10, 20, QString("Стоимость пути: %1 USD").arg(cost));
    }
}

/**
 * @brief Событие нажатия мышки
 * @param event
 */
void CMainWinWidget::mousePressEvent(QMouseEvent *event)
{
    // Получаем координаты клика мыши
    QPointF clickPos = event->pos();
    for(int i = 0; i < points.size(); ++i)
    {
        if(QLineF(points[i], clickPos).length() <= 5)
        {
            // Проверяем, попали ли в точку
            userIndex = i;

            auto element = points[userIndex];
            points.erase(points.begin() + userIndex);
            points.insert(0, element);

            CAntColony colony(points);
            colony.run();
            path = colony.GetBestPath();
            update(); // Обновляем виджет для отображения пути
            return;
        }
    }
    QMessageBox::information(this, "Ошибка", "Кликните на точку, чтобы задать точку назначения.");
}

/**
 * @brief Сгенерировать точки
 */
void CMainWinWidget::generatePoints()
{
    for(int i = 0; i < NUM_POINTS; ++i)
    {
        double angle = QRandomGenerator64::global()->bounded(0, RADIUS * 2 * M_PI);
        double r = QRandomGenerator64::global()->bounded(0, RADIUS - 10);
        QPointF point(RADIUS - r * cos(angle), RADIUS - r * sin(angle));
        points.append(point);
    }
}

/**
 * @brief СОЕДИНИТЬ ТОЧКИ
 */
void CMainWinWidget::connectPoints()
{
    adjList.resize(points.size());

    for(int i = 0; i < points.size(); ++i)
    {
        int numConnections = QRandomGenerator64::global()->bounded(MIN_CONNECTIONS, MAX_CONNECTIONS + 1);
        QVector<int> neighbors;

        // Получаем соседние индексы
        for(int j = 1; j <= numConnections / 2; ++j)
        {
            neighbors.append((i + j) % points.size());
            neighbors.append((i - j + points.size()) % points.size());
        }

        // Удаляем дубликаты и сохраняем в список смежности
        std::sort(neighbors.begin(), neighbors.end());
        neighbors.erase(std::unique(neighbors.begin(), neighbors.end()), neighbors.end());
        adjList[i] = neighbors;
    }
}

/**
 * @brief Рассчитать стоимость
 * @return
 */
double CMainWinWidget::calculateCost()
{
    double totalDistance = 0.0;
    for(size_t i = 0; i < path.size() - 1; ++i)
    {
        totalDistance += QLineF(points[path[i]], points[path[i + 1]]).length();
    }
    return totalDistance * PRICE; // 10 USD за единицу расстояния
}
