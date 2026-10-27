#include "trailmanager.hpp"

#include <QTimer>

#include "Database/databasemanager.hpp"

TrailManager::TrailManager(DatabaseManager *database, int sessionId)
    : m_sessionId(sessionId),
      m_database(database)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TrailManager::getPoints);
    timer->start(10 * 1000);   // interval in milliseconds

    getPoints();
}


QVariantList TrailManager::currentTrail() const
{
    qDebug() << "still points left? :" << m_points.length();
    return m_points;
}

int TrailManager::trailLength() const
{
    return m_points.length();
}

void TrailManager::getPoints()
{
    auto coordinates = m_database->getTrailPoints( m_sessionId );

    QVariantList list;
    for (const auto &coord : coordinates)
        list << QVariant::fromValue(coord);

    setPoints(list);
}

void TrailManager::setPoints(QVariantList pts) {
    m_points = pts;
    emit trailChanged();
}
