#include <filestove/ui/options_dialog.hpp>

#include <utility>
#include <limits>

namespace filestove::ui {

OptionsDialog::OptionsDialog(Config const& config)
    :m_checkboxLogging(new QCheckBox("Write Logfile", this)), m_editInterval(new QSpinBox(this)),
     m_editIdle(new QSpinBox(this)), m_editBufferSize(new QSpinBox(this)), m_editChunkSize(new QSpinBox(this)),
     m_buttonOk(new QPushButton("OK", this)), m_buttonCancel(new QPushButton("Cancel", this))
{
    setWindowTitle("FileStove Options");

    m_checkboxLogging->setChecked(config.doLogging);
    m_editInterval->setRange(10, std::numeric_limits<int>::max());
    m_editInterval->setValue(static_cast<int>(config.scanInterval.count()));
    m_editIdle->setRange(1, 1024);
    m_editIdle->setValue(static_cast<int>(config.readIdleThreshold >> 20));
    m_editBufferSize->setRange(1, std::numeric_limits<int>::max());
    m_editBufferSize->setValue(config.readBufferSize);
    m_editChunkSize->setRange(1, std::numeric_limits<int>::max());
    m_editChunkSize->setValue(config.fileCollectChunkSize);

    m_layoutForm.addRow(m_checkboxLogging);
    m_layoutForm.addRow("Scan Interval (msecs):", m_editInterval);
    m_layoutForm.addRow("File I/O Idle Threshold (MB):", m_editIdle);
    m_layoutForm.addRow("File read buffer size (bytes):", m_editBufferSize);
    m_layoutForm.addRow("File collect chunk size:", m_editChunkSize);
    m_layout.addLayout(&m_layoutForm);

    m_layoutButtons.addStretch();
    m_layoutButtons.addWidget(m_buttonOk);
    m_layoutButtons.addWidget(m_buttonCancel);
    m_layout.addLayout(&m_layoutButtons);

    setLayout(&m_layout);

    connect(m_buttonOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

std::optional<Config> OptionsDialog::updateConfig(Config const& c) const
{
    Config ret = c;
    ret.doLogging = m_checkboxLogging->isChecked();
    ret.scanInterval = std::chrono::milliseconds(m_editInterval->value());
    ret.readIdleThreshold = std::min(m_editIdle->value(), 1024) << 20;
    ret.readBufferSize = m_editBufferSize->value();
    ret.fileCollectChunkSize = m_editChunkSize->value();

    return (ret == c) ? std::nullopt : std::make_optional(ret);
}

}
