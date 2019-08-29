#ifndef FILESTOVE_INCLUDE_GUARD_UI_OPTIONS_DIALOG_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_OPTIONS_DIALOG_HPP_

#include <filestove/config.hpp>

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QDialog>

#pragma warning(disable: 4251)
#include <QBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#pragma warning(pop)

#include <optional>

namespace filestove::ui {

class OptionsDialog : public QDialog{
    Q_OBJECT;
private:
    QVBoxLayout m_layout;
    QFormLayout m_layoutForm;
    QCheckBox* m_checkboxLogging;
    QSpinBox* m_editInterval;
    QSpinBox* m_editIdle;;
    QSpinBox* m_editBufferSize;
    QSpinBox* m_editChunkSize;
    QHBoxLayout m_layoutButtons;
    QPushButton* m_buttonOk;
    QPushButton* m_buttonCancel;
public:
    OptionsDialog(Config const& config);

    std::optional<Config> updateConfig(Config const& c) const;
};

}

#endif


