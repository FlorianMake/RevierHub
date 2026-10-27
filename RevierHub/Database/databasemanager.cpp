#include "databasemanager.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDateTime>

#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::init()
{
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/revierhub.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "DB open failed:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "DB opened at:" << dbPath;
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery q;

    q.exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            started_at DATETIME NOT NULL,
            ended_at DATETIME
        )
    )");

    q.exec(R"(
        CREATE TABLE IF NOT EXISTS trail_points (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id INTEGER NOT NULL,
            lat REAL NOT NULL,
            lon REAL NOT NULL,
            timestamp DATETIME NOT NULL,
            dwell_seconds REAL DEFAULT 0,
            FOREIGN KEY(session_id) REFERENCES sessions(id)
        )
    )");

    // here would be the place to add new version dependent changes.
    // If we update the database structure for example, we would need to add them as a UPDATE.
    // Because some previous versions would not contain all table data we need.
    // This sounds already like a big mess ... lets think about more information we can add to the tables
    // to have all the information already when we would need them. That's a big todo before first rollout...

    return true;
}

int DatabaseManager::startSession()
{
    QSqlQuery q;
    q.prepare("INSERT INTO sessions (started_at) VALUES (?)");
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.exec();
    m_currentSession = q.lastInsertId().toInt();
    return m_currentSession;
}

void DatabaseManager::endSession(int sessionId)
{
    QSqlQuery q;
    q.prepare("UPDATE sessions SET ended_at = ? WHERE id = ?");
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(sessionId);
    q.exec();
}

void DatabaseManager::addTrailPoint(int sessionId, const QGeoCoordinate &coord, double dwellSeconds)
{
    QSqlQuery q;
    q.prepare("INSERT INTO trail_points (session_id, lat, lon, timestamp, dwell_seconds) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(sessionId);
    q.addBindValue(coord.latitude());
    q.addBindValue(coord.longitude());
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(dwellSeconds);
    q.exec();
}

void DatabaseManager::deleteOlderThan(int days)
{
    QSqlQuery q;
    q.prepare("DELETE FROM trail_points WHERE timestamp < datetime('now', ? || ' days')");
    q.addBindValue(QString("-%1").arg(days));
    q.exec();

    // Clean up orphaned sessions
    q.exec("DELETE FROM sessions WHERE id NOT IN "
           "(SELECT DISTINCT session_id FROM trail_points)");
}

int DatabaseManager::getTrailPoints() {
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM trail_points");
    if (!q.exec())
        return -1;

    if (q.next())
        return q.value(0).toInt();

    return -1;
}


QList<QGeoCoordinate> DatabaseManager::getTrailPoints(int sessionId)
{
    QList<QGeoCoordinate> points;
    QSqlQuery q;

    q.prepare("SELECT lat, lon FROM trail_points WHERE session_id = ? ORDER BY timestamp ASC");
    q.addBindValue(sessionId);

    q.exec();
    while (q.next()) {
        points.append(QGeoCoordinate(q.value(0).toDouble(), q.value(1).toDouble()));
    }
    return points;
}
