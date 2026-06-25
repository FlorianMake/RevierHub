#include "getlivelocation.hpp"
#include "Database/databasemanager.hpp"

GetLiveLocation::GetLiveLocation(DatabaseManager *dataBaseManager, int sessionId, QObject *parent)
    : QObject(parent)
    , m_dataBaseManager(dataBaseManager)
    , m_sessionId(sessionId)
{
    m_source = QGeoPositionInfoSource::createDefaultSource(this);

    if (m_source)
    {
        connect(
            m_source,
            &QGeoPositionInfoSource::positionUpdated,
            this,
            &GetLiveLocation::onPositionUpdated);

        m_source->setUpdateInterval(1000);
        m_source->startUpdates();
    }
}

void GetLiveLocation::onPositionUpdated(
    const QGeoPositionInfo &info)
{
    if (!info.isValid())
        return;

    auto coordinate = info.coordinate();

    if(checkMovementDistance(coordinate, m_currentCoordinate))
    {
        m_currentCoordinate = coordinate;
        m_dataBaseManager->addTrailPoint(m_sessionId, m_currentCoordinate);
        qDebug() << "add trailpoint";
        emit locationChanged(m_currentCoordinate);
    }
    if(checkMovementDistance(coordinate, m_currentCenter, 10.0))
    {
        m_currentCenter = coordinate;
        emit centerChanged(m_currentCenter);
    }
}

bool GetLiveLocation::checkMovementDistance(QGeoCoordinate coordinateA, QGeoCoordinate coordinateB, double distance)
{
    double value = coordinateA.distanceTo(coordinateB);
    if(value > distance || !m_currentCenter.isValid())
    {
        return true;
    }

    return false;
}