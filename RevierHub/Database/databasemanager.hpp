#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QObject>
#include <QSqlDatabase>
#include <QGeoCoordinate>

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
    void addTrailPoint(int sessionId, const QGeoCoordinate &coord, double dwellSeconds = 0);

    // Cleanup
    void deleteOlderThan(int days);

    // returns the current session id, if id<0 an error occured or you did just not start a session...
    const int currentSessionId() const { return m_currentSession; }

    int getTrailPoints();

private:
    QSqlDatabase m_db;
    bool createTables();
    int m_currentSession = -1;
};

#endif // DATABASEMANAGER_HPP
