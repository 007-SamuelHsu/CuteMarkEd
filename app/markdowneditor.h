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
#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <qplaintextedit.h>

class MarkdownHighlighter;


class MarkdownEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void resetHighlighting();
    void loadStyleFromStylesheet(const QString &fileName);

    int countWords() const;

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    MarkdownHighlighter *highlighter;
};

#endif // MARKDOWNEDITOR_H
