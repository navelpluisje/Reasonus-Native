#!/bin/bash

source ./scripts/bash-ini-parser.sh

touch tmp/changelog.txt

# Start populating the chagelog file
CHANGELOG_FILE="./src/changelog.ini"

# Read the global strings and add them to the file
ini_list_keys "$CHANGELOG_FILE" "global" | while read key; do
    value=$(ini_read "$CHANGELOG_FILE" "global" "$key")
    echo "$value" >> tmp/changelog.txt
    echo "" >> tmp/changelog.txt
done

ini_list_keys "$CHANGELOG_FILE" "all" | while read key; do
    value=$(ini_read "$CHANGELOG_FILE" "all" "$key")
    echo "  - [ALL] $value" >> tmp/changelog.txt
done
if ini_key_exists "$CHANGELOG_FILE" "all" "1"; then
  echo "" >> tmp/changelog.txt
fi

# Read the FaderPort 8 strings and add them to the file
ini_list_keys "$CHANGELOG_FILE" "fp8" | while read key; do
    value=$(ini_read "$CHANGELOG_FILE" "fp8" "$key")
    echo "  - [FP 8] $value" >> tmp/changelog.txt
done
if ini_key_exists "$CHANGELOG_FILE" "fp8" "1"; then
  echo "" >> tmp/changelog.txt
fi

# Read the FaderPort v2 strings and add them to the file
ini_list_keys "$CHANGELOG_FILE" "v2" | while read key; do
    value=$(ini_read "$CHANGELOG_FILE" "v2" "$key")
    echo "  - [FP V2] $value" >> tmp/changelog.txt
done
if ini_key_exists "$CHANGELOG_FILE" "v2" "1"; then
  echo "" >> tmp/changelog.txt
fi
# And we're done
