/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
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
#include "snippetcollectiontest.h"

#include <QtTest>

#include <snippets/snippetcollection.h>


void SnippetCollectionTest::initTestCase()
{
    qRegisterMetaType<SnippetCollection::CollectionChangedType>();  // for QSignalSpy
}

void SnippetCollectionTest::notifiesListenersOfNewSnippets()
{
    const Snippet snippet;
    SnippetCollection collection;

    QSignalSpy spy(&collection, SIGNAL(collectionChanged(SnippetCollection::CollectionChangedType)));

    collection.insert(snippet);

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();

    QCOMPARE(arguments.at(0).value<SnippetCollection::CollectionChangedType>(), SnippetCollection::ItemAdded);
}

void SnippetCollectionTest::notifiesListenersOfRemovedSnippets()
{
    const Snippet snippet;
    SnippetCollection collection;
    QSignalSpy spy(&collection, SIGNAL(collectionChanged(SnippetCollection::CollectionChangedType)));

    collection.insert(snippet);
    collection.remove(snippet);

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeAt(1);

    QCOMPARE(arguments.at(0).value<SnippetCollection::CollectionChangedType>(), SnippetCollection::ItemDeleted);
}
