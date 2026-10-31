#include "databasemanager.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDateTime>
#include <QVariantMap>

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

    if (!q.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Failed to enable foreign keys:" << q.lastError().text();
        return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            started_at DATETIME NOT NULL,
            ended_at DATETIME
        )
    )")) {
        qWarning() << "Failed to create sessions table:" << q.lastError().text();
        return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS trail_points (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id INTEGER NOT NULL,
            lat REAL NOT NULL,
            lon REAL NOT NULL,
            timestamp DATETIME NOT NULL,
            dwell_seconds REAL DEFAULT 0,
            horizontal_accuracy REAL,
            FOREIGN KEY(session_id) REFERENCES sessions(id)
        )
    )")) {
        qWarning() << "Failed to create trail points table:" << q.lastError().text();
        return false;
    }

    QSqlQuery tableInfo;
    if (!tableInfo.exec("PRAGMA table_info(trail_points)")) {
        qWarning() << "Failed to inspect trail point schema:" << tableInfo.lastError().text();
        return false;
    }
    bool hasHorizontalAccuracy = false;
    while (tableInfo.next()) {
        if (tableInfo.value(1).toString() == "horizontal_accuracy") {
            hasHorizontalAccuracy = true;
            break;
        }
    }

    if (!hasHorizontalAccuracy
        && !q.exec("ALTER TABLE trail_points ADD COLUMN horizontal_accuracy REAL")) {
        qWarning() << "Failed to add trail point accuracy:" << q.lastError().text();
        return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS hunting_areas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at DATETIME NOT NULL,
            is_draft INTEGER NOT NULL DEFAULT 0
        )
    )")) {
        qWarning() << "Failed to create hunting areas table:" << q.lastError().text();
        return false;
    }

    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS hunting_area_vertices (
            area_id INTEGER NOT NULL,
            vertex_index INTEGER NOT NULL,
            lat REAL NOT NULL,
            lon REAL NOT NULL,
            PRIMARY KEY (area_id, vertex_index),
            FOREIGN KEY(area_id) REFERENCES hunting_areas(id) ON DELETE CASCADE
        )
    )")) {
        qWarning() << "Failed to create hunting area vertices table:" << q.lastError().text();
        return false;
    }

    QSqlQuery areaTableInfo(m_db);
    if (!areaTableInfo.exec("PRAGMA table_info(hunting_areas)")) {
        qWarning() << "Failed to inspect hunting area schema:"
                   << areaTableInfo.lastError().text();
        return false;
    }
    bool hasDraftState = false;
    while (areaTableInfo.next()) {
        if (areaTableInfo.value(1).toString() == "is_draft") {
            hasDraftState = true;
            break;
        }
    }
    if (!hasDraftState
        && !q.exec("ALTER TABLE hunting_areas "
                   "ADD COLUMN is_draft INTEGER NOT NULL DEFAULT 0")) {
        qWarning() << "Failed to add hunting area draft state:" << q.lastError().text();
        return false;
    }

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

void DatabaseManager::addTrailPoint(int sessionId, const QGeoCoordinate &coord,
                                    double horizontalAccuracy, double dwellSeconds)
{
    QSqlQuery q;
    q.prepare("INSERT INTO trail_points "
              "(session_id, lat, lon, timestamp, dwell_seconds, horizontal_accuracy) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(sessionId);
    q.addBindValue(coord.latitude());
    q.addBindValue(coord.longitude());
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(dwellSeconds);
    q.addBindValue(horizontalAccuracy);
    if (!q.exec())
        qWarning() << "Failed to add trail point:" << q.lastError().text();
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

bool DatabaseManager::verticesFromVariantList(const QVariantList &values,
                                              QList<QGeoCoordinate> *points) const
{
    points->clear();
    for (const QVariant &value : values) {
        if (!value.canConvert<QGeoCoordinate>()) {
            qWarning() << "Invalid hunting area vertex";
            return false;
        }

        const QGeoCoordinate coordinate = value.value<QGeoCoordinate>();
        if (!coordinate.isValid()) {
            qWarning() << "Invalid hunting area coordinate";
            return false;
        }
        points->append(coordinate);
    }

    return true;
}

QVariantList DatabaseManager::huntingAreas() const
{
    QVariantList areas;
    QSqlQuery areasQuery(m_db);
    if (!areasQuery.exec("SELECT id, name, is_draft FROM hunting_areas ORDER BY id")) {
        qWarning() << "Failed to load hunting areas:" << areasQuery.lastError().text();
        return areas;
    }

    QSqlQuery verticesQuery(m_db);
    verticesQuery.prepare("SELECT lat, lon FROM hunting_area_vertices "
                          "WHERE area_id = ? ORDER BY vertex_index");
    while (areasQuery.next()) {
        QVariantList points;
        verticesQuery.bindValue(0, areasQuery.value(0));
        if (!verticesQuery.exec()) {
            qWarning() << "Failed to load hunting area vertices:"
                       << verticesQuery.lastError().text();
            continue;
        }
        while (verticesQuery.next())
            points.append(QVariant::fromValue(QGeoCoordinate(
                verticesQuery.value(0).toDouble(), verticesQuery.value(1).toDouble())));

        QVariantMap area;
        area.insert("id", areasQuery.value(0).toInt());
        area.insert("name", areasQuery.value(1).toString());
        area.insert("isDraft", areasQuery.value(2).toBool());
        area.insert("points", points);
        areas.append(area);
    }
    return areas;
}

int DatabaseManager::createHuntingArea(const QString &name, const QVariantList &values)
{
    QList<QGeoCoordinate> points;
    if (!verticesFromVariantList(values, &points) || points.size() < 3) {
        qWarning() << "A hunting area requires at least three vertices";
        return -1;
    }

    const QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << "A hunting area requires a name";
        return -1;
    }
    if (!m_db.transaction()) {
        qWarning() << "Failed to begin hunting area transaction:" << m_db.lastError().text();
        return -1;
    }

    QSqlQuery areaQuery(m_db);
    areaQuery.prepare("INSERT INTO hunting_areas (name, created_at) VALUES (?, ?)");
    areaQuery.addBindValue(trimmedName);
    areaQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!areaQuery.exec()) {
        qWarning() << "Failed to create hunting area:" << areaQuery.lastError().text();
        m_db.rollback();
        return -1;
    }
    const int areaId = areaQuery.lastInsertId().toInt();
    QSqlQuery vertexQuery(m_db);
    vertexQuery.prepare("INSERT INTO hunting_area_vertices "
                        "(area_id, vertex_index, lat, lon) VALUES (?, ?, ?, ?)");
    for (qsizetype index = 0; index < points.size(); ++index) {
        vertexQuery.bindValue(0, areaId);
        vertexQuery.bindValue(1, index);
        vertexQuery.bindValue(2, points.at(index).latitude());
        vertexQuery.bindValue(3, points.at(index).longitude());
        if (!vertexQuery.exec()) {
            qWarning() << "Failed to create hunting area vertex:"
                       << vertexQuery.lastError().text();
            m_db.rollback();
            return -1;
        }
    }
    if (!m_db.commit()) {
        qWarning() << "Failed to commit hunting area:" << m_db.lastError().text();
        return -1;
    }
    emit huntingAreasChanged();
    return areaId;
}

int DatabaseManager::createHuntingAreaDraft(const QString &name)
{
    const QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << "A hunting area draft requires a name";
        return -1;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO hunting_areas (name, created_at, is_draft) "
                  "VALUES (?, ?, 1)");
    query.addBindValue(trimmedName);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!query.exec()) {
        qWarning() << "Failed to create hunting area draft:" << query.lastError().text();
        return -1;
    }

    const int areaId = query.lastInsertId().toInt();
    emit huntingAreasChanged();
    return areaId;
}

bool DatabaseManager::updateHuntingAreaDraftVertices(int areaId,
                                                     const QVariantList &values)
{
    QList<QGeoCoordinate> points;
    if (areaId < 1 || !verticesFromVariantList(values, &points))
        return false;
    if (!m_db.transaction()) {
        qWarning() << "Failed to begin hunting area draft update:"
                   << m_db.lastError().text();
        return false;
    }

    QSqlQuery draftQuery(m_db);
    draftQuery.prepare("SELECT 1 FROM hunting_areas WHERE id = ? AND is_draft = 1");
    draftQuery.addBindValue(areaId);
    if (!draftQuery.exec() || !draftQuery.next()) {
        qWarning() << "Hunting area draft does not exist:" << areaId;
        m_db.rollback();
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM hunting_area_vertices WHERE area_id = ?");
    query.addBindValue(areaId);
    if (!query.exec()) {
        qWarning() << "Failed to remove hunting area draft vertices:"
                   << query.lastError().text();
        m_db.rollback();
        return false;
    }

    query.prepare("INSERT INTO hunting_area_vertices "
                  "(area_id, vertex_index, lat, lon) VALUES (?, ?, ?, ?)");
    for (qsizetype index = 0; index < points.size(); ++index) {
        query.bindValue(0, areaId);
        query.bindValue(1, index);
        query.bindValue(2, points.at(index).latitude());
        query.bindValue(3, points.at(index).longitude());
        if (!query.exec()) {
            qWarning() << "Failed to update hunting area draft vertex:"
                       << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }
    if (!m_db.commit()) {
        qWarning() << "Failed to commit hunting area draft:" << m_db.lastError().text();
        return false;
    }
    emit huntingAreasChanged();
    return true;
}

bool DatabaseManager::finishHuntingAreaDraft(int areaId, const QString &name,
                                             const QVariantList &values)
{
    QList<QGeoCoordinate> points;
    const QString trimmedName = name.trimmed();
    if (areaId < 1 || trimmedName.isEmpty() || !verticesFromVariantList(values, &points)
        || points.size() < 3) {
        qWarning() << "A finished hunting area requires a name and three vertices";
        return false;
    }
    if (!updateHuntingAreaDraftVertices(areaId, values))
        return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE hunting_areas SET name = ?, is_draft = 0 "
                  "WHERE id = ? AND is_draft = 1");
    query.addBindValue(trimmedName);
    query.addBindValue(areaId);
    if (!query.exec() || query.numRowsAffected() != 1) {
        qWarning() << "Failed to finish hunting area draft:" << query.lastError().text();
        return false;
    }
    emit huntingAreasChanged();
    return true;
}

bool DatabaseManager::updateHuntingAreaVertices(int areaId, const QVariantList &values)
{
    QList<QGeoCoordinate> points;
    if (areaId < 1 || !verticesFromVariantList(values, &points) || points.size() < 3) {
        qWarning() << "A hunting area requires at least three vertices";
        return false;
    }
    if (!m_db.transaction()) {
        qWarning() << "Failed to begin hunting area update:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM hunting_area_vertices WHERE area_id = ?");
    query.addBindValue(areaId);
    if (!query.exec()) {
        qWarning() << "Failed to remove hunting area vertices:" << query.lastError().text();
        m_db.rollback();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        QSqlQuery existsQuery(m_db);
        existsQuery.prepare("SELECT 1 FROM hunting_areas WHERE id = ?");
        existsQuery.addBindValue(areaId);
        if (!existsQuery.exec() || !existsQuery.next()) {
            qWarning() << "Hunting area does not exist:" << areaId;
            m_db.rollback();
            return false;
        }
    }
    query.prepare("INSERT INTO hunting_area_vertices "
                  "(area_id, vertex_index, lat, lon) VALUES (?, ?, ?, ?)");
    for (qsizetype index = 0; index < points.size(); ++index) {
        query.bindValue(0, areaId);
        query.bindValue(1, index);
        query.bindValue(2, points.at(index).latitude());
        query.bindValue(3, points.at(index).longitude());
        if (!query.exec()) {
            qWarning() << "Failed to update hunting area vertex:" << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }
    if (!m_db.commit()) {
        qWarning() << "Failed to commit hunting area update:" << m_db.lastError().text();
        return false;
    }
    emit huntingAreasChanged();
    return true;
}

bool DatabaseManager::deleteHuntingArea(int areaId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM hunting_areas WHERE id = ?");
    query.addBindValue(areaId);
    if (!query.exec()) {
        qWarning() << "Failed to delete hunting area:" << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        qWarning() << "Hunting area does not exist:" << areaId;
        return false;
    }
    emit huntingAreasChanged();
    return true;
}
