#include <filestove/ui/widget.hpp>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QFileDialog>
#pragma warning(pop)

namespace filestove::ui {

StoveWidget::StoveWidget()
    :QWidget(), m_list(new QListWidget(this)), m_buttonAddFiles(new QPushButton("+", this)),
     m_buttonAddDirectory(new QPushButton("+ Dir", this))
{
    m_layout.addWidget(m_list);
    m_buttonLayout.addWidget(m_buttonAddFiles);
    m_buttonLayout.addWidget(m_buttonAddDirectory);
    m_layout.addLayout(&m_buttonLayout);
    setLayout(&m_layout);

    show();

    connect(m_buttonAddFiles, &QPushButton::clicked,
            this, &StoveWidget::onAddFiles);
    connect(m_buttonAddDirectory, &QPushButton::clicked,
            this, &StoveWidget::onAddDirectory);
}

void StoveWidget::onAddFiles()
{
    QFileDialog filediag(this);
    filediag.setFileMode(QFileDialog::ExistingFiles);
    if (filediag.exec()) {
        QStringList added_files = filediag.selectedFiles();
        for (auto const f : added_files) {
            m_list->addItem(f);
        }
    }
}

void StoveWidget::onAddDirectory()
{
    QFileDialog filediag(this);
    filediag.setFileMode(QFileDialog::Directory);
    filediag.setOption(QFileDialog::ShowDirsOnly);
    if (filediag.exec()) {
        QStringList added_files = filediag.selectedFiles();
        for (auto const f : added_files) {
            m_list->addItem(f);
        }
    }
}

}

