#include "htmlpreviewgenerator.h"

#include "discount/document.h"

HtmlPreviewGenerator::HtmlPreviewGenerator(QObject *parent) :
    QThread(parent),
    mathSupportEnabled(false)
{
}

void HtmlPreviewGenerator::enqueue(const QString &text)
{
    QMutexLocker locker(&tasksMutex);
    tasks.enqueue(text);
    bufferNotEmpty.wakeOne();
}

void HtmlPreviewGenerator::setHtmlTemplate(const QString &t)
{
    htmlTemplate = t;
}

void HtmlPreviewGenerator::setMathSupportEnabled(bool enabled)
{
    mathSupportEnabled = enabled;
}


void HtmlPreviewGenerator::run()
{
    forever {
        QString text;

        {
            // wait for new task
            QMutexLocker locker(&tasksMutex);
            while (tasks.count() == 0) {
                bufferNotEmpty.wait(&tasksMutex);
            }

            text = tasks.dequeue();
        }

        // end processing?
        if (text.isNull()) {
            return;
        }

        // generate HTML from markdown
        Discount::Document document(text);
        QString htmlContent = document.toHtml();
        QString html = renderTemplate(buildHtmlHeader(), htmlContent);
        emit htmlResultReady(html);

        // generate table of contents
        QString toc = document.generateToc();
        emit tocResultReady(toc);
    }
}

QString HtmlPreviewGenerator::renderTemplate(const QString &header, const QString &content)
{
    if (htmlTemplate.isEmpty()) {
        return content;
    }

    return QString(htmlTemplate)
            .replace(QLatin1String("__HTML_HEADER__"), header)
            .replace(QLatin1String("__HTML_CONTENT__"), content);
}

QString HtmlPreviewGenerator::buildHtmlHeader() const
{
    QString header;

    if (mathSupportEnabled) {
        header += "<script type=\"text/javascript\" src=\"http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>";
    }

    return header;
}
