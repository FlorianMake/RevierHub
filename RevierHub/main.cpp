#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QDebug>
#include <QGeoServiceProvider>
#include <QString>
#include <QSslSocket>
#include <QPermission>
#include <QLocationPermission>
#include <QGeoPositionInfo>
#include <QQmlContext>
#include <QDirIterator>
#include <QFile>
#include <QSaveFile>
#include <QUrl>

#include "getlivelocation.hpp"
#include "trailmanager.hpp"

#include "Database/databasemanager.hpp"

namespace {
QString installHuntingStyle()
{
    QFile source("assets:/styles/hunting.json");
    if (!source.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to read bundled Hunting style:" << source.errorString();
        return {};
    }

    const QString stylePath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation) + "/hunting.json";
    QSaveFile destination(stylePath);
    if (!destination.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write Hunting style:" << destination.errorString();
        return {};
    }

    if (destination.write(source.readAll()) == -1 || !destination.commit()) {
        qWarning() << "Failed to install Hunting style:" << destination.errorString();
        return {};
    }

    return QUrl::fromLocalFile(stylePath).toString();
}
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("RevierHub");
    app.setApplicationName("RevierHub");

    QLocationPermission permission;

    permission.setAccuracy(QLocationPermission::Precise);
    permission.setAvailability(QLocationPermission::Always);

    /*
    // use this to create a output for each single file in the application... helpful for finding new icons!!!!11
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << it.next();
    }
    */

    qApp->requestPermission(
        permission,
        [](const QPermission &permission)
        {
            if (permission.status() == Qt::PermissionStatus::Granted)
            {
                qDebug() << "Location permission granted";
            }
            else
            {
                qDebug() << "Location permission denied";
            }
        });

    DatabaseManager* db = new DatabaseManager(&app);
    if (!db->init()) {
        qWarning() << "Database failed to initialize!";
        return -1;
    }

    const QString huntingStyleUrl = installHuntingStyle();
    if (huntingStyleUrl.isEmpty())
        return -1;

    int sessionId = db->startSession();
    if (sessionId < 0) {
        qWarning() << "Failed to start trail session";
        return -1;
    }

    TrailManager trails(db, sessionId);

    // Force Qt to scan lib/arm64 for all plugin types
    QString libPath = "/data/app/~~.../lib/arm64"; // we need the dynamic path
    QString libDir = QCoreApplication::applicationDirPath();
    app.addLibraryPath(libDir);

    GetLiveLocation liveLocation;

    // Also try setting QT_PLUGIN_PATH env var
    qputenv("QT_PLUGIN_PATH", libDir.toUtf8());
    qputenv("QT_GEOSERVICES_PLUGIN_PATH", libDir.toUtf8());

    // On Android, plugins are in lib/arm64 — add it explicitly
    QString pluginPath = QCoreApplication::applicationDirPath() + "/../lib/arm64";
    app.addLibraryPath(pluginPath);

    QObject::connect(
        &liveLocation,
        &GetLiveLocation::acceptedPositionChanged,
        db,
        [db, sessionId](const QGeoCoordinate &coordinate,
                        double horizontalAccuracy) {
            db->addTrailPoint(sessionId, coordinate, horizontalAccuracy);
        });
    QObject::connect(
        &liveLocation,
        &GetLiveLocation::locationChanged,
        &trails,
        [&trails](const QGeoCoordinate &) {
            trails.getPoints();
        });
    QObject::connect(
        &app,
        &QCoreApplication::aboutToQuit,
        db,
        [db, sessionId]() {
            db->endSession(sessionId);
        });

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty(
        "liveLocation",
        &liveLocation);

    engine.rootContext()->setContextProperty(
        "trails",
        &trails);
    engine.rootContext()->setContextProperty(
        "huntingStyleUrl",
        huntingStyleUrl);

    engine.loadFromModule("RevierHub", "Main");

    return QGuiApplication::exec();
}
