#-------------------------------------------------
#
# Project created by QtCreator 2013-03-16T11:40:15
#
#-------------------------------------------------

TEMPLATE = subdirs

win32 {
    SUBDIRS = discount \
        peg-markdown-highlight \
        hunspell \
        app \
        fontawesomeicon

    app.depends = discount peg-markdown-highlight hunspell
}

unix {
    SUBDIRS = peg-markdown-highlight \
        app \
        fontawesomeicon

    app.depends = peg-markdown-highlight
}

