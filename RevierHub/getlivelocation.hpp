#ifndef GETLIVELOCATION_HPP
#define GETLIVELOCATION_HPP

#include <QObject>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>

class GetLiveLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QGeoCoordinate coordinate
            READ coordinate
                NOTIFY locationChanged)

public:

    QGeoCoordinate coordinate() const
    {
        return m_currentCoordinate;
    }

    explicit GetLiveLocation(QObject *parent = nullptr);

    QGeoCoordinate currentCoordinate() const;

signals:
    void locationChanged(const QGeoCoordinate &coordinate);

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *m_source;
    QGeoCoordinate m_currentCoordinate;
};



#endif // GETLIVELOCATION_HPP
