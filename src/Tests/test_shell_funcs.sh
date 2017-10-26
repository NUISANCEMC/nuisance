#!/usr/bin/bash

### Adapted from https://unix.stackexchange.com/questions/4965/keep-duplicates-out-of-path-on-source

add_to_PATH () {
  for d; do
    if [ "$d" == "/usr/bin" ] || [ "$d" == "/usr/bin64" ] || [ "$d" == "/usr/local/bin" ] || [ "$d" == "/usr/local/bin64" ]; then
      case ":$TEST_PATH:" in
        *":$d:"*) :;;
        *) TEST_PATH=$TEST_PATH:$d;;
      esac
    else
      case ":$TEST_PATH:" in
        *":$d:"*) :;;
        *) TEST_PATH=$d:$TEST_PATH;;
      esac
    fi
  done
}

TEST_PATH="/my/path"

add_to_PATH "/my/other/path"

if [ $TEST_PATH != "/my/other/path:/my/path" ]; then
  echo "[FAILED -- Prepend new path ]: Expected \"${TEST_PATH}\" == \"/my/other/path:/my/path\"."
  exit 1
else
  echo "[PASSED -- Prepend new path ]: \"${TEST_PATH}\" == \"/my/other/path:/my/path\"."
fi

add_to_PATH "/my/other/path"

if [ $TEST_PATH != "/my/other/path:/my/path" ]; then
  echo "[FAILED -- prepend duplicate path]: Expected \"${TEST_PATH}\" == \"/my/other/path:/my/path\"."
  exit 1
else
  echo "[PASSED -- prepend duplicate path]: \"${TEST_PATH}\" == \"/my/other/path:/my/path\"."
fi

add_to_PATH "/usr/bin"

if [ $TEST_PATH != "/my/other/path:/my/path:/usr/bin" ]; then
  echo "[FAILED -- append system path]: Expected \"${TEST_PATH}\" == \"/my/other/path:/my/path:/usr/bin\"."
  exit 1
else
  echo "[PASSED -- append system path]: \"${TEST_PATH}\" == \"/my/other/path:/my/path:/usr/bin\"."
fi
