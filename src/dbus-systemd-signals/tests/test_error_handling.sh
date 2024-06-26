#!/bin/bash

# test_error_handling.sh
# Testskript zur Überprüfung der Fehlerbehandlung

# Temporäre Ausgabedatei
ERROR_LOG="/tmp/systemd_signal_error_log.txt"

# Erwartete Fehlermeldung
EXPECTED_ERROR="Failed to connect to system bus"

# Starten der dbus-systemd-signals Komponente mit falschen Berechtigungen
# Umleitung der Fehlerausgabe in die Datei
sudo -u nobody ./dbus_systemd_signals 2> $ERROR_LOG

# Lesen der Fehlerausgabe aus der Datei
ERROR_OUTPUT=$(cat $ERROR_LOG)

# Überprüfen, ob die Fehlermeldung der erwarteten Fehlermeldung entspricht
if [[ $ERROR_OUTPUT == *"$EXPECTED_ERROR"* ]]; then
  echo "Test bestanden: Die Fehlermeldung entspricht der erwarteten Fehlermeldung."
else
  echo "Test fehlgeschlagen: Die Fehlermeldung entspricht nicht der erwarteten Fehlermeldung."
  echo "Die Fehlermeldung war: $ERROR_OUTPUT"
fi

# Aufräumen
rm $ERROR_LOG
