#ifndef GETLIVELOCATION_HPP
#define GETLIVELOCATION_HPP

#include <QObject>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <limits>

class GetLiveLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate NOTIFY locationChanged)
    Q_PROPERTY(QGeoCoordinate newCenter READ newCenter NOTIFY centerChanged)
    Q_PROPERTY(double heading READ heading NOTIFY locationChanged)
    Q_PROPERTY(bool hasGpsLock READ hasGpsLock NOTIFY locationChanged)

public:

    QGeoCoordinate coordinate() const
    {
        return m_currentCoordinate;
    }

    QGeoCoordinate newCenter() const
    {
        return m_currentCenter;
    }

    double heading() const
    {
        return m_heading;
    }

    bool hasGpsLock() const
    {
        return m_hasGpsLock;
    }

    explicit GetLiveLocation(QObject *parent = nullptr);

    QGeoCoordinate currentCoordinate() const;
    QGeoCoordinate currentCenter() const;

signals:
    void locationChanged(const QGeoCoordinate &coordinate);
    void acceptedPositionChanged(const QGeoCoordinate &coordinate,
                                 double horizontalAccuracy);
    void centerChanged(const QGeoCoordinate &coordinate);

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);
    void updateHeading(double heading);

private:
    bool shallRecenter();


    QGeoPositionInfoSource *m_source;
    QGeoCoordinate m_currentCoordinate;
    QGeoCoordinate m_currentCenter;
    double m_heading = std::numeric_limits<double>::quiet_NaN();
    bool m_hasGpsLock = false;
    int m_accurateFixes = 0;
};



#endif // GETLIVELOCATION_HPP
