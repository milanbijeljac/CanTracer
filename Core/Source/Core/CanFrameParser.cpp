#include "CanFrameParser.h"
#include <cstring>

namespace Core {

CanFrameParser::CanFrameParser(QObject* parent)
    : QObject(parent)
{
}

void CanFrameParser::processBytes(const QByteArray& data)
{
    for (char c : data)
        processByte(static_cast<uint8_t>(c));
}

void CanFrameParser::reset()
{
    m_state = State::WaitSOF;
    m_index = 0;
}

void CanFrameParser::processByte(uint8_t byte)
{
    switch (m_state)
    {
    case State::WaitSOF:
        if (byte == SOF)
        {
            m_index = 0;
            m_state = State::CollectPayload;
        }
        break;

    case State::CollectPayload:
        if (byte == SOF)
        {
            // Re-sync: treat this as a new SOF
            m_index = 0;
            break;
        }
        m_buffer[m_index++] = byte;
        if (m_index >= PAYLOAD_SIZE)
            m_state = State::WaitEOF;
        break;

    case State::WaitEOF:
        if (byte == EOF_MARKER)
        {
            emit frameReceived(parsePayload());
        }
        // Whether valid or not, go back to waiting for next frame
        m_state = State::WaitSOF;
        m_index = 0;
        break;
    }
}

CanMessage CanFrameParser::parsePayload() const
{
    CanMessage msg;

    // CAN ID: 4 bytes little-endian at buffer[0..3]
    msg.canId = static_cast<uint32_t>(m_buffer[0])
              | (static_cast<uint32_t>(m_buffer[1]) << 8)
              | (static_cast<uint32_t>(m_buffer[2]) << 16)
              | (static_cast<uint32_t>(m_buffer[3]) << 24);

    // DATA: 8 bytes at buffer[4..11]
    std::memcpy(msg.data, &m_buffer[4], 8);

    // DLC: 1 byte at buffer[12]
    msg.dlc = m_buffer[12];
    if (msg.dlc > 8)
        msg.dlc = 8;

    msg.count = 1;
    msg.timestamp = QDateTime::currentDateTime();

    return msg;
}

} // namespace Core
