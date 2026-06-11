#include "CanTableModel.h"

CanTableModel::CanTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int CanTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_idOrder.size();
}

int CanTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColCount_;
}

QVariant CanTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const uint32_t id = m_idOrder.at(index.row());
    const CanMessage& msg = m_messages[id];

    switch (index.column())
    {
    case ColCanId:     return msg.idToHex();
    case ColDlc:       return msg.dlc;
    case ColData:      return msg.dataToHex();
    case ColCount:     return msg.count;
    case ColTimestamp:  return msg.timestamp.toString("hh:mm:ss.zzz");
    default:           return {};
    }
}

QVariant CanTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
    case ColCanId:     return QStringLiteral("CAN ID");
    case ColDlc:       return QStringLiteral("DLC");
    case ColData:      return QStringLiteral("Data");
    case ColCount:     return QStringLiteral("Count");
    case ColTimestamp:  return QStringLiteral("Timestamp");
    default:           return {};
    }
}

void CanTableModel::updateMessage(const CanMessage& msg)
{
    auto it = m_messages.find(msg.canId);
    if (it != m_messages.end())
    {
        // Update existing row
        it->data[0] = msg.data[0]; it->data[1] = msg.data[1];
        it->data[2] = msg.data[2]; it->data[3] = msg.data[3];
        it->data[4] = msg.data[4]; it->data[5] = msg.data[5];
        it->data[6] = msg.data[6]; it->data[7] = msg.data[7];
        it->dlc = msg.dlc;
        it->count++;
        it->timestamp = msg.timestamp;

        int row = m_idOrder.indexOf(msg.canId);
        emit dataChanged(index(row, 0), index(row, ColCount_ - 1));
    }
    else
    {
        // Insert new row
        int row = m_idOrder.size();
        beginInsertRows(QModelIndex(), row, row);
        m_idOrder.append(msg.canId);
        m_messages.insert(msg.canId, msg);
        endInsertRows();
    }
}

void CanTableModel::clear()
{
    beginResetModel();
    m_idOrder.clear();
    m_messages.clear();
    endResetModel();
}
