# QX-MINI51 计数流水灯

适用于 QX-MINI51（STC89C52RC）的 Keil C51 示例程序。

## 功能

- P1.0–P1.7 的 LED 以约 200 ms 的间隔往返流水（低电平点亮）。
- 每次 LED 移动时，四位数码管显示值加 1；计数到 60 后回到 1。
- 每次按下 S1–S4，数码管立即加 1，并从 P3.6 输出约 180 ms 的不同音高：S1=C5、S2=D5、S3=E5、S4=G5。按键带约 20 ms 软件消抖，可连续松开再按。
- 使用 Timer0 中断扫描数码管。

## 文件

- `QX-MINI51-Time-Marquee.c`：Keil C51 源码。
- `QX-MINI51-Time-Marquee.uvproj`：Keil uVision 4 工程，双击即可打开和编辑源码。
- `QX-MINI51-Time-Marquee.hex`：可通过 STC-ISP 烧录的程序文件。
- `build.bat`：调用本机 Keil C51 工具重新编译并生成 HEX。
- `open-keil.bat`：打开 Keil uVision 工程。
- `check-tools.bat`：检查本机编译工具和串口。
- `flash.bat`：启动 STC-ISP，并提示要选择的 HEX；烧录仍由用户在 STC-ISP 中确认。
- `.vscode/tasks.json`：VS Code 中的一键检查、构建和打开 STC-ISP 任务。

## 本地部署与修改

在 Windows 安装 Git、Keil C51，并克隆仓库：

```bat
git clone https://github.com/duolinger01/QX-MINI51-Time-Marquee.git
cd QX-MINI51-Time-Marquee
check-tools.bat
build.bat
```

用 VS Code 打开克隆下来的文件夹，直接编辑 `QX-MINI51-Time-Marquee.c`。
常用调整项在文件顶部：`LED_STEP_INTERVAL_MS` 控制流水速度，
`KEY_DEBOUNCE_INTERVAL_MS` 控制按键消抖时间，`KEY_NOTE_DURATION_MS` 控制琴音时长，
`COUNTER_MAX_VALUE` 控制计数上限。修改后运行 `build.bat`，生成新的 HEX。
要在 Keil IDE 中编辑，双击 `QX-MINI51-Time-Marquee.uvproj`，或运行 `open-keil.bat`。
在 Keil 里打开源码后按 **F7**（或点击 Build）编译，HEX 会生成在仓库目录。
VS Code 的“运行生成任务”中也可选择 `Open Keil uVision project` 或
`Build firmware HEX`；后者直接调用 C51、BL51 和 OH51 命令行工具。

Keil 默认目录为 `C:\Keil_v5\C51\BIN`。若安装路径不同，在当前命令行设置：

```bat
set KEIL_C51_BIN=D:\Keil_v5\C51\BIN
```

若 uVision 4 安装路径不同，将 `KEIL_UV4_EXE` 设置为 `UV4.exe` 的完整路径；
`open-keil.bat` 会用该路径打开仓库里的 `.uvproj` 工程。

烧录时另外安装 STC-ISP，再运行 `flash.bat`。在 STC-ISP 中选择
`STC89C52RC/LE52RC` 和开发板对应的串口，打开仓库目录下生成的 HEX，
确认设置后手动点击“下载/编程”。此脚本不会自动擦写芯片。
若 STC-ISP 不在默认位置，设置 `STC_ISP_EXE` 为它的完整 exe 路径。

## 硬件

- MCU：STC89C52RC，外部时钟约 11.0592 MHz。
- 独立按键 S1–S4 分别接 P3.5、P3.4、P3.3、P3.2，按下为低电平。
- 蜂鸣器接 P3.6；LED 模块接通 J2，数码管接通 J6。

Keil C51 和 STC-ISP 是第三方工具，仓库不包含它们的安装包；请从各自官方渠道获取。
本示例未附带许可证声明。
