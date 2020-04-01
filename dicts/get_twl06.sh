#!/usr/bin/env bash

die() {
    echo $1
    exit 1
}

OUTPUT=twl06.txt

if [ -f "$OUTPUT" ];
then
    echo "Moving old dictionary out of the way..."
    mv -n $OUTPUT ${OUTPUT}.bak
fi

wget https://www.wordgamedictionary.com/twl06/download/twl06.txt || die "Failed to download dictionary"
dos2unix $OUTPUT || die "Failed to convert windows line endings"
grep -E '^[[:alpha:]]+$' $OUTPUT | tr '[:lower:]' '[:upper:]' | sort --output=$OUTPUT
