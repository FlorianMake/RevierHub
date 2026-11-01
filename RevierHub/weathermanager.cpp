#include "weathermanager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopeGuard>
#include <QTimer>
#include <QUrlQuery>

namespace {
constexpr int weatherRefreshIntervalMs = 10 * 60 * 1000;
}

WeatherManager::WeatherManager(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setInterval(weatherRefreshIntervalMs);
    connect(m_refreshTimer, &QTimer::timeout, this, &WeatherManager::fetchWeather);
}

bool WeatherManager::available() const
{
    return m_available;
}

double WeatherManager::windSpeed() const
{
    return m_windSpeed;
}

double WeatherManager::windDirection() const
{
    return m_windDirection;
}

int WeatherManager::precipitationProbability() const
{
    return m_precipitationProbability;
}

double WeatherManager::precipitation() const
{
    return m_precipitation;
}

void WeatherManager::updateLocation(const QGeoCoordinate &coordinate)
{
    if (!coordinate.isValid())
        return;

    m_coordinate = coordinate;
    if (!m_refreshTimer->isActive()) {
        fetchWeather();
        m_refreshTimer->start();
    }
}

void WeatherManager::fetchWeather()
{
    if (!m_coordinate.isValid())
        return;

    QUrl url("https://api.open-meteo.com/v1/forecast");
    QUrlQuery query;
    query.addQueryItem("latitude", QString::number(m_coordinate.latitude(), 'f', 6));
    query.addQueryItem("longitude", QString::number(m_coordinate.longitude(), 'f', 6));
    query.addQueryItem("current",
                       "wind_speed_10m,wind_direction_10m,precipitation");
    query.addQueryItem("hourly", "precipitation_probability");
    query.addQueryItem("forecast_days", "1");
    query.addQueryItem("timezone", "auto");
    url.setQuery(query);

    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const auto cleanupReply = qScopeGuard([reply]() { reply->deleteLater(); });
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Weather request failed:" << reply->errorString();
            return;
        }

        const QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        const QJsonObject root = document.object();
        const QJsonObject current = root.value("current").toObject();
        const QJsonObject hourly = root.value("hourly").toObject();
        const QJsonArray hourlyTimes = hourly.value("time").toArray();
        const QJsonArray hourlyProbabilities =
            hourly.value("precipitation_probability").toArray();

        if (current.isEmpty() || hourlyTimes.size() != hourlyProbabilities.size()) {
            qWarning() << "Weather response did not contain the expected data";
            return;
        }

        const QString currentTime = current.value("time").toString();
        int timeIndex = -1;
        for (int index = 0; index < hourlyTimes.size(); ++index) {
            if (hourlyTimes.at(index).toString() <= currentTime) {
                timeIndex = index;
            }
        }
        if (timeIndex < 0) {
            qWarning() << "Weather response did not contain a matching hourly forecast";
            return;
        }

        m_windSpeed = current.value("wind_speed_10m").toDouble();
        m_windDirection = current.value("wind_direction_10m").toDouble();
        m_precipitation = current.value("precipitation").toDouble();
        m_precipitationProbability = hourlyProbabilities.at(timeIndex).toInt();
        m_available = true;
        emit weatherChanged();
    });
}
