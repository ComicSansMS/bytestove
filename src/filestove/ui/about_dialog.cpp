#include <filestove/ui/about_dialog.hpp>

#include <filestove/version.hpp>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QApplication>
#pragma warning(pop)

#include <QDesktopServices>
#include <QFile>
#include <QFontMetrics>
#include <QStyle>
#pragma warning(push)
#pragma warning(disable: 4251)
#include <QTextStream>
#pragma warning(pop)
#include <QUrl>

namespace filestove::ui {

AboutDialog::AboutDialog(QWidget* parent)
    :QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
     m_labelImage(new QLabel(this)), m_labelText(new QLabel(this)),
     m_editLicense(new QPlainTextEdit(this)),
     m_buttonOk(new QPushButton("OK", this)), m_buttonAboutQt(new QPushButton("About Qt", this))
{
    setWindowTitle("About FileStove");

    QPixmap img;
    img.load(":/filestove_base.ico");
    m_labelImage->setPixmap(img.scaled(QSize(128, 128), Qt::KeepAspectRatio));

    m_labelText->setText(QString(
        "<b>FileStove Version %1.%2.%3</b><br/>"
        "&#169; 2019 Andreas Weis<br />"
        R"(<a href="website">github.com/ComicSansMS/filestove/</a><br />)"
        "<br/>"
        "Licensed under GPL v3.<br />"
        "This program comes with <em>absolutely no warranty</em>.<br />"
        "This is free software, and you are welcome to<br/>"
        R"(redistribute it <a href="license">under certain conditions</a>.)")
        .arg(version().major).arg(version().minor).arg(version().patch));
    connect(m_labelText, &QLabel::linkActivated, this, &AboutDialog::onLinkClicked);

    m_layoutLabels.addStretch();
    m_layoutLabels.addWidget(m_labelImage);
    m_layoutLabels.addWidget(m_labelText);
    m_layoutLabels.addStretch();
    m_layout.addLayout(&m_layoutLabels);

    {
        QFile f(":/gpl.txt");
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream txstr(&f);
        QString const license_text = txstr.readAll();
        m_editLicense->setPlainText(license_text);
        QFontMetrics const font_metrics = m_editLicense->fontMetrics();
        QSize const text_size = font_metrics.size(0, license_text);
        auto const scrollbar_width = m_editLicense->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        // it is totally unclear where the + 10 part comes from
        m_editLicense->setMinimumWidth(text_size.width() + scrollbar_width + 10);
    }
    m_editLicense->setReadOnly(true);
    m_editLicense->setMinimumHeight(225);
    m_editLicense->hide();
    m_layout.addWidget(m_editLicense);

    m_layoutButtons.addWidget(m_buttonOk);
    m_layoutButtons.addWidget(m_buttonAboutQt);
    m_layout.addLayout(&m_layoutButtons);

    setLayout(&m_layout);

    connect(m_buttonAboutQt, &QPushButton::clicked, this, &AboutDialog::showAboutQt);
    connect(m_buttonOk, &QPushButton::clicked, this, &AboutDialog::accept);
}

void AboutDialog::showAboutQt()
{
    QApplication::aboutQt();
}

void AboutDialog::onLinkClicked(QString const& href)
{
    if (href == "license") {
        m_editLicense->show();
    } else if(href == "website") {
        QDesktopServices::openUrl(QUrl("https://github.com/ComicSansMS/filestove/"));
    }
}

}
