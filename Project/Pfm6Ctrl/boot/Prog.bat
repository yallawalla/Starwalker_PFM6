@echo off
rem    set serial port number / nastavi ustrezno številko porta --pn Nr
rem    set filename / nastavi ime datoteke --fn name.hex
rem    connection can be made by the first FTDI USB/UART interface (Connected to the only one) / povezava se naredi s prvim FTDI USB/UART vmesnikom (prikljuèen naj bo le eden)

boot_reset.exe COM25
STMFlashLoader.exe -c --pn 25 --to 10000 --br 921600 --db 8 --pr EVEN --sb 1 --co OFF -i STM32F2_1024K -e --all -d --fn Pfm6Ctrl.hex --v --o -r --a 0x08000000


