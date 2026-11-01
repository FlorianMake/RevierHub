#ifndef WEATHER_MANAGER_HPP
#define WEATHER_MANAGER_HPP

#include <QObject>
#include <QGeoCoordinate>

class QNetworkAccessManager;
class QTimer;

class WeatherManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool available READ available NOTIFY weatherChanged)
    Q_PROPERTY(double windSpeed READ windSpeed NOTIFY weatherChanged)
    Q_PROPERTY(double windDirection READ windDirection NOTIFY weatherChanged)
    Q_PROPERTY(int precipitationProbability READ precipitationProbability NOTIFY weatherChanged)
    Q_PROPERTY(double precipitation READ precipitation NOTIFY weatherChanged)

public:
    explicit WeatherManager(QObject *parent = nullptr);

    bool available() const;
    double windSpeed() const;
    double windDirection() const;
    int precipitationProbability() const;
    double precipitation() const;

public slots:
    void updateLocation(const QGeoCoordinate &coordinate);

signals:
    void weatherChanged();

private:
    void fetchWeather();

    QNetworkAccessManager *m_networkManager;
    QTimer *m_refreshTimer;
    QGeoCoordinate m_coordinate;
    bool m_available = false;
    double m_windSpeed = 0.0;
    double m_windDirection = 0.0;
    int m_precipitationProbability = 0;
    double m_precipitation = 0.0;
};

#endif // WEATHER_MANAGER_HPP
