# ST7565P_Parallel 项目指南

## 项目概述

STM32F103C8 (Blue Pill) 驱动 ST7565P LCD 显示屏（128x64 单色），
使用 **8080 并行接口** 通信。基于 PlatformIO + STM32Cube HAL 框架。

## 硬件连接

| 引脚 | 功能 | GPIO |
|------|------|------|
| CS | 片选 | PA8 |
| RST | 复位 | PA11 |
| DC | 数据/命令 | PB1 |
| WR | 写使能 | PB0 |
| RD | 读使能 | PB3 |
| D0-D7 | 8位数据总线 | PB8-PB15 |
| USART1 TX | 调试输出 | PA9 (115200) |
| PC13 | 板载 LED | 输出 |

## 构建 & 烧录

```bash
# 编译
pio run

# 烧录 (JLink)
pio run --target upload

# 串口监视
pio device monitor
```

## 项目结构

```
Core/
├── Inc/
│   ├── ST7565_Parallel.h      # C++ 类驱动 (并口，推荐使用)
│   ├── ST7565_graphics.h      # 图形基类 (drawPixel, drawLine, fillRect 等)
│   ├── ST7565_graphics_font.h # 字体渲染
│   ├── st7565.h               # C 风格驱动 (struct 方式)
│   ├── fonts/                 # 大量 .h 格式字体文件
│   └── AlphaNumeric/          # 额外的字体文件
├── Src/
│   ├── main.cpp               # 主程序入口
│   ├── ST7565_Parallel.cpp    # 并口驱动实现
│   ├── ST7565_graphics.cpp    # 图形绘制实现
│   ├── ST7565_graphics_font.cpp
│   └── st7565.cpp             # C 风格驱动实现
├── lib/
│   ├── st7565.c / .h          # 另一组 C 驱动
│   └── st7565_graphics.cpp / .h
```

## 两套驱动

### 1. ST7565_Parallel (C++ 类，推荐)

继承 `ST7565_graphics` 图形基类，面向对象设计。

```cpp
// 初始化
ST7565_Parallel mylcd(128, 64, cs, rst, dc, wr, rd, dataPins);
ST7565_Parallel_Screen fullScreen(buffer, 128, 64, 0, 0);

mylcd.ActiveBuffer = &fullScreen;
mylcd.LCDbegin();
mylcd.LCDclearBuffer();
mylcd.setFontNum(UC1609Font_Default);

// 绘图
mylcd.drawPixel(x, y, color);
mylcd.drawLine(x0, y0, x1, y1, color);
mylcd.fillRect(x, y, w, h, color);
mylcd.drawCircle(x0, y0, r, color);
mylcd.drawText(x, y, "Hello", color, bg, size);

// 刷新显示
mylcd.LCDupdate();
```

### 2. st7565.h (C 风格)

struct 方式定义引脚，纯 C 接口，适合 C 项目。

## 关键 API

### 图形函数 (ST7565_graphics)
- `drawPixel(x, y, color)` — 画点
- `drawLine(x0, y0, x1, y1, color)` — 画线
- `drawRect(x, y, w, h, color)` — 矩形
- `fillRect(x, y, w, h, color)` — 填充矩形
- `drawCircle(x0, y0, r, color)` — 圆形
- `fillCircle(x0, y0, r, color)` — 填充圆形
- `drawTriangle(x0,y0, x1,y1, x2,y2, color)` — 三角形
- `fillTriangle(...)` — 填充三角形
- `drawRoundRect(x, y, w, h, r, color)` — 圆角矩形
- `drawBitmap(x, y, bitmap, w, h, color, bg)` — 位图
- `drawChar(x, y, c, color, bg)` — 字符
- `drawText(x, y, text, color, bg, size)` — 文本

### 显示函数 (ST7565_Parallel)
- `LCDbegin()` — 初始化 LCD
- `LCDupdate()` — 将缓冲区刷到屏幕
- `LCDclearBuffer()` — 清除缓冲区
- `LCDFillScreen(pixel)` — 填充屏幕
- `LCD_Contrast(val)` — 调节对比度
- `LCDInvertDisplay(on)` — 反转显示
- `LCDBuffer(x, y, w, h, data)` — 写入自定义数据
- `LCDBuffer_Icon(x, y, w, h, data)` — 写入图标
- `LCD_DrawIcon_Battery(level)` — 电池图标
- `LCD_DrawIcon_Signal(level)` — 信号图标
- `LCD_DrawIcon_Lock(status)` — 锁图标
- `LCD_DrawIcon_Upload/Download/Card/Printer/Phone` — 其他图标

### 颜色常量
```cpp
#define FOREGROUND 1   // 白/亮
#define BACKGROUND 0   // 黑/灭
#define COLORINVERSE 2 // 反转
```

### 字体设置
```cpp
mylcd.setFontNum(UC1609Font_Default);  // 默认 5x8
// 可选: UC1609Font_Thick, UC1609Font_Seven_Seg,
//       UC1609Font_Wide, UC1609Font_Tiny, UC1609Font_ArialRound,
//       UC1609Font_ArialBold, 等
```

## 注意

- 双缓冲：`screenBuffer[1536]` 作为后台缓冲，`LCDupdate()` 刷新到 LCD
- 像素颜色：`1`=亮、`0`=灭、`2`=反转
- 对比度调节范围 `0x00`~`0xFE`，默认 `0x20`
- 旋转支持：`LCD_Degrees_0/90/180/270`
- 系统时钟：HSE 8MHz → PLL x9 → 72MHz
