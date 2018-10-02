# pixelpanel-TPM2.Net-SPI
ESP32 - SPI-Pixelpanel Ansteuerung. Code läuft mit arduino-Umgebung auf dem ESP32. 
Schaufelt aus dem UDP-Server auf 65506 pixeldaten im Protokoll TPM2.Net via FastPixel auf den SPI-Port 
des ESP32. Dieser loggt sich in ein WLAN ein und empfängt so die Daten.

Zusätzlich ist demnächst Basecamp inkludiert

## Nutzen 
Einfach per Glediator [[http://www.solderlab.de/]] Pixelmatrix einrichten und auf die IP des ESP32 lenken. Port 65506
Protokoll siehe: [[http://www.ledstyles.de/index.php/Attachment/28200-TPM2-Specs-V1-0-2013-ger-pdf/]]
Anzahl der LEDs angeben und der Code hier müsste auch angepasst werden.

Aktuell wird nicht auf TPM2-Pakete geantwortet. Somit lässt sich auch die Matrixgröße nicht mitteilen.
