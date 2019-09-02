#include <filestove/ui/widget.hpp>

#include <filestove/ui/pathlist_widget.hpp>

#include <gbBase/Assert.hpp>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QFileDialog>
#pragma warning(pop)

namespace filestove::ui {

StoveWidget::StoveWidget(std::vector<std::filesystem::path> const& path_list)
    :QWidget(), m_list(new PathlistWidget(this)), m_labelStatus(new QLabel(this)),
     m_progressOverall(new QProgressBar(this)),
     m_buttonAddFiles(new QPushButton("+", this)),
     m_buttonAddDirectory(new QPushButton("+", this)),
     m_buttonRemoveEntry(new QPushButton("-", this))
{
    m_buttonAddFiles->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    m_buttonAddDirectory->setIcon(style()->standardIcon(QStyle::SP_DirIcon));

    m_buttonAddFiles->setToolTip("Add files");
    m_buttonAddDirectory->setToolTip("Add directory");
    m_buttonRemoveEntry->setToolTip("Remove entry from list");

    m_layout.addWidget(m_list);
    m_layout.addWidget(m_labelStatus);
    m_layout.addWidget(m_progressOverall);
    m_progressOverall->hide();
    m_buttonLayout.addWidget(m_buttonAddFiles);
    m_buttonLayout.addWidget(m_buttonAddDirectory);
    m_buttonLayout.addWidget(m_buttonRemoveEntry);
    m_layout.addLayout(&m_buttonLayout);
    setLayout(&m_layout);

    for (auto const& p : path_list) {
        addEntry(p, std::filesystem::is_directory(p) ? FileType::Directory : FileType::Regular);
    }

    setWindowTitle("FileStove");

    connect(m_buttonAddFiles, &QPushButton::clicked,
            this, &StoveWidget::onAddFiles);
    connect(m_buttonAddDirectory, &QPushButton::clicked,
            this, &StoveWidget::onAddDirectory);
    connect(m_buttonRemoveEntry, &QPushButton::clicked,
            this, &StoveWidget::onRemoveEntry);
}

void StoveWidget::onAddFiles()
{
    QFileDialog filediag(this);
    filediag.setFileMode(QFileDialog::ExistingFiles);
    if (filediag.exec()) {
        QStringList added_files = filediag.selectedFiles();
        for (auto const f : added_files) {
            addEntry(f.toStdString(), FileType::Regular);
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
            addEntry(f.toStdString(), FileType::Directory);
        }
    }
}

void StoveWidget::onRemoveEntry()
{
    m_list->removeSelectedItems();
}

void StoveWidget::closeEvent(QCloseEvent* evt)
{
    QStringList l;
    for (int i = 0; i < m_list->count(); ++i) {
        l.push_back(m_list->item(i)->text());
    }
    emit pathlistUpdate(l);
    return QWidget::closeEvent(evt);
}

void StoveWidget::onShowRequested()
{
    show();
}

void StoveWidget::onCollectUpdate(std::uintmax_t n_files, std::uintmax_t total_size)
{
    m_labelStatus->setText(QString("Collecting %2 bytes in %1 files").arg(n_files).arg(total_size));
}

void StoveWidget::onCollectCompleted(std::uintmax_t n_files, std::uintmax_t total_size)
{
    m_labelStatus->setText(QString("%2 bytes in %1 files").arg(n_files).arg(total_size));
    m_progressOverall->setRange(0, static_cast<int>(total_size / (4096 * 1024)));
    m_progressOverall->setValue(0);
    m_progressOverall->show();
}

void StoveWidget::onCookingUpdate(std::uintmax_t bytes_read)
{
    m_progressOverall->setValue(static_cast<int>(bytes_read / (4096 * 1024)));
}

void StoveWidget::onCookingCompleted()
{
    m_progressOverall->hide();
}

void StoveWidget::addEntry(std::filesystem::path const& p, FileType type)
{
    QListWidgetItem* item = new QListWidgetItem();
    if (type == FileType::Directory) {
        item->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    } else {
        GHULBUS_ASSERT(type == FileType::Regular);
        item->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    }
    item->setText(QString::fromStdString(p.generic_string()));
    m_list->addItem(item);
}

}

