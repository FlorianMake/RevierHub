#include "getlivelocation.hpp"

GetLiveLocation::GetLiveLocation(QObject *parent)
    : QObject(parent)
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

    m_currentCoordinate = info.coordinate();

    emit locationChanged(m_currentCoordinate);
}

QGeoCoordinate GetLiveLocation::currentCoordinate() const
{
    return m_currentCoordinate;
}