#!/bin/bash

# test_message_queue_functionality.sh
# Testskript zur Überprüfung der Nachrichtenwarteschlangen-Funktionalität

# Erstellen einer temporären Nachrichtenwarteschlange
MQ_PATH="/my_test_queue"
mq_unlink $MQ_PATH
mq_create $MQ_PATH

# Ausführen der gather_sysinfo Funktion
./gather_sysinfo

# Empfang der Nachricht aus der Nachrichtenwarteschlange
RECEIVED_MESSAGE=$(mq_receive $MQ_PATH)

# Erwartetes Ausgabeformat
EXPECTED_PATTERN="sysinfo,host=my_host uptime=[0-9]+,total_ram=[0-9]+,free_ram=[0-9]+,process_count=[0-9]+ [0-9]+"

# Überprüfen, ob die empfangene Nachricht dem erwarteten Format entspricht
if [[ $RECEIVED_MESSAGE =~ $EXPECTED_PATTERN ]]; then
  echo "Test bestanden: Die empfangene Nachricht entspricht dem erwarteten Format."
else
  echo "Test fehlgeschlagen: Die empfangene Nachricht entspricht nicht dem erwarteten Format."
  echo "Die empfangene Nachricht war: $RECEIVED_MESSAGE"
fi

# Aufräumen
mq_unlink $MQ_PATH
