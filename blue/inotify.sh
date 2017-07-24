#!/bin/bash
WATCHED_DIR="/var/log/bro/current/extract_files/"
EXEC_SCRIPT="./virustotal.py"

# EXEC_SCRIPT_2="./shellcode.py"
# OUTPUT_LOG_2="./shellcode.log"

if [ ! -d "$WATCHED_DIR" ]; then
	exit
fi

while read -r FILE; do
	  echo "file created $FILE"
	    python "$EXEC_SCRIPT" "$FILE" &
      done < <(exec inotifywait -m -e create -q --format '%w%f' "$WATCHED_DIR")

