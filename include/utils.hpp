//
// Created by Volkov Sergey on 07/03/2026.
//

#pragma once

#include <QString>
#include <QtEndian>
#include <optional>

namespace utils
{
    inline std::optional<quint64> parseXorValue(const QString& input)
    {
        QString trimmed = input.trimmed();
        if (trimmed.isEmpty()) return std::nullopt;

        quint64 result = 0;
        bool ok = false;
        if (trimmed.startsWith("0x", Qt::CaseInsensitive))
        {
            result = trimmed.mid(2).toULongLong(&ok, 16);
        } else
        {
            result = trimmed.toULongLong(&ok, 10);
        }
        if (result) result = qToBigEndian(result);
        return ok ? std::optional(result) : std::nullopt;
    }

    inline QStringList parseFilenameMask(const QString& input)
    {
        QString trimmed = input.trimmed();
        QStringList masks{};

        //empty masks is technically not an error
        if (trimmed.isEmpty()) return masks;

        for (const QString& mask : trimmed.split(";", Qt::SkipEmptyParts))
        {
            QString maskStr = mask.trimmed();
            if (!maskStr.isEmpty())
            {
                masks.append(maskStr);
            }
        }
        return masks;
    }
}
