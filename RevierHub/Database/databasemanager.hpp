#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QObject>
#include <QSqlDatabase>
#include <QGeoCoordinate>
#include <QVariantList>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool init();

    // Sessions
    int startSession();
    void endSession(int sessionId);

    // Trail points
    void addTrailPoint(int sessionId, const QGeoCoordinate &coord,
                       double horizontalAccuracy, double dwellSeconds = 0);

    // Cleanup
    void deleteOlderThan(int days);

    // returns the current session id, if id<0 an error occured or you did just not start a session...
    const int currentSessionId() const { return m_currentSession; }

    int getTrailPoints();
    QList<QGeoCoordinate> getTrailPoints(int sessionId);

    Q_INVOKABLE QVariantList huntingAreas() const;
    Q_INVOKABLE int createHuntingArea(const QString &name, const QVariantList &points);
    Q_INVOKABLE int createHuntingAreaDraft(const QString &name);
    Q_INVOKABLE bool updateHuntingAreaDraftVertices(int areaId,
                                                    const QVariantList &points);
    Q_INVOKABLE bool finishHuntingAreaDraft(int areaId, const QString &name,
                                            const QVariantList &points);
    Q_INVOKABLE bool updateHuntingAreaVertices(int areaId, const QVariantList &points);
    Q_INVOKABLE bool deleteHuntingArea(int areaId);

signals:
    void huntingAreasChanged();

private:
    QSqlDatabase m_db;
    bool createTables();
    bool verticesFromVariantList(const QVariantList &values,
                                 QList<QGeoCoordinate> *points) const;
    int m_currentSession = -1;
};

#endif // DATABASEMANAGER_HPP
