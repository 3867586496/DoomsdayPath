# 末日迷途 更新日志

## v0.3.0 — 物品与背包系统 (2026-05-29)

### 新增
- **Item 类**：物品系统，支持名称、描述、重量、属性和可食用标记。功能效果可量化（通过 StatChange）。
- **Inventory 类**：背包系统，负重上限 30，addItem/removeItem/itemAt/canAdd，changed() 信号驱动 UI 刷新。
- **BackpackPage**：背包界面，第五个页面。顶部显示负重，表格三列（名称/功能/操作），可食用物品有「食用」按钮，所有物品可「丢弃」。
- 测试物品：面包（+10 饥饿，-5 口渴，0.1kg）、木板（0.5kg，无效果）。

### 变更
- GamePage 新增 m_inventory 成员和「背包」按钮，emit openBackpack() 切换到背包界面。
- MainWindow 新增 BackpackPage（index 4），连接 itemUsed → applyItemEffects 应用物品效果到属性。
- CMakeLists.txt 新增 Item、Inventory、BackpackPage 源文件。

## v0.2.0 — 游戏界面与数据层 (2026-05-26)

### 新增
- **GameTime**：游戏时间系统，精确到分钟，显示「第N天 HH:MM」，advance(minutes) 推进时间。
- **PlayerStats**：玩家属性（生命/饥饿/口渴/理智），上限 100，带 clamp。
- **Action**：行动定义（名称、描述、消耗列表、获得列表、时间消耗），标准化 StatChange。
- **GamePage**：游戏主界面，左上状态面板，右上时间，分隔线，功能测试区，返回按钮。
- **DifficultyFactor**：难度因子，4 档预设（简单 0.5/正常 1.0/困难 2.0/地狱 5.0）+ 自定义。

### 变更
- LoadingPage 加入 QProgressBar 长条形加载条。
- MainWindow QStackedWidget 管理 4 个页面。

## v0.1.0 — 项目初始化 (2026-05-26)

### 新增
- CMake 构建系统（C++17, Qt6 Widgets/Core/Gui）。
- MainWindow + MainMenuPage + NewGamePage + LoadingPage 四个页面。
- 开始新游戏 / 读取进度 / 退出 菜单入口。
- 暗色主题 UI（#1a1a2e 背景，#e94560 强调色）。
