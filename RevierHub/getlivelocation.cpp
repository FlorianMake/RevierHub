#include "getlivelocation.hpp"

#include <cmath>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniObject>
#include <QPointer>
#endif

namespace {
constexpr double maximumTrailPointAccuracyMeters = 25.0;
constexpr int requiredInitialAccurateFixes = 3;

#ifdef Q_OS_ANDROID
QPointer<GetLiveLocation> liveLocationInstance;
#endif
}

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

#ifdef Q_OS_ANDROID
    liveLocationInstance = this;
    QJniObject::callStaticMethod<void>(
        "com/revierhub/app/OrientationProvider",
        "start",
        "(Landroid/content/Context;)V",
        QNativeInterface::QAndroidApplication::context());
#endif
}

void GetLiveLocation::onPositionUpdated(
    const QGeoPositionInfo &info)
{
    if (!info.isValid())
        return;

    m_currentCoordinate = info.coordinate();

#ifndef Q_OS_ANDROID
    if (info.hasAttribute(QGeoPositionInfo::Direction)
        && std::isfinite(info.attribute(QGeoPositionInfo::Direction))) {
        m_heading = info.attribute(QGeoPositionInfo::Direction);
    } else {
        m_heading = std::numeric_limits<double>::quiet_NaN();
    }
#endif

    if (!info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)
        || info.attribute(QGeoPositionInfo::HorizontalAccuracy)
            > maximumTrailPointAccuracyMeters) {
        m_accurateFixes = 0;
    } else {
        if (m_accurateFixes < requiredInitialAccurateFixes)
            ++m_accurateFixes;

        if (m_accurateFixes == requiredInitialAccurateFixes) {
            m_hasGpsLock = true;
            emit acceptedPositionChanged(
                m_currentCoordinate,
                info.attribute(QGeoPositionInfo::HorizontalAccuracy));
        }
    }

    emit locationChanged(m_currentCoordinate);

    if(shallRecenter()) {
        m_currentCenter = info.coordinate();
        emit centerChanged(m_currentCenter);
    }
}

void GetLiveLocation::updateHeading(double heading)
{
    m_heading = heading;
    emit locationChanged(m_currentCoordinate);
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

#ifdef Q_OS_ANDROID
extern "C" JNIEXPORT void JNICALL
Java_com_revierhub_app_OrientationProvider_onHeadingChanged(
    JNIEnv *,
    jclass,
    jdouble heading)
{
    if (!liveLocationInstance)
        return;

    QMetaObject::invokeMethod(
        liveLocationInstance,
        "updateHeading",
        Qt::QueuedConnection,
        Q_ARG(double, static_cast<double>(heading)));
}
#endif