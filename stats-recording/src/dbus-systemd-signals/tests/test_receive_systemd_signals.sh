#!/bin/bash

# test_receive_systemd_signals.sh
# Testskript zur Überprüfung des Empfangs von systemd-Signalen

# Temporäre Ausgabedatei
OUTPUT_FILE="/tmp/systemd_signal_test_output.txt"

# Erwartetes Ausgabeformat
EXPECTED_PATTERN="systemd_event,unit=example.service state=started [0-9]+"

# Starten der dbus-systemd-signals Komponente und Umleitung der Ausgabe in die Datei
./dbus_systemd_signals > $OUTPUT_FILE &

# Prozess-ID speichern
PID=$!

# Kurze Wartezeit, um sicherzustellen, dass die Komponente gestartet ist
sleep 5

# Senden eines Testsignals an systemd
systemctl start example.service

# Kurze Wartezeit, um das Signal zu verarbeiten
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
