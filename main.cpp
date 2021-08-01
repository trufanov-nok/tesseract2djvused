#include <QCoreApplication>
#include <QCommandLineParser>

#include "hocr2djvused.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("tesseract2djvused");
    a.setApplicationVersion("1.0.0");


    QCommandLineParser parser;
    parser.addVersionOption();
    parser.setApplicationDescription(QObject::tr("A simple converter of singlepage hOCR files produced by Tesseract 3.02+ to format acceptable by DjVuLibre's djvused."));
    parser.addPositionalArgument("hocr_file", QObject::tr("The hOCR file produced by Tesseract to convert."));
    parser.addPositionalArgument("dest_file", QObject::tr("The file to store results"));
    parser.addHelpOption();

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if (args.size() >= 2) {
        HOCR2DjVuSed::convertFile(args[0], args[1]);
    } else if (args.size() == 1) {
        HOCR2DjVuSed::convertFile(args[0], "");
    }

    return 0;
}
