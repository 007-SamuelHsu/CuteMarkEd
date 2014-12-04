/*
 * Copyright 2014 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "jsonstyletranslator.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "style.h"
#include "collection.h"

namespace {

static const QLatin1String NAME("name");
static const QLatin1String BUILTIN("builtIn");

}

bool JsonStyleTranslator::processDocument(const QJsonDocument &jsonDocument, Collection<Style> *collection)
{
    if (!isValid(jsonDocument))
        return false;

    QJsonArray styleArray = jsonDocument.object().value("styles").toArray();
    foreach (QJsonValue entry, styleArray) {
        Style style = fromJsonObject(entry.toObject());
        collection->insert(style);
    }

    return true;
}

QJsonDocument JsonStyleTranslator::createDocument(Collection<Style> *collection)
{
    QJsonArray styleArray;
    for (int i = 0; i < collection->count(); ++i) {
        Style style = collection->at(i);

        QJsonObject entry = toJsonObject(style);
        styleArray.append(entry);
    }

    QJsonObject object;
    object.insert("styles", styleArray);

    QJsonDocument doc(object);
    return doc;
}

Style JsonStyleTranslator::fromJsonObject(const QJsonObject &object)
{
    Style style;

    style.name = object.value(NAME).toString();
    style.builtIn = object.value(BUILTIN).toBool();

    return style;
}

QJsonObject JsonStyleTranslator::toJsonObject(const Style &style)
{
    QJsonObject object;

    object.insert(NAME, style.name);
    object.insert(BUILTIN, style.builtIn);

    return object;
}

bool JsonStyleTranslator::isValid(const QJsonDocument &jsonDocument) const
{
    return !jsonDocument.isEmpty() &&
           jsonDocument.isObject() &&
           jsonDocument.object().contains("styles") &&
           jsonDocument.object().value("styles").isArray();
}

