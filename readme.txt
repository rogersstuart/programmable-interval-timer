sudo chmod -R 777 /dev/ttyUSB0
./build.sh clean && ./flash.sh /dev/ttyUSB0 512000 && python -m serial.tools.miniterm --raw --eol CRLF --encoding ascii /dev/ttyUSB0 115200
