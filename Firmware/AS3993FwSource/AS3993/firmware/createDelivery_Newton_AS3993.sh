set -e
#set -x
rm -f build.log
CONFIG_FILE="src/as3993_config.h"
GUI_DIR_uart="../GUI/Reader_Tool/firmware/uart"
GUI_DIR_usb="../GUI/Reader_Tool/firmware/usb"
ENABLE_LOGGER="USE_LOGGER=1"
ENABLE_UART_STREAM="USE_UART_STREAM_DRIVER=1"
ENABLE_AS3994_CPU="CPU=24FJ256GB110"
ENABLE_RADON_CPU="CPU=24FJ64GB004"
DISABLE_BOOTLOADER="DISABLE_BOOTLOADER=1"
#get version information
VERSION=`grep 'FIRMWARE_VERSION' "$CONFIG_FILE" | grep -oPe '[0-9a-fA-F]{6}'`
MAJOR=`echo "$VERSION" | cut -c 1-2`
MAJOR=`echo $((0x$MAJOR))`
MINOR=`echo "$VERSION" | cut -c 3-4`
MINOR=`echo $((0x$MINOR))`
PATCH=`echo "$VERSION" | cut -c 5-6`
PATCH=`echo $((0x$PATCH))`
VERSIONSTRING="v$MAJOR-$MINOR-$PATCH"
echo "Creating Delivery $VERSIONSTRING"
hg revert "$CONFIG_FILE"
cp "$CONFIG_FILE" "$CONFIG_FILE.orig"

echo "--- NEWTON AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define NEWTON *[0-1]/#define NEWTON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/NEWTON_AS3993_$VERSIONSTRING.bin"


#end
mv "$CONFIG_FILE.orig" "$CONFIG_FILE"
