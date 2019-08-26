#ifndef FILESTOVE_INCLUDE_GUARD_UI_OPTIONS_DIALOG_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_OPTIONS_DIALOG_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QDialog>

#pragma warning(disable: 4251)
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#pragma warning(pop)


namespace filestove::ui {

class OptionsDialog : public QDialog{
    Q_OBJECT;
private:
    QGridLayout m_layout;
    QLabel* m_labelLogging;
    QCheckBox* m_checkboxLogging;
    QLabel* m_labelInterval;
    QSpinBox* m_editInterval;
    QLabel* m_labelIdle;
    QSpinBox* m_editIdle;
    QLabel* m_labelBufferSize;
    QSpinBox* m_editBufferSize;
    QLabel* m_labelChunkSize;
    QSpinBox* m_editChunkSize;
    /*bool doLogging;
    std::vector<std::filesystem::path> directories;
    std::chrono::milliseconds scanInterval;
    std::size_t readIdleThreshold;
    std::int32_t readBufferSize;
    std::int32_t fileCollectChunkSize;*/
public:
    OptionsDialog();
};

}

#endif


