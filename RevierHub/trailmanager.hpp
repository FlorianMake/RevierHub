#ifndef TRAILMANAGER_HPP
#define TRAILMANAGER_HPP

#include <QObject>
#include <QStandardPaths>
#include <QDateTime>
#include <QList>
#include <QGeoCoordinate>

#include <QDebug>


class DatabaseManager;

class TrailManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList currentTrail READ currentTrail NOTIFY trailChanged)
    Q_PROPERTY(int trailLength READ trailLength NOTIFY trailChanged)

public:
    TrailManager(int sessionId);

    QVariantList currentTrail() const;
    int trailLength() const;

public slots:
    void getPoints();

signals:
    void trailChanged();

private:
    int m_sessionId;
    void setPoints(QVariantList pts);

    QVariantList m_points;
    DatabaseManager* m_database;
};

#endif // TRAILMANAGER_HPP
