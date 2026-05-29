#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

class QComboBox;
class QCheckBox;
class QPushButton;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);

signals:
    void backClicked();
    void resolutionChanged(int width, int height);
    void fullscreenToggled(bool fullscreen);

private:
    void setupUI();
    void applySettings();

    QComboBox *m_resolutionCombo = nullptr;
    QCheckBox *m_fullscreenCheck = nullptr;
};

#endif // SETTINGSPAGE_H
