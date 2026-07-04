# ST7565P_Parallel

STM32F103C8 (Blue Pill) 驱动 ST7565P LCD 显示屏，使用 **8080 并行接口** 通信，基于 PlatformIO + STM32Cube HAL 框架。

## 特性

- **8080 并口驱动** — 8 位数据总线，高速刷新
- **双缓冲架构** — 后台缓冲 + `LCDupdate()` 一次刷新
- **完整图形库** — 点、线、矩形、圆角矩形、圆形、三角形、位图
- **100+ 种字体** — 3px~64px，含等宽/衬线/无衬线/数码管/装饰字体
- **图标集** — 电池、信号、WiFi、蓝牙、锁、打印机等状态图标
- **旋转支持** — 0°/90°/180°/270° 显示方向
- **对比度调节** — 软件可调 `0x00` ~ `0xFE`
- **两套驱动** — C++ 类（推荐）和 C 风格接口

## 硬件连接

| LCD 引脚 | 功能 | STM32 引脚 |
|---------|------|-----------|
| CS | 片选 | PA8 |
| RST | 复位 | PA11 |
| DC | 数据/命令选择 | PB1 |
| WR | 写使能 | PB0 |
| RD | 读使能 | PB3 |
| D0 | 数据位 0 | PB8 |
| D1 | 数据位 1 | PB9 |
| D2 | 数据位 2 | PB10 |
| D3 | 数据位 3 | PB11 |
| D4 | 数据位 4 | PB12 |
| D5 | 数据位 5 | PB13 |
| D6 | 数据位 6 | PB14 |
| D7 | 数据位 7 | PB15 |
| LED | 背光 | 3.3V 串联电阻 |

调试串口：USART1 (PA9 TX, PA10 RX) 115200bps

## 快速开始

### 环境

- [PlatformIO](https://platformio.org/) (VS Code 插件或 CLI)
- JLink 调试器（用于烧录）
- ST7565P LCD 模块

### 编译 & 烧录

```bash
# 编译
pio run

# 烧录
pio run --target upload

# 串口监视
pio device monitor
```

## 使用示例

```cpp
#include "ST7565_Parallel.h"

// 定义引脚
LcdDataPin cs = {GPIOA, GPIO_PIN_8};
LcdDataPin rst = {GPIOA, GPIO_PIN_11};
LcdDataPin dc = {GPIOB, GPIO_PIN_1};
LcdDataPin wr = {GPIOB, GPIO_PIN_0};
LcdDataPin rd = {GPIOB, GPIO_PIN_3};
LcdDataPin dataPins[] = {
    {GPIOB, GPIO_PIN_8},  {GPIOB, GPIO_PIN_9},
    {GPIOB, GPIO_PIN_10}, {GPIOB, GPIO_PIN_11},
    {GPIOB, GPIO_PIN_12}, {GPIOB, GPIO_PIN_13},
    {GPIOB, GPIO_PIN_14}, {GPIOB, GPIO_PIN_15}
};

// 缓冲区（128x64 / 8 = 1024 bytes）
uint8_t screenBuffer[1024];

ST7565_Parallel mylcd(128, 64, cs, rst, dc, wr, rd, dataPins);
ST7565_Parallel_Screen fullScreen(screenBuffer, 128, 64, 0, 0);

void setup() {
    mylcd.ActiveBuffer = &fullScreen;
    mylcd.LCDbegin();
    mylcd.LCDclearBuffer();
    mylcd.setFontNum(UC1609Font_Default);

    // 绘制文本
    mylcd.drawText(0, 0, "Hello World", 1, 0, 1);
    // 绘制矩形
    mylcd.drawRect(0, 15, 64, 30, 1);
    // 填充矩形
    mylcd.fillRect(10, 10, 20, 20, 1);
    // 绘制圆角矩形
    mylcd.drawRoundRect(64, 15, 20, 30, 5, 1);
    // 绘制图标
    mylcd.LCD_DrawIcon_Battery(100);
    mylcd.LCD_DrawIcon_Signal(100);

    mylcd.LCDupdate();  // 刷新到屏幕
}
```

## API 参考

### 图形绘制

| 函数 | 说明 |
|------|------|
| `drawPixel(x, y, color)` | 画点 |
| `drawLine(x0, y0, x1, y1, color)` | 画线 |
| `drawRect(x, y, w, h, color)` | 矩形 |
| `fillRect(x, y, w, h, color)` | 填充矩形 |
| `drawRoundRect(x, y, w, h, r, color)` | 圆角矩形 |
| `fillRoundRect(x, y, w, h, r, color)` | 填充圆角矩形 |
| `drawCircle(x0, y0, r, color)` | 圆形 |
| `fillCircle(x0, y0, r, color)` | 填充圆形 |
| `drawTriangle(x0,y0, x1,y1, x2,y2, color)` | 三角形 |
| `fillTriangle(...)` | 填充三角形 |
| `drawBitmap(x, y, bitmap, w, h, color, bg)` | 位图 |

### 文本渲染

| 函数 | 说明 |
|------|------|
| `setFontNum(font)` | 选择字体 |
| `setTextColor(fg, bg)` | 设置颜色 |
| `setTextSize(size)` | 设置字号 |
| `drawChar(x, y, c, color, bg)` | 显示字符 |
| `drawText(x, y, text, color, bg, size)` | 显示字符串 |

### 显示控制

| 函数 | 说明 |
|------|------|
| `LCDbegin()` | 初始化 LCD |
| `LCDupdate()` | 缓冲区刷新到屏幕 |
| `LCDclearBuffer()` | 清空缓冲区 |
| `LCDFillScreen(pixel)` | 填充屏幕 |
| `LCD_Contrast(val)` | 调节对比度 (0x00~0xFE) |
| `LCDInvertDisplay(on)` | 反转显示 |
| `setRotation(deg)` | 设置旋转 (0/90/180/270) |

### 图标

`LCD_DrawIcon_Battery(level)` — 电量 0~100
`LCD_DrawIcon_Signal(level)` — 信号 0~100
`LCD_DrawIcon_Lock(status)` — 锁状态
`LCD_DrawIcon_Upload(status)` — 上传
`LCD_DrawIcon_Download(status)` — 下载
`LCD_DrawIcon_Card(status)` — SD 卡
`LCD_DrawIcon_Printer(status)` — 打印机
`LCD_DrawIcon_phone(icon)` — 电话

### 颜色

```cpp
#define FOREGROUND    1   // 亮（白）
#define BACKGROUND    0   // 灭（黑）
#define COLORINVERSE  2   // 反转
```

## 字体列表

项目包含超过 100 种字体文件，位于 `Core/Inc/fonts/` 和 `Core/Inc/AlphaNumeric/`：

- **等宽** — Font_*_Mono, Font_*_Inconsola
- **无衬线** — Font_*_Sans, Font_*_Grotesk, Font_*_Ubuntu
- **衬线** — Font_*_Serif
- **数码管** — Font_*_Segment_*
- **装饰** — Font_*_Sinclair, Font_*_Retro, Font_*_Matrix
- **超小** — Font_3_Tiny, Font_3_Picopixel, Font_5_Org

使用 `setFontNum()` 选择，内置 12 种快捷枚举：

| 枚举 | 说明 |
|------|------|
| `UC1609Font_Default` | 默认 5x8 全 ASCII |
| `UC1609Font_Thick` | 粗体（无小写） |
| `UC1609Font_Seven_Seg` | 七段数码管 |
| `UC1609Font_Wide` | 宽体（无小写） |
| `UC1609Font_Tiny` | 超小 3x5 |
| `UC1609Font_Homespun` | 手写风格 |
| `UC1609Font_Bignum` | 大数字 |
| `UC1609Font_Mednum` | 中数字 |
| `UC1609Font_ArialRound` | Arial Rounded |
| `UC1609Font_ArialBold` | Arial Bold |
| `UC1609Font_Mia` | Mia |
| `UC1609Font_Dedica` | Dedica |

## 项目结构

```
Core/
├── Inc/
│   ├── ST7565_Parallel.h       # C++ 并口驱动头文件
│   ├── ST7565_graphics.h       # 图形基类
│   ├── ST7565_graphics_font.h  # 字体渲染
│   ├── st7565.h                # C 风格驱动头文件
│   ├── fonts/                  # 字体数据 (90+)
│   └── AlphaNumeric/           # 额外字体
├── Src/
│   ├── main.cpp                # 主程序
│   ├── ST7565_Parallel.cpp     # 并口驱动实现
│   ├── ST7565_graphics.cpp     # 图形实现
│   ├── ST7565_graphics_font.cpp
│   └── st7565.cpp              # C 驱动实现
lib/                            # 另一组 C 驱动备份
```

## 技术细节

- **分辨率**: 128x64 像素
- **驱动 IC**: UC1609C (与 ST7565P 兼容)
- **接口**: 8080 并行 (8 位数据总线)
- **帧缓冲**: 1024 bytes (128 × 64 / 8)
- **系统时钟**: HSE 8MHz → PLL x9 → 72MHz
- **烧录方式**: JLink / SWD
