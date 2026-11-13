# QTPy ESP32-S2 repeater remains in TinyUF2 after firmware update

## Summary

The `QTPy_ESP32S2_WIO_repeater` firmware builds successfully, and the final full-image firmware was written through the QTPy’s TinyUF2 mass-storage bootloader. After reset and a full power cycle, however, the board continued to enumerate as the TinyUF2 bootloader instead of starting the repeater application. No repeater startup output appeared on the board’s USB CDC or UART debug connection.

The original goal is to run a minimal MeshCore repeater on the Adafruit QT Py ESP32-S2 with its USB serial configuration interface working. The failure point in the application boot path is not yet known.

## Hardware and build

- Board: Adafruit QT Py ESP32-S2, TinyUF2 Bootloader 0.10.2, board ID `ESP32S2-QTPY-revB`
- Radio: Seeed Wio-SX1262
- PlatformIO environment: `QTPy_ESP32S2_WIO_repeater`
- USB CDC: intended as the configuration interface
- UART-TTL adapter: `/dev/ttyUSB0`, 115200 baud; board UART0 pins are GPIO5/TX and GPIO16/RX
- NeoPixel: power GPIO38, data GPIO39
- Debug image includes GDB symbols and registers an ESP32-S2 Xtensa exception handler that invokes `esp_gdbstub_panic_handler()`.

## Expected behavior

After flashing and reset, the device should leave TinyUF2 mode, enumerate as the repeater’s USB CDC serial device, and produce UART startup markers:

1. `QTPy debug UART ready`
2. `Board initialized`
3. `Starting radio initialization`
4. `Radio initialization succeeded` or `Radio initialization failed`

The startup indicator should turn green at initialization and red if `radio_init()` returns false.

## Actual behavior

- The final firmware build succeeds.
- TinyUF2 consumes the update file and remains available over USB mass storage after reset.
- The bootloader’s `CURRENT.UF2` was decoded and compared against the generated image; the flashed image matched the expected build contents.
- After tapping RESET and after a reported full power cycle, USB still enumerated as Adafruit TinyUF2 (`239a:0111`), not as the running repeater. `/dev/ttyACM0` did not appear.
- No repeater startup messages were captured from `/dev/ttyUSB0`.
- Whether the application is being rejected, returning to the bootloader, or failing before its first UART message has not been determined.

## Investigation and evidence

1. The target compiled successfully with `pio run -e QTPy_ESP32S2_WIO_repeater`.
2. The USB CDC upload path worked while the app enumerated normally, but later became unavailable after a deliberate GDB-stub test exception halted execution.
3. The ESP32-S2 GDB stub was verified independently: over UART0, it emitted a stop packet (`$T0b#e6`), and Xtensa GDB connected and resolved the injected null-pointer store to `setup()` in `examples/simple_repeater/main.cpp`.
4. The deliberate test fault was removed from the final source and build. It must not be reintroduced in a normal repeater image.
5. Adding `sdkconfig.defaults` did not enable the GDB stub for this Arduino/PlatformIO build; that file was removed. The working test used the SDK-provided panic handler and exception registration instead.
6. To recover from the halted test image without access to the board buttons, the QTPy was placed in TinyUF2 mode. The final image was generated from PlatformIO’s merged flash output and converted to an ESP32-S2 UF2 image matching the board’s existing TinyUF2 image layout. The bootloader consumed it, and its `CURRENT.UF2` matched the expected image.
7. Resetting and power-cycling after that update did not restore normal application enumeration or produce startup output.

## Relevant implementation

- `variants/qtpy_esp32s2_wio/platformio.ini`: board pins and repeater environment; currently includes `-g3`, `-ggdb`, `ARDUINO_USB_MODE=1`, and `ARDUINO_USB_CDC_ON_BOOT=1`.
- `variants/qtpy_esp32s2_wio/target.cpp`: `install_crash_gdbstub()` and `radio_init()`.
- `examples/simple_repeater/main.cpp`: early UART setup, indicator startup, and progress markers around `radio_init()`.
- `variants/qtpy_esp32s2_wio/QTPyESP32S2WIOBoard.h`: NeoPixel power and color control.
- `merge-bin.py`: produces the full flash image used when preparing the UF2 update.

## Questions to resolve

- Why does the ESP32-S2 remain in TinyUF2 mode after the expected image is written and the board is reset?
- Is the application image at the right address and compatible with the bootloader’s OTA/partition configuration?
- Does the app begin executing and reset before UART0 can emit its first marker, or is the bootloader declining to launch it?
- Is the current `ARDUINO_USB_MODE=1` configuration appropriate for the intended USB CDC configuration interface on this board?
- Can a known-good minimal QTPy ESP32-S2 app built with the same PlatformIO board definition boot through this TinyUF2 installation?

## Suggested next diagnostics

1. Preserve the current image and capture a complete power-on/reset UART0 log, beginning before power is applied.
2. If the device still enumerates as TinyUF2, flash a known-good minimal application through the same TinyUF2 path to distinguish bootloader/layout problems from MeshCore startup problems.
3. Compare the board’s partition table, OTA selection data, bootloader settings, application offset, and USB mode against a known-good QT Py ESP32-S2 build.
4. Once the application is confirmed to start, use the existing GDB stub only for naturally occurring faults; keep deliberate fault injection limited to a separately identified test build.
