#ifndef FILESTOVE_INCLUDE_GUARD_UI_CONFIG_HOLDER_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_CONFIG_HOLDER_HPP_

#include <filestove/config.hpp>

#include <QObject>

namespace filestove::ui {

class ConfigHolder : public QObject {
    Q_OBJECT;
private:
    Config m_config;
public:
    ConfigHolder(Config config);
public slots:
    void saveConfig();
    void updatePathlist(QStringList const& l);
signals:
    void configChanged();
};

}

#endif
