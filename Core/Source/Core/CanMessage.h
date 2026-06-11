#pragma once

#include <cstdint>
#include <QString>
#include <QDateTime>

struct CanMessage
{
    uint32_t canId = 0;
    uint8_t  data[8] = {};
    uint8_t  dlc = 0;
    uint32_t count = 0;
    QDateTime timestamp;

    QString dataToHex() const
    {
        QString hex;
        for (uint8_t i = 0; i < dlc; ++i)
        {
            if (i > 0) hex += ' ';
            hex += QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
        }
        return hex;
    }

    QString idToHex() const
    {
        return QString("0x%1").arg(canId, 8, 16, QChar('0')).toUpper();
    }
};
