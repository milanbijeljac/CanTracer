# CanTracer

Qt6 / C++17 desktop application that visualizes CAN bus traffic — a lightweight CANoe-style trace window. It reads a framed binary protocol over a serial port and displays a live, ID-keyed table of received CAN messages.

CanTracer is the host half of a two-repository project. The hardware half is **[STM32F446_Comm](../STM32F446_Comm)** (`D:\01_Git\STM32F446_Comm`) — bare-metal STM32F446RETx firmware acting as a CAN-to-UART bridge. The two repos share a wire format but no source code, so **any change to the frame layout must be mirrored in both**; nothing catches a mismatch at build time.

## Architecture

```
+--------------------------------------------------+
|  CanTracer  (this repo)                          |
|                                                  |
|  App/   MainWindow ── CanTableModel  (Qt Widgets) |
|            |               ^                      |
|  Core/  SerialPort ──> CanFrameParser (Qt Core)   |
+---------------------+----------------------------+
                      | UART (115200 8N1, virtual COM)
+---------------------+----------------------------+
|  STM32F446_Comm firmware (separate repo)         |
|  CAN RX ISR -> FreeRTOS queue -> UART TX task    |
+---------------------+----------------------------+
                      | CANH / CANL
```

The signal chain inside the app is a straight pipeline, wired up in `MainWindow`'s constructor:

```
QSerialPort::readyRead
  -> SerialPort::dataReceived(QByteArray)
    -> CanFrameParser::processBytes()      byte-wise state machine
      -> CanFrameParser::frameReceived(CanMessage)
        -> CanTableModel::updateMessage()  insert new ID / update existing row
          -> QTableView repaint
```

`Core` is a Qt-only static library with no dependency on the GUI, so the parser and serial layer can be reused or unit-tested headlessly. `App` links `Core` plus `Qt6::Widgets`.

## Project Structure

```
CanTracer/
|-- CMakeLists.txt              Top-level: C++17, AUTOMOC, finds Qt6 Widgets + SerialPort
|-- Core/
|   |-- CMakeLists.txt          Static lib "Core" (Qt6::Core, Qt6::SerialPort)
|   +-- Source/Core/
|       |-- CanMessage.h        POD message struct + hex formatting helpers
|       |-- CanFrameParser.*    15-byte frame state machine (SOF/ID/DATA/DLC/EOF)
|       +-- SerialPort.*        QSerialPort wrapper: open/close, port enumeration
+-- App/
    |-- CMakeLists.txt          Executable "CanTracer" (links Core, Qt6::Widgets)
    +-- Source/
        |-- App.cpp             main(): QApplication + MainWindow
        |-- MainWindow.*        Toolbar (port combo, Refresh, Connect, status) + table
        +-- CanTableModel.*     QAbstractTableModel, one row per unique CAN ID
```

## UART Protocol

Fixed-size 15-byte binary frames, sent by the STM32 for every CAN message received.

```
Byte:  [0]    [1..4]     [5..12]     [13]   [14]
Field: SOF    CAN ID     DATA        DLC    EOM
Size:  1B     4B         8B          1B     1B
```

| Field  | Size    | Value / Description                                                  |
|--------|---------|----------------------------------------------------------------------|
| SOF    | 1 byte  | `0xDE` — start of frame marker                                       |
| CAN ID | 4 bytes | 32-bit CAN identifier, **little-endian** (raw MCU byte order)        |
| DATA   | 8 bytes | CAN payload; always 8 bytes on the wire, bytes past DLC are zero-filled by the firmware |
| DLC    | 1 byte  | Data Length Code (0–8), number of valid data bytes                   |
| EOM    | 1 byte  | `0xAD` — end of message marker                                       |

On the firmware side this is a `__attribute__((packed))` struct written straight to USART2, which is why the ID is little-endian rather than network order.

`CanFrameParser` consumes the stream one byte at a time through a three-state machine — `WaitSOF` → `CollectPayload` (13 bytes) → `WaitEOF` — so it recovers from partial reads and mid-stream connects without needing frame-aligned buffers. A DLC above 8 is clamped to 8 before the message is emitted.

## UI

A single window: a toolbar and a table.

| Control      | Behaviour                                                        |
|--------------|------------------------------------------------------------------|
| Port combo   | Lists serial ports from `QSerialPortInfo::availablePorts()`      |
| Refresh      | Re-enumerates ports (use after plugging in the board)            |
| Connect      | Opens the selected port at 115200 8N1; toggles to Disconnect     |
| Status label | `Disconnected` / `Connected: COMx` / `Error: …`                  |

The table is a **trace view, not a log**: one row per unique CAN ID, holding the most recent payload for that ID. Rows appear in first-seen order and are never removed while connected.

| Column    | Content                                        |
|-----------|------------------------------------------------|
| CAN ID    | `0x` + 8 hex digits, upper case                 |
| DLC       | 0–8                                             |
| Data      | Space-separated hex bytes, `DLC` bytes shown    |
| Count     | Number of times this ID has been received       |
| Timestamp | Host arrival time of the last frame, `hh:mm:ss.zzz` |

Connecting clears the model and resets the parser, so each session starts from an empty table.

## Building

Requirements:

- CMake ≥ 3.15
- A C++17 compiler (MSVC, MinGW, GCC, or Clang)
- Qt 6 with the **Widgets** and **SerialPort** modules

```bash
cmake -B build -S . -DCMAKE_PREFIX_PATH=<path-to-Qt6>/lib/cmake
cmake --build build
```

On Windows, point `CMAKE_PREFIX_PATH` at the Qt kit matching your compiler, e.g.
`C:/Qt/6.x.x/msvc2019_64/lib/cmake`. The resulting binary is `build/App/CanTracer`
(`CanTracer.exe` on Windows); with a shared Qt build it needs Qt's `bin` directory on
`PATH`, or run `windeployqt` against the executable.

`CMAKE_AUTOMOC` is on at the top level, so the `Q_OBJECT` classes need no manual moc step.

## Running

1. Flash and power the STM32F446_Comm board; it enumerates as a virtual COM port.
2. Start CanTracer, press **Refresh**, pick the port, press **Connect**.
3. With the firmware's default configuration (CAN in loopback mode, six cyclic TX messages), rows appear within a few seconds and their Count columns tick up at the configured periods.

## Current Limitations

- **Receive only.** `SerialPort::open()` uses `QIODevice::ReadOnly` and there is no transmit path — the app cannot yet send CAN frames or commands to the STM32. The firmware side is likewise TX-only (`USART_CR1_RE` is never set).
- **Baud rate is fixed** at 115200 in the UI; `SerialPort::open()` takes a `baudRate` argument but `MainWindow` always uses the default.
- **No logging or export.** Nothing is written to disk; history beyond the latest frame per ID is not retained.
- **No filtering, sorting, or DBC decoding** — raw hex only.
- **Frame re-sync is heuristic.** A `0xDE` byte occurring inside a payload restarts payload collection, so a frame whose ID or data contains `0xDE` is discarded and re-synced on the next real SOF. Fixed-length framing without byte stuffing cannot fully disambiguate this; robust recovery would need escaping or a checksum.
- **Row lookup is linear.** `CanTableModel::updateMessage` calls `QList::indexOf` per update — fine for tens of IDs, worth a hash map if the bus grows busy.
- Standard 11-bit IDs are displayed the same as extended ones (8 hex digits, zero-padded); the frame carries no IDE flag to distinguish them.

## Planned Work

- Host-to-STM32 command channel (open the port read/write, add a TX frame format) to send CAN frames and configure messages at runtime
- Message logging and export (an empty `export/` directory is already reserved)
- Filtering and search over IDs; optional append-only log view alongside the trace view
- Selectable baud rate and connection settings
- DBC / signal-level decoding
