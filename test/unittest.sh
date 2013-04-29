#!/bin/bash

echo
echo "TESTING: $1"
echo

echo "Running..."
(
  echo 't' | ../src/core/ba $1 1>/dev/null
  status=$?
  if [ $status != 0 ]; then
    echo "======> BAUNITTEST FAILURE - PROGRAM RETURNS $status <======"
  fi
) 2>&1 | grep BAUNITTEST | while read LINE; do
  if echo "$LINE" | grep FAILURE >/dev/null; then
    echo -e "\033[0;31m$LINE\033[0m"
  else
    echo "$LINE"
  fi
done

echo "Done."
echo
