avrdude -F -V -c stk500 -p m32 -P /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 -U flash:w:main.hex
