#!/bin/bash

#Variables
BUILD_DIR=build.win
OUTPUT_NAME=invaders
METADATA_FILE=metadata.yml

#Extract game version from METADATA_FILE
VERSION=$(grep '^version:' "$METADATA_FILE" | awk '{print $2}')

# Fallback if version not found
if [ -z "$VERSION" ]; then
  VERSION="v0.0.0"
  echo "Version not found in metadata.yml, using default $VERSION"
fi

ZIP_NAME=$BUILD_DIR/${OUTPUT_NAME}-win-${VERSION}.zip

EXE_FILE=$BUILD_DIR/$OUTPUT_NAME.exe
DLL_FILES=$(find $BUILD_DIR -name '*.dll')

#Clean old zip
rm -f $ZIP_NAME

zip -j $ZIP_NAME $EXE_FILE $DLL_FILES $EXTRA_FILES 2>/dev/null

if [ -f "$ZIP_NAME" ]; then
    echo "Created $ZIP_NAME successfully in $BUILD_DIR."
else
    echo "Failed to create ZIP archive."
fi