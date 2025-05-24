# t2ftCalendar

Ein Jahreskalender für den Desktop

## Beschreibung der Funktion

t2ftCalendar ist ein einfacher Jahreskalender für das aktuelle Jahr. Es gibt eine Spalte für jeden Monat und eine Zeile für jeden Tag.
Weiterhin werden die Kalenderwochen angezeigt.

![grafik](https://github.com/user-attachments/assets/53720d60-7b79-4fdc-ae75-b945147d3841)

Es wird jede Stunde und beim Aufwachen aus einem 
Stromsparmodus geprüft, ob sich das Jahr geändert hat. Ist das der Fall wird der Kalender für das neue Jahr erzeugt und angezeigt.

Die Applikation ist ein rahmenloses Fenster, das immer hinter allen amderen Fenstern liegt.  

Die Position des Fensters kann geändert werden, wenn sich der Mauscursor auf der Applikation befindet
und die **mittlere** Maustaste gedrückt wird.

Die Größe des Fensters kann geändert werden, wenn sich der Mauscursor auf der Applikation befindet 
und die **rechte** Maustaste gedrückt wird.

Position und Größe werden bei Programmende gespeichert und beim erneuten Start automatisch wiederhergestellt.

Es besteht außerdem die Möglichkeit die Ferientermine und die gesetzlichen Feiertage für ein Bundesland über eine
Konfigurationsdatei zu definieren.

## Konfigurationsdatei

Der Name dieser Datei kann inklusive des Pfads in der Registry unter dem SZ_KEY

    Computer\HKEY_CURRENT_USER\SOFTWARE\t2ft\t2ftCalendar\ConfigFile

angegeben werden.

Fehlt dieser Registry-Eintrag, dann wird die Konfigurationsdatei im Programmverzeichnis unter dem Namen 

    t2ftCalendar.cfg

erwartet.

Die Einträge in dieser Datei erfolgen nach dem einfachen Schema:

    Schlüssel=Wert
    

### Bundesland
Um die passenden Feiertage und Schulferientermine für ein Bundesland zu definieren wird dieser Schlüssel verwendet:

>`Bundesland=` *String* mit dem zweistelliger Ländercode für das Bundesland, entsprechen der Wikipedia Seite zur [ISO 3166-2:DE](https://de.wikipedia.org/wiki/ISO_3166-2:DE)
- **BW** = Baden-Württemberg
- **BY** = Bayern
- **BE** = Berlin
- **BB** = Brandenburg
- **HB** = Bremen
- **HH** = Hamburg
- **HE** = Hessen
- **MV** = Mecklenburg-Vorpommern
- **NI** = Niedersachsen
- **NW** = Nordrhein-Westfalen
- **RP** = Rheinland-Pfalz
- **SL** = Saarland
- **SN** = Sachsen
- **ST** = Sachsen-Anhalt
- **SH** = Schleswig-Holstein
- **TH** = Thüringen 

Die Schulferientermine werden von der Webseite [https://ferien-api.de/](https://ferien-api.de/) geholt.
Die gesetzlichen Feiertage stammen von der Webseite [https://feiertage-api.de](https://feiertage-api.de).

### Import von Kalendern

Es besteht ausßerdem die Möglichkeit Kalender im ICAL format zu importieren. Importierte Kalendereinträge werden als kleine, 
farbige Quadrate am oberen Rand eines Tages dargestellt. Bewegt man den Mauscurser über ein solches Quadrat, erscheint ein 
ToolTip mit näheren Angaben zum Termin.

Zum Importieren eines Kalenders werden die folgenden Schlüssel verwendet, bis auf die URL sind alle Schlüssel optional:

>`URL1=` *String* mit der URL zu unter der die Kalenderdaten im ICAL Format abgefragt werden können

>`DefaultName1=` *String* Name des Kalenders. Wird genutzt, falls in den ICAL Daten keine `X-WR-CALNAME` Property enthalten ist. 
Der Name hat in der aktuellen Programmversion keine Funktion.

>`DefaultColor1=` *String* mit Angabe einer Farbe im CCS Format (z.B. `#FF0000` für Hellrot). 
Wird genutzt, falls in den ICAL Daten keine `X-APPLE-CALENDAR-COLOR` Property enthalten ist.

>`ForcedColor1=` *String* mit Angabe einer Farbe im CCS Format (z.B. `#00FF00` für Hellgrün). 
Falls dieser Schlüssel angegeben ist, wird unabhängig von den Angaben im Kalender immder diese Farbe verwendet.

>`UpdateInterval1=` *Zahl* mit der Angabe der Sekunden die definiert wie oft der Kalender aktualisiert wird. 
Der Vorgabewert ist 3600, also 1 Stunde

Den obigen Schlüsseln wird eine Zahl nachgestellt, um mehrere Kalender zu importieren. 
Also z.B. `URL1=`, `URL2=`, `URL3=`,  und so weiter.
