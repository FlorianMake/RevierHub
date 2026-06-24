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

    if(shallRecenter()) {
        m_currentCenter = info.coordinate();
        emit centerChanged(m_currentCenter);
    }
}

QGeoCoordinate GetLiveLocation::currentCoordinate() const
{
    return m_currentCoordinate;
}

QGeoCoordinate GetLiveLocation::currentCenter() const
{
    return m_currentCenter;
}

bool GetLiveLocation::shallRecenter()
{
    double value = m_currentCenter.distanceTo(m_currentCoordinate);

    if(value > 10.00 || !m_currentCenter.isValid()) {
        return true;
    }

    return false;
}