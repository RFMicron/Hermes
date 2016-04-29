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

echo "--- FERMI AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/FERMI_AS3993_$VERSIONSTRING.bin"

echo "--- FERMI AS3993 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/FERMI_AS3993_UART_$VERSIONSTRING.bin"

echo "--- FEMTO (2v1) AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define FEMTO2_1 *[0-1]/#define FEMTO2_1 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/FEMTOv2_1_AS3993_$VERSIONSTRING.bin"

echo "--- FEMTO (2v1) AS3993 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define FEMTO2_1 *[0-1]/#define FEMTO2_1 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/FEMTOv2_1_AS3993_UART_$VERSIONSTRING.bin"

echo "--- FEMTO (2v0) AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define FEMTO2 *[0-1]/#define FEMTO2 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/FEMTOv2_AS3993_$VERSIONSTRING.bin"

echo "--- FEMTO (2v0) AS3993 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define FEMTO2 *[0-1]/#define FEMTO2 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/FEMTOv2_AS3993_UART_$VERSIONSTRING.bin"

echo "--- FERMI AS3980 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 0/;s/#define RUN_ON_AS3980..*/#define RUN_ON_AS3980 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/FERMI_AS3980_$VERSIONSTRING.bin"

echo "--- MEGA AS3994 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define MEGA *[0-1]/#define MEGA 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 0/;s/#define RUN_ON_AS3994..*/#define RUN_ON_AS3994 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_AS3994_CPU" "$ENABLE_UART_STREAM" "$DISABLE_BOOTLOADER" >> build.log
cp build/as3993.hex "$GUI_DIR_uart/MEGA_AS3994_$VERSIONSTRING.hex"

echo "--- RADON AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define RADON *[0-1]/#define RADON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_RADON_CPU" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/RADON_AS3993_$VERSIONSTRING.bin"

echo "--- RADON AS3993 UART---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define RADON *[0-1]/#define RADON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" "$ENABLE_RADON_CPU" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/RADON_AS3993_UART_$VERSIONSTRING.bin"

echo "--- NEWTON AS3993 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define NEWTON *[0-1]/#define NEWTON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/NEWTON_AS3993_$VERSIONSTRING.bin"

echo "--- NEWTON AS3980 ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define NEWTON *[0-1]/#define NEWTON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 0/;s/#define RUN_ON_AS3980..*/#define RUN_ON_AS3980 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_LOGGER" >> build.log
cp build/as3993.bin "$GUI_DIR_usb/NEWTON_AS3980_$VERSIONSTRING.bin"

echo "--- NEWTON AS3993 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define NEWTON *[0-1]/#define NEWTON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/NEWTON_AS3993_UART_$VERSIONSTRING.bin"

echo "--- NEWTON AS3980 UART ---"
rm "$CONFIG_FILE"
sed -e 's/#define *FERMI *[0-1]/#define FERMI 0/;s/#define NEWTON *[0-1]/#define NEWTON 1/;s/#define RUN_ON_AS3993..*/#define RUN_ON_AS3993 0/;s/#define RUN_ON_AS3980..*/#define RUN_ON_AS3980 1/' "$CONFIG_FILE.orig" > "$CONFIG_FILE"
make clean ; make "$ENABLE_UART_STREAM" >> build.log
cp build/as3993.bin "$GUI_DIR_uart/NEWTON_AS3980_UART_$VERSIONSTRING.bin"

#end
mv "$CONFIG_FILE.orig" "$CONFIG_FILE"
