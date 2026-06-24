#ifndef GETLIVELOCATION_HPP
#define GETLIVELOCATION_HPP

#include <QObject>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>

class DatabaseManager;

class GetLiveLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate NOTIFY locationChanged)
    Q_PROPERTY(QGeoCoordinate newCenter READ newCenter NOTIFY centerChanged)

public:

    QGeoCoordinate coordinate() const
    {
        return m_currentCoordinate;
    }

    QGeoCoordinate newCenter() const
    {
        return m_currentCenter;
    }

    explicit GetLiveLocation(DatabaseManager* dataBaseManager, int sessionId, QObject *parent = nullptr);

signals:
    void locationChanged(const QGeoCoordinate &coordinate);
    void centerChanged(const QGeoCoordinate &coordinate);

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);

private:
    bool checkMovementDistance(double distance);
    int m_sessionId;
    DatabaseManager* m_dataBaseManager;


    QGeoPositionInfoSource *m_source;
    QGeoCoordinate m_currentCoordinate;
    QGeoCoordinate m_currentCenter;
};



#endif // GETLIVELOCATION_HPP
