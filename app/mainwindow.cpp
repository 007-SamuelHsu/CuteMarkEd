#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QStandardPaths>
#include <QTextDocumentWriter>
#include <QTimer>
#include <QWebFrame>
#include <QWebPage>
#include <QWebInspector>

#include "controls/activelabel.h"
#include "htmlpreviewgenerator.h"
#include "markdownmanipulator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    generator(new HtmlPreviewGenerator(this)),
    splitFactor(0.5)
{
    ui->setupUi(this);

    setFileName(QString());

    QTimer::singleShot(0, this, SLOT(initializeUI()));
}

MainWindow::~MainWindow()
{
    // stop background HTML preview generator
    generator->enqueue(QString());
    generator->wait();
    delete generator;

    delete ui;
}

void MainWindow::webViewScrolled()
{
    double factor = (double)ui->plainTextEdit->verticalScrollBar()->maximum() /
                   ui->webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    int value = ui->webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    ui->plainTextEdit->verticalScrollBar()->setValue(qRound(value * factor));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSave()) {
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    updateSplitter(false);
}

void MainWindow::initializeUI()
{
    setupActions();

    // add style label to statusbar
    styleLabel = new QLabel(ui->actionDefault->text(), this);
    styleLabel->setToolTip(tr("Change Preview Style"));
    statusBar()->addWidget(styleLabel);

    styleLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(styleLabel, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(styleContextMenu(QPoint)));

    // add view label to statusbar
    viewLabel = new ActiveLabel(this);
    statusBar()->addPermanentWidget(viewLabel);

    connect(viewLabel, SIGNAL(doubleClicked()),
            this, SLOT(toggleHtmlView()));

    ui->tocWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    ui->dockWidget->close();
    toggleHtmlView();

    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(ui->plainTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrollValueChanged(int)));

    QFile f(":/template.html");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString htmlTemplate = f.readAll();
        generator->setHtmlTemplate(htmlTemplate);
    }

    // start background HTML preview generator
    connect(generator, SIGNAL(htmlResultReady(QString)),
            this, SLOT(htmlResultReady(QString)));
    connect(generator, SIGNAL(tocResultReady(QString)),
            this, SLOT(tocResultReady(QString)));
    generator->start();

    // set default style
    styleDefault();

    // load remote javascript and use system proxy configuration
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // setup disk cache for network access
    diskCache = new QNetworkDiskCache(this);
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    diskCache->setCacheDirectory(cacheDir);

//    ui->webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
//    QWebInspector *inspector = new QWebInspector();
//    inspector->setPage(ui->webView->page());
}

void MainWindow::fileNew()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
        ui->plainTextEdit->resetHighlighting();
        ui->webView->setHtml(QString());
        ui->htmlSourceTextEdit->clear();
        setFileName(QString());
    }
}

void MainWindow::fileOpen()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("Markdown Files (*.markdown *.md);;All Files (*)"));
    if (!name.isEmpty()) {
        load(name);
    }
}

bool MainWindow::fileSave()
{
    if (fileName.isEmpty()) {
        return fileSaveAs();
    }

    QTextDocumentWriter writer(fileName, "plaintext");
    bool success = writer.write(ui->plainTextEdit->document());
    if (success) {
        ui->plainTextEdit->document()->setModified(false);
    }

    return success;
}

bool MainWindow::fileSaveAs()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(),
                                              tr("Markdown Files (*.markdown *.md);;All Files (*)"));
    if (name.isEmpty()) {
        return false;
    }

    setFileName(name);
    return fileSave();
}

void MainWindow::fileExportToHtml()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to HTML..."), QString(),
                                                    tr("HTML Files (*.html *.htm);;All Files (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QTextDocumentWriter writer(fileName, "plaintext");
    writer.write(ui->htmlSourceTextEdit->document());
}

void MainWindow::fileExportToPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to PDF..."), QString(),
                                                    tr("PDF Files (*.pdf);;All Files (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QPrinter printer;
    printer.setOutputFileName(fileName);
    printer.setOutputFormat(QPrinter::PdfFormat);
    ui->webView->print(&printer);
}

void MainWindow::filePrint()
{
    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if (ui->webView->hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);

    if (dlg->exec() == QDialog::Accepted)
        ui->webView->print(&printer);

    delete dlg;
}

void MainWindow::editUndo()
{
    if (ui->plainTextEdit->document()->isUndoAvailable()) {
        ui->plainTextEdit->document()->undo();
    }
}

void MainWindow::editRedo()
{
    if (ui->plainTextEdit->document()->isRedoAvailable()) {
        ui->plainTextEdit->document()->redo();
    }
}

void MainWindow::editCopyHtml()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->htmlSourceTextEdit->toPlainText());
}

void MainWindow::editStrong()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText("**");
}

void MainWindow::editEmphasize()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText("*");
}

void MainWindow::editStrikethrough()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText("~~");
}

void MainWindow::editInlineCode()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText("`");
}

void MainWindow::styleDefault()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/default.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/markdown.css"));

    generator->setCodeHighlightingStyle("default");
    plainTextChanged();

    styleLabel->setText(ui->actionDefault->text());
}

void MainWindow::styleGithub()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/default.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/github.css"));

    generator->setCodeHighlightingStyle("github");
    plainTextChanged();

    styleLabel->setText(ui->actionGithub->text());
}

void MainWindow::styleSolarizedLight()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/solarized-light+.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/solarized-light.css"));

    generator->setCodeHighlightingStyle("solarized_light");
    plainTextChanged();

    styleLabel->setText(ui->actionSolarizedLight->text());
}

void MainWindow::styleSolarizedDark()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/solarized-dark+.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/solarized-dark.css"));

    generator->setCodeHighlightingStyle("solarized_dark");
    plainTextChanged();

    styleLabel->setText(ui->actionSolarizedDark->text());
}

void MainWindow::extrasMathSupport(bool checked)
{
    generator->setMathSupportEnabled(checked);
    plainTextChanged();
}

void MainWindow::extrasCodeHighlighting(bool checked)
{
    generator->setCodeHighlightingEnabled(checked);
    plainTextChanged();
}

void MainWindow::helpAbout()
{
    QMessageBox::about(this, tr("About CuteMarkEd"),
                       tr("<p><b>CuteMarkEd 0.2</b><br> Qt Markdown Editor<br>Copyright 2013 Christian Loose</p>"));
}

void MainWindow::styleContextMenu(const QPoint &pos)
{
    QList<QAction*> actions;
    actions << ui->actionDefault << ui->actionGithub << ui->actionSolarizedLight << ui->actionSolarizedDark;

    QMenu *menu = new QMenu();
    menu->addActions(actions);

    menu->exec(styleLabel->mapToGlobal(pos));
}

void MainWindow::toggleHtmlView()
{
    if (viewLabel->text() == tr("HTML preview")) {
        ui->webView->hide();
        ui->htmlSourceTextEdit->show();
        viewLabel->setText(tr("HTML source"));
    } else {
        ui->webView->show();
        ui->htmlSourceTextEdit->hide();
        viewLabel->setText(tr("HTML preview"));
    }

    updateSplitter(true);
}

void MainWindow::plainTextChanged()
{
    QString code = ui->plainTextEdit->toPlainText();

    // do nothing when text is empty
    if (code.isEmpty()) {
        return;
    }

    // generate HTML from markdown
    generator->enqueue(code);
}

void MainWindow::htmlResultReady(const QString &html)
{
    ui->webView->page()->networkAccessManager()->setCache(diskCache);

    QUrl baseUrl = QUrl::fromLocalFile(qApp->applicationDirPath());
    ui->webView->setHtml(html, baseUrl);

    ui->htmlSourceTextEdit->setPlainText(html);
}

void MainWindow::tocResultReady(const QString &toc)
{
    QString styledToc = QString("<html><head>\n<style type=\"text/css\">ul { list-style-type: none; padding: 0; margin-left: 1em; } a { text-decoration: none; }</style>\n</head><body>%1</body></html>").arg(toc);
    ui->tocWebView->setHtml(styledToc);
}

void MainWindow::setupActions()
{
    // file menu
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave->setIcon(QIcon("icon-save.fontawesome"));
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->action_Print->setShortcut(QKeySequence::Print);
    ui->action_Print->setIcon(QIcon("icon-print.fontawesome"));
    ui->actionExit->setShortcut(QKeySequence::Quit);

    // edit menu
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionUndo->setIcon(QIcon("icon-undo.fontawesome"));
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionRedo->setIcon(QIcon("icon-repeat.fontawesome"));

    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCut->setIcon(QIcon("icon-cut.fontawesome"));
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionCopy->setIcon(QIcon("icon-copy.fontawesome"));
    ui->actionPaste->setShortcut(QKeySequence::Paste);
    ui->actionPaste->setIcon(QIcon("icon-paste.fontawesome"));
    ui->actionStrong->setShortcut(QKeySequence::Bold);
    ui->actionStrong->setIcon(QIcon("icon-bold.fontawesome"));
    ui->actionEmphasize->setShortcut(QKeySequence::Italic);
    ui->actionEmphasize->setIcon(QIcon("icon-italic.fontawesome"));
    ui->actionStrikethrough->setIcon(QIcon("icon-strikethrough.fontawesome"));

    // view menu
    ui->menuView->insertAction(ui->menuView->actions()[0], ui->dockWidget->toggleViewAction());

    // style menu
    ui->actionDefault->setShortcut(QKeySequence(Qt::ALT + Qt::Key_1));
    ui->actionGithub->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
    ui->actionSolarizedLight->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
    ui->actionSolarizedDark->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4));

    // put style actions in a group
    QActionGroup* group = new QActionGroup( this );
    ui->actionDefault->setActionGroup(group);
    ui->actionGithub->setActionGroup(group);
    ui->actionSolarizedLight->setActionGroup(group);
    ui->actionSolarizedDark->setActionGroup(group);
}

bool MainWindow::load(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    QByteArray content = file.readAll();
    QString text = QString::fromUtf8(content);

    ui->plainTextEdit->resetHighlighting();
    ui->plainTextEdit->setPlainText(text);

    setFileName(fileName);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->plainTextEdit->document()->isModified())
        return true;

    if (fileName.startsWith(QLatin1String(":/")))
        return true;

    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;

    return true;
}

void MainWindow::setFileName(const QString &fileName)
{
    this->fileName = fileName;
    ui->plainTextEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty()) {
        shownName = "untitled.md";
    } else {
        shownName = QFileInfo(fileName).fileName();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("CuteMarkEd"));
    setWindowModified(false);
}

void MainWindow::updateSplitter(bool htmlViewToggled)
{
    // not fully initialized?
    if (centralWidget()->size() != ui->splitter->size()) {
        return;
    }

    QList<int> childSizes = ui->splitter->sizes();
    int leftWidth = ui->splitter->width() * splitFactor;
    int rightWidth = ui->splitter->width() * (1 - splitFactor);

    bool webViewFolded = ui->webView->isVisible() && childSizes[1] == 0;
    bool htmlSourceFolded = ui->htmlSourceTextEdit->isVisible() && childSizes[2] == 0;

    childSizes[0] = leftWidth;
    if (htmlViewToggled || !webViewFolded) {
        childSizes[1] = rightWidth;
    }
    if (htmlViewToggled || !htmlSourceFolded) {
        childSizes[2] = rightWidth;
    }

    ui->splitter->setSizes(childSizes);
}

void MainWindow::tocLinkClicked(const QUrl &url)
{
    QString anchor = url.toString().remove("#");
    ui->webView->page()->mainFrame()->scrollToAnchor(anchor);
}

void MainWindow::viewChangeSplit()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action->objectName() == ui->actionSplit_1_1->objectName()) {
        splitFactor = 0.5;
    } else if (action->objectName() == ui->actionSplit_2_1->objectName()) {
        splitFactor = 0.666;
    } else if (action->objectName() == ui->actionSplit_1_2->objectName()) {
        splitFactor = 0.333;
    } else if (action->objectName() == ui->actionSplit_3_1->objectName()) {
        splitFactor = 0.75;
    } else if (action->objectName() == ui->actionSplit_1_3->objectName()) {
        splitFactor = 0.25;
    }

    updateSplitter(true);
}

void MainWindow::splitterMoved(int pos, int index)
{
    Q_UNUSED(index)
    splitFactor = (float)pos / ui->splitter->size().width();
}

void MainWindow::scrollValueChanged(int value)
{
    double factor = (double)ui->webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) /
                   ui->plainTextEdit->verticalScrollBar()->maximum();

    ui->webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, qRound(value * factor));
}

void MainWindow::addJavaScriptObject()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("mainwin", this);
}
