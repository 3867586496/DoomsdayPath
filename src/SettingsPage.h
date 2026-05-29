#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

class QComboBox;
class QCheckBox;
class QPushButton;
class QSpinBox;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);

    int autoSaveInterval() const;
    int autoSaveCount() const;
    void setAutoSaveInterval(int days);
    void setAutoSaveCount(int count);

signals:
    void backClicked();
    void resolutionChanged(int width, int height);
    void fullscreenToggled(bool fullscreen);
    void autoSaveSettingsChanged(int intervalDays, int maxCount);

private:
    void setupUI();
    void applySettings();

    QComboBox *m_resolutionCombo = nullptr;
    QCheckBox *m_fullscreenCheck = nullptr;
    QSpinBox *m_autoSaveIntervalBox = nullptr;
    QSpinBox *m_autoSaveCountBox = nullptr;
};

#endif // SETTINGSPAGE_H
