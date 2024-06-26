#!/bin/bash

# test_process_psi_data.sh
# Testskript zur Überprüfung der Verarbeitung von PSI-Daten

# Temporäre Ausgabedatei
OUTPUT_FILE="/tmp/psi_data_test_output.txt"

# Erwartetes Ausgabeformat
EXPECTED_PATTERN="psi,resource=cpu,stalled=some avg10=[0-9.]+,avg60=[0-9.]+,avg300=[0-9.]+,total=[0-9]+ [0-9]+"

# Beispielhafte PSI-Daten
PSI_DATA="some avg10=0.00 avg60=0.00 avg300=0.00 total=12345"

# Ausführen der Funktion process_psi_data und Umleitung der Ausgabe in die Datei
echo $PSI_DATA | ./epoll_psi > $OUTPUT_FILE

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
rm $OUTPUT_FILE
