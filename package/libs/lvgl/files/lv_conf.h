/**
 * @file lv_conf.h
 * LVGL configuration for the ucode panel binding, LVGL v9.3.0.
 *
 * Only values that differ from lv_conf_internal.h defaults are set here,
 * plus the few defaults that are load bearing and must not drift.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/*
 * RGB565 is formats[0] on panel-mipi-dbi when the firmware blob says r5g6b5.
 * At this depth mipi_dbi_fb_dirty() hands the buffer to mipi_dbi_command_buf()
 * with no conversion, so do not change this without checking the panel driver.
 */
#define LV_COLOR_DEPTH 16

/* libc malloc rather than a fixed LVGL pool, so LV_MEM_SIZE does not apply. */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/* Single threaded, driven by uloop from ucode. */
#define LV_USE_OS   LV_OS_NONE

#define LV_USE_LINUX_DRM 1

/*
 * The SoC exposes no render node, so GBM allocation cannot work. With this at
 * 0 the backend falls back to DRM dumb buffers, which is what we want.
 */
#define LV_LINUX_DRM_GBM_BUFFERS 0

#define LV_USE_EVDEV 1

/* Widgets the panel uses. All of these default to 1; pin the load bearing ones. */
#define LV_USE_ARC      1
#define LV_USE_BAR      1
#define LV_USE_CHART    1
#define LV_USE_LABEL    1
#define LV_USE_LINE     1
#define LV_USE_TILEVIEW 1

/* The boot applet redraws the logo the bootloader stages painted. */
#define LV_USE_IMAGE        1

/*
 * lv_qrcode_class derives from lv_canvas_class, so the canvas comes with it
 * whether or not anything draws on one directly. It renders into an I1 draw
 * buffer, which LV_DRAW_SW_SUPPORT_I1 already covers by default.
 */
#define LV_USE_CANVAS       1
#define LV_USE_QRCODE       1

/*
 * Screenshots, so a page can be looked at without standing in front of the
 * panel. lv_snapshot re-renders a widget tree into a buffer of its own, and
 * lodepng carries the encoder that turns it into a file. lodepng.c is gated as
 * one unit, so the decoder comes along with it.
 */
#define LV_USE_SNAPSHOT     1
#define LV_USE_LODEPNG      1

/*
 * Fonts are loaded at run time rather than compiled in, and
 * lv_binfont_create_from_buffer() reaches its buffer by wrapping it in a memfs
 * path. That is a driver serving memory: it registers a drive letter and opens
 * nothing on disk, so it does not undo the deliberate absence of a filesystem
 * driver that makes lodepng answer 78 for every filename.
 *
 * lv_init() registers it, so enabling it is the whole of the change.
 */
#define LV_USE_FS_MEMFS     1
#define LV_FS_MEMFS_LETTER  'M'

/*
 * Everything else off. This is not cosmetic: it drops the widget code and
 * keeps the style property lookup tables small.
 */
#define LV_USE_ANIMIMG      0
#define LV_USE_BUTTON       0
#define LV_USE_BUTTONMATRIX 0
#define LV_USE_CALENDAR     0
#define LV_USE_CHECKBOX     0
#define LV_USE_DROPDOWN     0
#define LV_USE_IMAGEBUTTON  0
#define LV_USE_KEYBOARD     0
#define LV_USE_LED          0
#define LV_USE_LIST         0
#define LV_USE_MENU         0
#define LV_USE_MSGBOX       0
#define LV_USE_ROLLER       0
#define LV_USE_SCALE        0
#define LV_USE_SLIDER       0
#define LV_USE_SPAN         0
#define LV_USE_SPINBOX      0
#define LV_USE_SPINNER      0
#define LV_USE_SWITCH       0
#define LV_USE_TABLE        0
#define LV_USE_TABVIEW      0
#define LV_USE_TEXTAREA     0
#define LV_USE_WIN          0

/*
 * The application ships its own type and loads it with lv.font_load(), so
 * nothing here is cut for anybody's design. Montserrat stays on as the default
 * only because LV_FONT_DEFAULT has to name a font that exists at link time:
 * lv_obj's default style dereferences it before any page has loaded a thing.
 */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

#endif /* LV_CONF_H */
