#ifndef STYLE_H
#define STYLE_H
// =============================================================================
// style.h — shared stylesheet constants
// =============================================================================

#include <QString>

namespace Style {

// ---- Palette ----------------------------------------------------------------
inline constexpr const char *kAccent   = "#e94560";
inline constexpr const char *kAccentH  = "#ff6b81";
inline constexpr const char *kAccentP  = "#533483";
inline constexpr const char *kPanelBg  = "#151525";
inline constexpr const char *kDarkBg   = "#0a0a12";
inline constexpr const char *kBorder   = "#2a2a4a";
inline constexpr const char *kText     = "#e8e8f0";
inline constexpr const char *kTextDim  = "#8888a0";
inline constexpr const char *kDanger   = "#ff6b81";

// ---- Global application base stylesheet -------------------------------------
inline const QString kAppStyle = QStringLiteral(
    "QWidget {"
    "  background-color: %1;"
    "  color: %2;"
    "  font-family: \"Microsoft YaHei\", \"SimHei\", sans-serif;"
    "}"
    "QLabel { background: transparent; }"
    "QGroupBox {"
    "  background-color: %3;"
    "  border: 1px solid %4;"
    "  border-radius: 8px;"
    "  padding: 8px;"
    "  margin-top: 8px;"
    "}"
    "QGroupBox::title {"
    "  color: %5;"
    "  padding: 0 6px;"
    "}");

// ---- Main menu button -------------------------------------------------------
inline const QString kMenuBtn = QStringLiteral(
    "QPushButton {"
    "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #1a1a3e,stop:1 #12122a);"
    "  color: %1;"
    "  border: 1px solid %2;"
    "  border-radius: 12px;"
    "  font-size: 18px;"
    "  font-weight: bold;"
    "  padding: 14px 48px;"
    "  min-width: 240px;"
    "}"
    "QPushButton:hover {"
    "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #2a2a5e,stop:1 #1a1a3e);"
    "  border-color: %3;"
    "  color: #ffffff;"
    "}"
    "QPushButton:pressed {"
    "  background: %4;"
    "}");

} // namespace Style

#endif // STYLE_H
