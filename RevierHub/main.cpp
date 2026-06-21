#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QDebug>
#include <QGeoServiceProvider>

#include <QSslSocket>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Force Qt to scan lib/arm64 for all plugin types
    QString libPath = "/data/app/~~.../lib/arm64"; // we need the dynamic path
    QString libDir = QCoreApplication::applicationDirPath();
    app.addLibraryPath(libDir);

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
    return QGuiApplication::exec();
}
