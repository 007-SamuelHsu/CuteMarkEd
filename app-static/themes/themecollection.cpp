/*
 * Copyright 2015 Christian Loose <christian.loose@hamburg.de>
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
#include "themecollection.h"


int ThemeCollection::insert(const Theme &theme)
{
    themesIndex << theme.name();
    themes << theme;
    return 0;
}

int ThemeCollection::count() const
{
    return themes.count();
}

const Theme &ThemeCollection::at(int offset) const
{
    return themes.at(offset);
}

const QString ThemeCollection::name() const
{
    return QStringLiteral("themes");
}
