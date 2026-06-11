#pragma once

#include <QObject>
#include "CanMessage.h"

namespace Core {

class CanFrameParser : public QObject
{
    Q_OBJECT

public:
    explicit CanFrameParser(QObject* parent = nullptr);

    void processBytes(const QByteArray& data);
    void reset();

signals:
    void frameReceived(const CanMessage& message);

private:
    static constexpr uint8_t SOF = 0xDE;
    static constexpr uint8_t EOF_MARKER = 0xAD;
    static constexpr int PAYLOAD_SIZE = 13; // CAN ID(4) + DATA(8) + DLC(1)
    static constexpr int FRAME_SIZE = 15;   // SOF + PAYLOAD + EOF

    enum class State
    {
        WaitSOF,
        CollectPayload,
        WaitEOF
    };

    void processByte(uint8_t byte);
    CanMessage parsePayload() const;

    State m_state = State::WaitSOF;
    uint8_t m_buffer[PAYLOAD_SIZE] = {};
    int m_index = 0;
};

} // namespace Core
