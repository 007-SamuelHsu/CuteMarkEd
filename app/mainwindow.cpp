#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QTimer>

#include "controls/activelabel.h"
#include "htmlpreviewgenerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    generator(new HtmlPreviewGenerator(this))
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

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSave()) {
        e->accept();
    } else {
        e->ignore();
    }
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

    toggleHtmlView();

    // start background HTML preview generator
    connect(generator, SIGNAL(resultReady(QString)),
            this, SLOT(htmlResultReady(QString)));
    generator->start();
}

void MainWindow::fileNew()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
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

void MainWindow::styleDefault()
{
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl());
    styleLabel->setText(ui->actionDefault->text());
}

void MainWindow::styleGithub()
{
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/github.css"));
    styleLabel->setText(ui->actionGithub_like->text());
}

void MainWindow::styleSolarizedLight()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/solarized-light+.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/solarized-light.css"));
    styleLabel->setText(ui->actionSolarizedLight->text());
}

void MainWindow::styleSolarizedDark()
{
    ui->plainTextEdit->loadStyleFromStylesheet(":/theme/solarized-dark+.txt");
    ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl("qrc:/css/solarized-dark.css"));
    styleLabel->setText(ui->actionSolarizedDark->text());
}

void MainWindow::styleContextMenu(const QPoint &pos)
{
    QList<QAction*> actions;
    actions << ui->actionDefault << ui->actionGithub_like << ui->actionSolarizedLight << ui->actionSolarizedDark;

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
}

void MainWindow::plainTextChanged()
{
    QString code = ui->plainTextEdit->toPlainText();

    // generate HTML from markdown
    generator->enqueue(code);
}

void MainWindow::htmlResultReady(const QString &html)
{
    ui->webView->setHtml(html);
    ui->htmlSourceTextEdit->setPlainText(html);
}

void MainWindow::setupActions()
{
    // file menu
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->actionExit->setShortcut(QKeySequence::Quit);

    // edit menu
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);

    // style menu
    ui->actionDefault->setShortcut(QKeySequence(Qt::ALT + Qt::Key_1));
    ui->actionGithub_like->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
    ui->actionSolarizedLight->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
    ui->actionSolarizedDark->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4));

    // put style actions in a group
    QActionGroup* group = new QActionGroup( this );
    ui->actionDefault->setActionGroup(group);
    ui->actionGithub_like->setActionGroup(group);
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
