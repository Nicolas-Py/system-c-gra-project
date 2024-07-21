# Projektbericht: Cache-Simulation und Analyse

## Aufgabenstellung
Dieses Projekt untersucht die Simulation von Direct-Mapped und voll-assoziativen Caches in modernen Prozessoren. Ziel ist es, die Leistungsmetriken beider Cache-Architekturen zu vergleichen, implementiert in SystemC, C++ und C.

## Literaturrecherche
Caches spielen eine entscheidende Rolle in der Prozessorarchitektur, indem sie den Zugriff auf häufig verwendete Daten beschleunigen. Direct-Mapped Caches sind einfach und schnell, da jede Speicheradresse einer bestimmten Cache-Linie zugeordnet ist. Fully-Associative Caches bieten größere Flexibilität und höhere Trefferquoten, sind jedoch komplexer in der Verwaltung und teurer in der Implementierung.

## Methodik und Messumgebung
### Simulation Design
- **Direct-Mapped Cache:** Implementiert mittels direkter Indexierung, wobei jede Speicheradresse einer bestimmten Cache-Linie zugeordnet wird.
- **Full-Associative Cache:** Implementiert mit zufälligem Zugriff, wobei das Finden der richtigen Cache-Linie durch den Vergleich der Tags erfolgt.

Wir haben außerdem einen Hauptspeicher implementiert, der die Aufgabe hat, Daten zurückzuschreiben und die Caches mit Datenblöcken zu aktualisieren, falls beim Lesen aus dem Cache die Daten nicht vorhanden sind.

## Ergebnisse
### Leistungsmetriken
- **Latenz:** Direct-Mapped Cache hatte aufgrund seiner Einfachheit eine geringere Latenz.
- **Verarbeitungszeit:** Direct-Mapped Caches verarbeiteten Anfragen schneller als Fully-Associative Caches bei gleicher Anzahl von Anfragen.
- **Primitive Gates:** Im vollassoziativen Cache mehr Gates gebraucht im Verglich zu Direct-Mapped
- **Hits und Misses:** Anzahl an Hits und Misses sind verschieden zwischen Direct-Mapped und Vollassoziativ mit den gleichen Anfragen

# Fazit 
**Zusammenfassung der Ergebnisse:**
- **Trefferquoten:** In unserer Simulation war die Trefferquote des Fully-Associative Caches nicht höher als die des Direct-Mapped Caches, da wir keine LRU-Ersatzstrategie verwendet haben. (laut Aufgabenstellung)
- **Latenz:** Direct-Mapped Caches hatten eine geringere Latenz im Vergleich zu Fully-Associative Caches.
- **Verarbeitungszeit:** Direct-Mapped Caches verarbeiteten Anfragen schneller aufgrund ihrer einfacheren Architektur.
- **Hardware-Komplexität und Kosten:** Fully-Associative Caches sind teurer und komplexer, da sie mehr primitive Gatter verwenden.

**Schlussfolgerungen:**
- **Leistungsfähigkeit:** Ohne LRU-Ersatzpolitik konnte der Fully-Associative Cache nicht seine volle Effizienz zeigen. Normalerweise bietet diese Architektur höhere Trefferquoten.
- **Anwendungsfälle:** Direct-Mapped Caches sind ideal für Anwendungen, bei denen Geschwindigkeit und Kosten wichtiger sind als die Trefferquote. Fully-Associative Caches eignen sich besser für Systeme, die maximale Speicherzugriffseffizienz erfordern.

**Zukunftsperspektiven:**
- **Erweiterungen:** Untersuchung weiterer Cache-Architekturen wie Set-Associative Caches und Implementierung verschiedener Ersatzstrategien wie LRU, FIFO.
- **Simulationstools:** Einsatz fortschrittlicherer Simulationswerkzeuge zur genaueren Analyse der Cache-Leistungsfähigkeit.
- **Praxisrelevanz:** Anwendung der Simulationsergebnisse in realen Prozessorarchitekturen zur Verbesserung der Systemleistung.

**Lernkurve und Herausforderungen:**
- **Herausforderungen:** Komplexität bei der Implementierung von Fully-Associative Caches ohne LRU-Ersatzpolitik.
- **Lernfortschritte:** Vertieftes Verständnis der Cache-Architekturen und praktische Erfahrung in der Implementierung und Analyse von Systemkomponenten mit C++, SystemC und C.

# Beiträge
Nico:

- Konzeptionierung und Implementierung des angepassten Merge-Sort-Algorithmus
- Erstellen der main.c (und print_usage.c) Funktionen, inklusive Überarbeitung der Sonderfälle(Edge Cases) in Zusammenarbeit mit Daniel
- Automatische Tests der beiden Cache-Implementierungen mit bis zu 100.00 automatisch generierten Instruktionen
- Überprüfung der Kommandozeilenschnittstelle auf korrektes Input-Handling
```
https://go98mir@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```

Daniel:
- Implementierung des Hauptspeichers + Interaktion mit dem Cache
- Allgemeiner Cacheaufbau und Festlegung der Basisfunktion der Caches
- Entwicklung und Implementierung des Direct-Mapped-Cache.
- Optimierung des Voll-Assoziativen-Cache-Modells
- Überprüfung von Sonderfälle und Fehlerszenarien in der Kommandozeilen-Eingabe
```
https://go98puj@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```

Ziang:

- Erstellung des Makefiles 
- Erstellung einer Funktion zum Einlesen von Requests aus CSV-Dateien
- Mitarbeit an der Funktionalität und Implementierung des Voll-Assoziativen-Caches
```
https://go56siq@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```
### [GitHub](https://github.com/Nicolas-Py/system-c-gra-project.git):
```
https://github.com/Nicolas-Py/system-c-gra-project.git
```


# Ausführungsanleitung
1. Alte ausführbare Dateien bereinigen
    ``` bash
    make clean
    ```
2. Die make file ausführen
    ``` bash
    make
    ```
3. Das Programm testen
    ``` bash
    ./cache_simulation "./examples/random_small.csv"
    ```
4. Mit den Optionen vertraut machen
    ``` bash
    ./cache_simulation -h
    ```
5. Das Programm mit weiterer Anpassung ausführen, z.B. Zyklen anzahl
    ``` bash
    ./cache_simulation -c 1000 "./examples/random_small.csv"
    ```

# Testausführungen
Dieses Verzeichnis enthält einige CSV-Dateien, die Sie zum Testen verwenden können.

## Zufällige Operationen
Diese Anweisungen wurden mit `csv_factory_test.py` generiert.

- **random_small.csv:**
100 Zeilen zufälliger Lese-/Schreibanweisungen

- **random_medium.csv:**
1.000 Zeilen zufälliger Lese-/Schreibanweisungen

- **random_big.csv:**
10.000 Zeilen zufälliger Lese-/Schreibanweisungen

## Cache-Nutzung bei Merge Sort
Diese Anweisungen wurden mit `merge_sort.py` generiert.

- **merge_sort_small.csv**
Simuliert Merge Sort mit dem Array

`[2, 0, 1, 3]`

- **merge_sort_medium.csv**
Simuliert Merge Sort mit dem Array

`[22, 6, 11, 21, 13, 10, 8, 7, 23, 25, 24, 29, 9, 15, 3, 31, 18, 20, 0, 16, 30, 2, 28, 27, 14, 19, 5, 1, 12, 4, 26, 17]`

- **merge_sort_big.csv**
Simuliert Merge Sort mit dem Array

`[64, 18, 22, 226, 116, 241, 208, 144, 44, 147, 229, 201, 80, 69, 174, 239, 250, 166, 220, 213, 186, 172, 23, 107, 77, 152, 185, 141, 108, 143, 72, 170, 184, 197, 179, 161, 52, 30, 120, 65, 135, 236, 118, 26, 17, 233, 93, 153, 207, 255, 86, 119, 132, 206, 129, 60, 182, 40, 48, 248, 51, 160, 20, 177, 83, 235, 168, 3, 137, 151, 187, 78, 253, 84, 194, 254, 101, 232, 102, 205, 92, 125, 200, 7, 240, 36, 46, 238, 149, 243, 55, 159, 190, 221, 211, 224, 209, 111, 127, 140, 34, 231, 138, 12, 109, 47, 180, 163, 157, 162, 58, 123, 112, 100, 171, 214, 188, 145, 16, 94, 242, 146, 196, 113, 124, 106, 156, 164, 122, 67, 251, 87, 136, 90, 150, 104, 89, 8, 227, 121, 28, 66, 45, 21, 99, 54, 74, 219, 228, 25, 6, 237, 50, 98, 13, 35, 115, 247, 199, 178, 230, 252, 43, 49, 198, 73, 191, 59, 165, 222, 155, 173, 97, 223, 114, 10, 218, 32, 139, 126, 57, 134, 37, 215, 167, 53, 105, 176, 5, 142, 249, 117, 75, 61, 14, 181, 96, 91, 71, 27, 39, 56, 29, 154, 175, 133, 41, 19, 81, 204, 62, 217, 169, 246, 85, 189, 234, 192, 148, 88, 130, 63, 4, 245, 103, 15, 2, 38, 216, 0, 195, 193, 82, 212, 244, 95, 1, 110, 76, 42, 9, 33, 202, 210, 225, 131, 11, 183, 24, 31, 128, 203, 79, 68, 158, 70]`

Wenn Sie die Lese-/Schreibschritte in einer lesbaren Form sehen möchten, führen Sie `test_files/merge_sort.py` aus.

## test_files
Dieses Verzeichnis enthält einige Python-Skripte für Tests und Eingabegenerierung.

## merge_sort.py
Es wird der Mergesort simuliert, aber jedes Teilarray, das im Rekursionsbaum weitergegeben wird, wird im Heap geschrieben, um die Auswirkungen des caches besser zu simulieren. Wenn Teilarrays zusammengeführt werden, wird das resultierende Array an die Stelle des zuvor unsortierten sub-arrays geschrieben und nicht mehr benötigter Speicher wird "freigegeben".

Das resultierende csv kann weiterverwendet werden um die auswirkung verschiedener cache-strategien auf die laufzeit von merge-sort zu untersuchen.

