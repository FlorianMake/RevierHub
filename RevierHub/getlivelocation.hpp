#ifndef GETLIVELOCATION_HPP
#define GETLIVELOCATION_HPP

#include <QObject>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>

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

    explicit GetLiveLocation(QObject *parent = nullptr);

    QGeoCoordinate currentCoordinate() const;
    QGeoCoordinate currentCenter() const;

signals:
    void locationChanged(const QGeoCoordinate &coordinate);
    void centerChanged(const QGeoCoordinate &coordinate);

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);

private:
    bool shallRecenter();


    QGeoPositionInfoSource *m_source;
    QGeoCoordinate m_currentCoordinate;
    QGeoCoordinate m_currentCenter;
};



#endif // GETLIVELOCATION_HPP
