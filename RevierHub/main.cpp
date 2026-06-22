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

#include <getlivelocation.hpp>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QLocationPermission permission;

    permission.setAccuracy(QLocationPermission::Precise);

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

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("RevierHub", "Main");

    engine.rootContext()->setContextProperty(
        "liveLocation",
        &liveLocation);

    return QGuiApplication::exec();
}
