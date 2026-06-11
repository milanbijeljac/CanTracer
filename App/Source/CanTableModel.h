#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include "Core/CanMessage.h"

class CanTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        ColCanId = 0,
        ColDlc,
        ColData,
        ColCount,
        ColTimestamp,
        ColCount_  // total number of columns
    };

    explicit CanTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void updateMessage(const CanMessage& msg);
    void clear();

private:
    QList<uint32_t> m_idOrder;       // row index → CAN ID
    QMap<uint32_t, CanMessage> m_messages; // CAN ID → latest message
};
