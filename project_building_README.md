# Project Building - UnitCamS3 Architecture Analysis

## **Architektura Projektu UnitCamS3**

### **1. Struktura Dwuczęściowa**

```
UnitCamS3-UserDemo/
├── firmware/     # ESP32 firmware (C++/Arduino)
└── web/         # Frontend aplikacja (React + TypeScript)
```

### **2. Jak to działa razem**

#### **A. Część Web (React + Vite)**
- **Technologie**: React, TypeScript, Vite, TailwindCSS, NextUI
- **Build process**: 
  ```bash
  npm run build
  ```
- **Wynik**: Jedna kompresowana strona w `index.html.gz`

#### **B. Integracja z ESP32**
1. **Build web** → generuje `dist/index.html`
2. **Kompresja** → `index.html.gz` (dzięki `vite-plugin-compression`)
3. **Upload** → kopiuje `index.html.gz` do folderu `firmware/data/`
4. **Flash filesystem** → wgrywa do LittleFS na ESP32

#### **C. Jak ESP32 serwuje stronę**

```cpp
// server_ap.cpp
_server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
```

**ESP32 działa jako serwer web:**
- Tworzy sieć WiFi "UnitCamS3-WiFi"
- Uruchamia serwer HTTP na porcie 80
- Serwuje statyczne pliki z LittleFS (flash memory)
- Dostarcza REST API endpoints

### **3. Proces Development**

#### **Krok 1: Rozwój Web UI**
```bash
cd web/
npm install
npm run dev          # Rozwój na localhost:3000
json-server --watch .\json_server\unitcams3.json  # Mock API
```

#### **Krok 2: Build i integracja**
```bash
npm run build        # Tworzy zoptymalizowaną stronę
# Kopiuj dist/index.html → firmware/data/index.html.gz
```

#### **Krok 3: Upload na ESP32**
```bash
cd firmware/
pio run --target uploadfs    # Upload filesystem (strona web)
pio run --target upload      # Upload firmware
```

### **4. Jak ESP32 może mieć stronę bez łączenia do internetu**

**ESP32 przechowuje całą stronę wewnętrznie:**

1. **LittleFS** - system plików w flash memory ESP32
2. **Single-file build** - cała strona React w jednym pliku HTML
3. **Embedded web server** - ESP32 uruchamia serwer HTTP
4. **Static file serving** - serwuje pliki z własnej pamięci

### **5. Architektura komunikacji**

```
[Przeglądarka] ←→ [WiFi: UnitCamS3-WiFi] ←→ [ESP32]
                                              ├── LittleFS (index.html.gz)
                                              ├── REST APIs (/api/*)
                                              ├── Camera stream
                                              └── Config management
```

### **6. REST API Endpoints**

ESP32 dostarcza API dla:
- `/api/cam/*` - kontrola kamery
- `/api/system/*` - konfiguracja systemu  
- `/api/mic/*` - mikrofon
- `/api/poster/*` - poster mode
- `/api/shooter/*` - shooter mode

### **7. Workflow Development**

1. **Frontend dev**: Rozwój UI w `web/` z hot reload
2. **Mock API**: Używa json-server dla testowania
3. **Build**: Kompiluje do single-file HTML
4. **Integration**: Kopiuje do `firmware/data/`
5. **Flash**: Wgrywa filesystem + firmware na ESP32
6. **Test**: Łączy z "UnitCamS3-WiFi" → 192.168.4.1

**Genialność rozwiązania**: ESP32 to kompletny web server z embedded frontend - nie potrzeba zewnętrznego serwera!

## **Kluczowe Pliki Konfiguracyjne**

### Web (Vite Config)
```typescript
// vite.config.ts
export default defineConfig({
  plugins: [react(), viteSingleFile(), viteCompression()],
  server: {
    proxy: {
      "/api": "http://localhost:3000",
    },
  },
});
```

### ESP32 (Server Setup)
```cpp
// server_ap.cpp
String ap_ssid = "UnitCamS3-WiFi";
WiFi.softAP(ap_ssid, emptyString, 1, 0, 1, false);
_server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
```

### WiFi Configuration
```json
// data/config.json
{
    "wifiSsid": "",
    "wifiPass": "",
    "startPoster": "no",
    "waitApFirst": "no",
    "nickname": "UnitCamS3",
    "postInterval": 5,
    "timeZone": "GMT+0",
    "startShooter": "no"
}
```

## **Tryby Pracy**

### 1. **Access Point Mode (domyślny)**
- ESP32 tworzy własną sieć WiFi
- Nazwa sieci: "UnitCamS3-WiFi" (bez hasła)
- Dostęp przez: http://192.168.4.1

### 2. **Poster Mode**
- Wymaga konfiguracji WiFi w config.json
- Łączy się z istniejącą siecią WiFi
- Tryb ciągłego wysyłania zdjęć

### 3. **Shooter Mode**  
- Tryb robienia zdjęć na żądanie
- Może działać w trybie AP lub WiFi

## **Build Requirements**

### Web Development
- Node.js
- npm/yarn
- TypeScript
- React
- Vite

### ESP32 Development
- PlatformIO
- ESP32-S3 toolchain
- LittleFS support

## **Upload Sequence**

1. **Build web assets**:
   ```bash
   cd web/
   npm run build
   ```

2. **Copy to ESP32 data**:
   ```bash
   cp dist/index.html firmware/data/index.html.gz
   ```

3. **Upload filesystem**:
   ```bash
   cd firmware/
   pio run --target uploadfs
   ```

4. **Upload firmware**:
   ```bash
   pio run --target upload
   ```

## **Debugging**

### Serial Monitor
```bash
pio device monitor
# lub
pio run --target monitor
```

### Common Issues
- **Upload failed**: Sprawdź połączenie USB i sterowniki
- **WiFi connection**: Sprawdź konfigurację w config.json
- **Web interface**: Sprawdź czy uploadfs się wykonał poprawnie
- **Reset device**: Użyj narzędzi M5Stack w przypadku problemów z flashem