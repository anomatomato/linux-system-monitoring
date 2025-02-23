#!/bin/bash

# test_epoll_event_handling.sh
# Testskript zur Überprüfung der Ereignisbehandlung

# Temporäre Ausgabedatei
OUTPUT_FILE="/tmp/epoll_event_test_output.txt"

# Erwartetes Ausgabeformat
EXPECTED_PATTERN="psi,resource=cpu,stalled=some avg10=[0-9.]+,avg60=[0-9.]+,avg300=[0-9.]+,total=[0-9]+ [0-9]+"

# Starten der epoll-psi Komponente und Umleitung der Ausgabe in die Datei
./epoll_psi > $OUTPUT_FILE &

# Prozess-ID speichern
PID=$!

# Kurze Wartezeit, um sicherzustellen, dass die Komponente gestartet ist
sleep 5

# Schreiben von Testdaten in die CPU-Pressure-Datei
echo "some avg10=0.00 avg60=0.00 avg300=0.00 total=12345" > /proc/pressure/cpu

# Kurze Wartezeit, um das Ereignis zu verarbeiten
sleep 5

# Lesen der Ausgabe aus der Datei
OUTPUT=$(cat $OUTPUT_FILE)

# Überprüfen, ob die Ausgabe dem erwarteten Format entspricht
if [[ $OUTPUT =~ $EXPECTED_PATTERN ]]; then
  echo "Test bestanden: Die Ausgabe entspricht dem erwarteten Format."
else
  echo "Test fehlgeschlagen: Die Ausgabe entspricht nicht dem erwarteten Format."
  echo "Die Ausgabe war: $OUTPUT"
fi

# Aufräumen
kill $PID
rm $OUTPUT_FILE
