#!/bin/sh

# Script for building library ZIP file for use on Arduino

# path to XBee ANSI C Host Library
PWD=`pwd`
DRIVER=$PWD/../..
BUILD=$DRIVER/lib/XBeeArduino
rm -rf $BUILD
mkdir -p $BUILD

# We have to copy all sources (and headers) to one dir because
# the Arduino build process is exceptionally limited.
find $DRIVER/src/{arduino,util,wpan,xbee,zigbee} -type f -exec cp {} $BUILD/ \;

# Copy headers while renaming to avoid naming conflicts with Arduino
# headers (like xbee/driver.h).
shopt -s nullglob
for d in wpan xbee zigbee
do
  cd $DRIVER/include/$d
  for f in *.h
  do
    echo "${d}_${f}"
    cp -a "$f" "$BUILD/${d}_${f}"
  done
  cd -
done

# Remove headers for other platforms.
rm $BUILD/xbee_platform_{mbed,dos,hcs08,posix,rabbit,win32}.h

# Remove this build script from the copied files.
rm $BUILD/build.sh

# Create modified xbee/xbee_platform.h since we can't define XBEE_PLATFORM_HEADER
# in a project file or Makefile.
echo "#define XBEE_PLATFORM_HEADER \"xbee_platform_arduino.h\"" > $BUILD/xbee_platform.h 
cat $DRIVER/include/xbee/platform.h >> $BUILD/xbee_platform.h

# remove commissioning code (wants to link zcl_comm_default_sas)
rm $BUILD/*commissioning*

# remove OTA code (wants to link xbee_update_firmware_ota)
rm $BUILD/*_ota_*

# Fix the #include statements throughout the program to reference munged names.
sed -ri -e 's|#include "([a-z]+)/|#include "\1_|' $BUILD/*

cd $BUILD/..
rm $BUILD.zip
zip -r $BUILD.zip *
cd -
