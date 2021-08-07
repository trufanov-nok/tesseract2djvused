#include "hocr2djvused.h"

#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QRegularExpression>

QMap<QString, QString> _dict = {
    {"ocr_page",   "page"},
    {"ocr_column", "column"},
    {"ocr_carea",  "column"},
    {"ocr_par",    "para"},
    {"ocr_line",   "line"},
    {"ocr_word",   "word"},
    {"ocrx_block", "region"},
    {"ocrx_line",  "line"},
    {"ocrx_word",  "word"}
};

QRegularExpression _bbox_rex = QRegularExpression(".*bbox\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+).*");

int _cur_page_h = -1;

void hOCR2DjVuSed(const QDomElement& el, QTextStream& ts, int indent)
{
    QString indent_str = QString(" ").repeated(indent);
    QString res = indent_str;

    const QString cls = el.attribute("class");
    QString id;
    QString bbox;

    const bool ignore = !_dict.contains(cls);
    if (!ignore) {
        id = _dict[cls];
        res += "( " + id + " ";
        const bool is_page = id == "page";


        const QString title = el.attribute("title");
        QRegularExpressionMatch match = _bbox_rex.match(title);
        if (match.hasMatch() && match.capturedLength() >= 5) {
            int x0, y0, x1, y1;
            bool ok;
            x0 = match.captured(1).toInt(&ok);
            assert(ok);
            y0 = match.captured(2).toInt(&ok);
            assert(ok);
            x1 = match.captured(3).toInt(&ok);
            assert(ok);
            y1 = match.captured(4).toInt(&ok);
            assert(ok);

            if (is_page) {
                _cur_page_h = y1;
            } else {
                // (0,0) point is top-left in hOCR and bottom-left in djvused
                assert(_cur_page_h > 0);
                const int h = y1-y0;
                y0 = _cur_page_h - y0 - h;
                y1 = _cur_page_h - y1 + h;
            }

            bbox = QString("%1 %2 %3 %4").arg(x0).arg(y0).arg(x1).arg(y1);
        }

        if (!bbox.isEmpty()) {
            res += bbox + " ";
        }
    }

    if (id == "word") {
        assert(!bbox.isEmpty()); // some tesseract versions use two word tags one inside other
                                 // see: https://github.com/jwilk/ocrodjvu/blob/master/tests/hocr2djvused
                                 // the second one has no bbox and I hope it'll be omitted by text()
        res += "\""+ el.text() + "\"";
        ts << res << " )\n";
    } else {

        const QDomNodeList children = el.childNodes();
        ts << res;
        bool child_found = false;
        const int incr = ignore ? 0 : 1;
        for (int i = 0; i < children.size(); i++) {
            if (children.at(i).isElement()) {
                if (!child_found) {
                    child_found = true;
                    ts << "\n";
                }

                hOCR2DjVuSed(children.at(i).toElement(), ts, indent + incr);
            }
        }


        if (!ignore) {
            if (child_found) {
                ts << indent_str << ")\n";
            } else {
                ts << " )\n";
            }
        }
    }
}

void convertHOCR(const QDomNode& root, QTextStream& ts)
{
    const QDomNodeList children = root.childNodes();
    for (int i = 0; i < children.size(); i++) {
        if (children.at(i).isElement()) {
            hOCR2DjVuSed(children.at(i).toElement(), ts, 0);
        }
    }
}

void convert( const QString& hocr, const QString dest)
{
    QFile hf(hocr);
    if (!hf.open(QIODevice::ReadOnly)) {
        qCritical() << QObject::tr("Error: file \"%1\" can't be open for reading!").arg(hocr);
        return;
    }

    QDomDocument doc;
    doc.setContent(&hf);

    QDomNodeList nodes = doc.elementsByTagName("body");
    if (!nodes.isEmpty()) {

        QTextStream ts;
        QFile df;
        if (!dest.isEmpty()) {
            df.setFileName(dest);
            if (!df.open(QIODevice::WriteOnly)) {
                qCritical() << QObject::tr("Error: file \"%1\" can't be open for writing!").arg(hocr);
                return;
            }
        } else {
            if (!df.open(stdout, QIODevice::WriteOnly)) {
                qCritical() << QObject::tr("Error: can't open stdout for writing!");
                return;
            }
        }

        ts.setDevice(&df);
        convertHOCR(nodes.at(0), ts);
    }
}


void
HOCR2DjVuSed::convertFile(const QString& hocr, const QString dest)
{
    convert(hocr, dest);
}
