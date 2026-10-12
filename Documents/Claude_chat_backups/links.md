**Exported:** 9/15/2026 16:55:24  
**Link:** [https://claude.ai/chat/4adb8d73-a3b6-493f-9ae6-4a7793bcbf06](https://claude.ai/chat/4adb8d73-a3b6-493f-9ae6-4a7793bcbf06)  

--
import requests
loc = requests.get("https://ipapi.co/json/").json()
MY_LOCATION = [loc["latitude"], loc["longitude"]]
--
- **Flächen tab** — add your own named areas as colored circles, with a color picker; they're saved across sessions in the browser
- **GeoJSON import** — tap "GeoJSON importieren" to load a `.geojson` file you drew at [geojson.io](https://geojson.io) — perfect for irregular borders and polygons
- Pull-up panel so the map stays large and finger-friendly
--

> File: [/mnt/user-data/outputs/feldkarte.html](https://claude.ai/api/organizations/f3224f93-c182-433f-84b2-8e76e5b39364/conversations/4adb8d73-a3b6-493f-9ae6-4a7793bcbf06/wiggle/download-file?path=%2Fmnt%2Fuser-data%2Foutputs%2Ffeldkarte.html)

--
|---|---|---|---|---|---|
| **A** | PWA + [Protomaps](https://protomaps.com) PMTiles | ✅ single file, HTTP-range served | ✅ | ✅ | Medium |
| **B** | [OruxMaps](https://www.oruxmaps.com) + custom layers | ✅ many formats | ✅ built-in | ✅ built-in | Low (existing app) |
| **C** | Python Flask server on phone (Termux) + PWA | ✅ MBTiles via server | ✅ | ✅ | High |
--

- **PMTiles** is a single `.pmtiles` file (download your Bavarian region from [protomaps.com/downloads](https://protomaps.com/downloads) — about 1–3 GB for Bavaria at full detail)
- It works via HTTP range requests — you serve it from a tiny local server, or use a service worker so Chrome can read it offline
--
> Run with --scan to get full insights.
> Get more help at https://help.gradle.org.
BUILD FAILED in 324ms
--
You can use '--warning-mode all' to show the individual deprecation warnings and determine if they come from your own scripts or plugins.
For more on this, please refer to https://docs.gradle.org/8.12/userguide/command_line_interface.html#sec:command_line_warnings in the Gradle documentation.
Building the android package failed!
--
 - Build Authoring improvements
For more details see https://docs.gradle.org/8.14/release-notes.html
Starting a Gradle Daemon (subsequent builds will be faster)
--
You can use '--warning-mode all' to show the individual deprecation warnings and determine if they come from your own scripts or plugins.
For more on this, please refer to https://docs.gradle.org/8.14/userguide/command_line_interface.html#sec:command_line_warnings in the Gradle documentation.
Build file '/home/retno/Development/qt_android/android_test/build/Android_Qt_6_9_1_android_arm64_v8a_Clang_arm64_v8a-Debug/android-build-appandroid_test/build.gradle' line: 42
--
> Run with --scan to get full insights.
> Get more help at https://help.gradle.org.
BUILD FAILED in 19s
--
```bash
git clone https://github.com/microsoft/vcpkg ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
--
```bash
git clone --recurse-submodules https://github.com/maplibre/maplibre-qt
```
--
```bash
git clone --recurse-submodules https://github.com/maplibre/maplibre-qt
```
--
```bash
git clone --recurse-submodules https://github.com/maplibre/maplibre-qt ~/Development/maplibre-qt
```
--
```bash
git clone --recurse-submodules https://github.com/maplibre/maplibre-qt.git ~/Development/maplibre-qt
```
--
```bash
GIT_TERMINAL_PROMPT=0 git clone --recurse-submodules https://github.com/maplibre/maplibre-qt.git ~/Development/maplibre-qt
```
--
```bash
git clone --recurse-submodules --depth 1 https://github.com/maplibre/maplibre-qt.git ~/Development/maplibre-qt
```
--
git config --global credential.helper store
git clone --recurse-submodules --depth 1 https://github.com/maplibre/maplibre-qt.git ~/Development/maplibre-qt
```
--

- [Future · Milestone #3 · maplibre/maplibre-native-qt](https://github.com/maplibre/maplibre-native-qt/milestone/3)
- [Releases · maplibre/maplibre-native-qt](https://github.com/maplibre/maplibre-native-qt/releases)
- [GitHub - maplibre/maplibre-native-qt: MapLibre Native Qt Bindings and Qt Location Plugin · GitHub](https://github.com/maplibre/maplibre-native-qt)
- [GitHub - maplibre/maplibre-native: MapLibre Native - Interactive vector tile maps for iOS, Android and other platforms. · GitHub](https://github.com/maplibre/maplibre-native)
- [maplibre-native-qt/CHANGELOG.md at main · maplibre/maplibre-native-qt](https://github.com/maplibre/maplibre-native-qt/blob/main/CHANGELOG.md)
- [maplibre-native/platform/qt/README.md at main · maplibre/maplibre-native](https://github.com/maplibre/maplibre-native/blob/main/platform/qt/README.md)
- [MapLibre Native for Qt: QMapLibre Core](https://maplibre.org/maplibre-native-qt/docs/group__QMapLibre.html)
- [maplibre-native-qt/docs/Usage.md at main · maplibre/maplibre-native-qt](https://github.com/maplibre/maplibre-native-qt/blob/main/docs/Usage.md)
- [MapLibre Native for Qt: Documentation](https://maplibre.org/maplibre-native-qt/docs/)
- [Labels · maplibre/maplibre-native-qt](https://github.com/maplibre/maplibre-native-qt/labels)

--
```bash
curl -s https://api.github.com/repos/maplibre/maplibre-native-qt/releases/latest | grep "browser_download_url" | grep -i "android"
```
--

curl -s https://api.github.com/repos/maplibre/maplibre-native-qt/releases/latest | grep "browser_download_url" | grep -i "android"

--

retno@laureline:~/Development/maplibre-native-qt$ curl -s https://api.github.com/repos/maplibre/maplibre-native-qt/releases/latest | grep "browser_download_url" | grep -i "android"
      "browser_download_url": "https://github.com/maplibre/maplibre-native-qt/releases/download/v3.0.0/maplibre-native-qt_v3.0.0_Qt6.5.3_Android.tar.bz2"
      "browser_download_url": "https://github.com/maplibre/maplibre-native-qt/releases/download/v3.0.0/maplibre-native-qt_v3.0.0_Qt6.6.3_Android.tar.bz2"
      "browser_download_url": "https://github.com/maplibre/maplibre-native-qt/releases/download/v3.0.0/maplibre-native-qt_v3.0.0_Qt6.7.3_Android.tar.bz2"

--
cd ~/Development
wget https://github.com/maplibre/maplibre-native-qt/releases/download/v3.0.0/maplibre-native-qt_v3.0.0_Qt6.7.3_Android.tar.bz2
```
--
            name: "maplibregl.mapping.additional_style_urls"
            value: "https://demotiles.maplibre.org/style.json"
        }
--
            name: "maplibregl.mapping.additional_style_urls"
            value: "https://demotiles.maplibre.org/style.json"
        }
--
            name: "maplibregl.mapping.additional_style_urls"
            value: "https://demotiles.maplibre.org/style.json"
        }
--
        name: "maplibregl.mapping.additional_style_urls"
        value: "https://demotiles.maplibre.org/style.json"
    }
--
        name: "maplibre.map.styles"
        value: "https://demotiles.maplibre.org/style.json"
    }
--
            name: "maplibre.map.styles"
            value: "https://demotiles.maplibre.org/style.json"
        }
--
    name: "maplibre.map.styles"
    value: "https://api.maptiler.com/maps/streets/style.json?key=get_your_own_key"
}
--
    name: "maplibre.map.styles"
    value: "https://tiles.openfreemap.org/styles/liberty"
}
--

![1000018581.png](https://claude.ai/api/f3224f93-c182-433f-84b2-8e76e5b39364/files/c5faed19-8490-43b6-946d-5fe3a1806d67/preview)

--

Powered by Claude Exporter (https://www.ai-chat-exporter.net)
