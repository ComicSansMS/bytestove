#include <filestove/ui/widget.hpp>

#include <filestove/ui/pathlist_widget.hpp>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QFileDialog>
#pragma warning(pop)

namespace filestove::ui {

StoveWidget::StoveWidget(std::vector<std::filesystem::path> const& path_list)
    :QWidget(), m_list(new PathlistWidget(this)), m_buttonAddFiles(new QPushButton("+", this)),
     m_buttonAddDirectory(new QPushButton("+ Dir", this)),
     m_buttonRemoveEntry(new QPushButton("-", this)), m_buttonOk(new QPushButton("OK", this))
{
    m_layout.addWidget(m_list);
    m_buttonLayout.addWidget(m_buttonAddFiles);
    m_buttonLayout.addWidget(m_buttonAddDirectory);
    m_buttonLayout.addWidget(m_buttonRemoveEntry);
    m_layout.addLayout(&m_buttonLayout);
    m_layout.addWidget(m_buttonOk);
    setLayout(&m_layout);

    for (auto const& p : path_list) {
        m_list->addItem(QString::fromStdString(p.generic_string()));
    }

    show();

    connect(m_buttonAddFiles, &QPushButton::clicked,
            this, &StoveWidget::onAddFiles);
    connect(m_buttonAddDirectory, &QPushButton::clicked,
            this, &StoveWidget::onAddDirectory);
    connect(m_buttonRemoveEntry, &QPushButton::clicked,
            this, &StoveWidget::onRemoveEntry);
    connect(m_buttonOk, &QPushButton::clicked,
            this, &StoveWidget::commitPathlist);
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

void StoveWidget::onRemoveEntry()
{
    m_list->removeSelectedItems();
}

void StoveWidget::commitPathlist()
{
    QStringList l;
    for (int i = 0; i < m_list->count(); ++i) {
        l.push_back(m_list->item(i)->text());
    }
    emit pathlistUpdate(l);
}

}

