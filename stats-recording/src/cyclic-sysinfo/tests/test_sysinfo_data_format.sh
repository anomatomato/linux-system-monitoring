#!/bin/bash

# test_sysinfo_data_format.sh
# Testskript zur Überprüfung des sysinfo-Datenabrufs und der Formatierung

# Temporäre Ausgabedatei
OUTPUT_FILE="/tmp/sysinfo_test_output.txt"

# Erwartetes Ausgabeformat
EXPECTED_PATTERN="sysinfo,host=my_host uptime=[0-9]+,total_ram=[0-9]+,free_ram=[0-9]+,process_count=[0-9]+ [0-9]+"

# Ausführen der gather_sysinfo Funktion und Umleitung der Ausgabe in die Datei
./gather_sysinfo > $OUTPUT_FILE

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
