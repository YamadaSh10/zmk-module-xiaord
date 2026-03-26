/*
 * SPDX-License-Identifier: MIT
 *
 * Mouse emulation screen.
 *
 * Layout (screen 240×240, center 120,120):
 *
 *   Profile radio buttons on circle perimeter (r=UI_CIRCLE_LAYOUT_RADIUS, up to 5):
 *     slots 10,11,0,1,2 → 10,11,12,1,2 o'clock
 *
 *   Inner area (r < ~80 from center):
 *     Upper   : output status label (font 36, y=-40)
 *     Row 1   : [USB] [-33,15]  [TRASH] [+33,15]
 *     Row 2   : [⌂] [0,58]
 */

#include <lvgl.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include "page_iface.h"
#include "display_api.h"
#include "endpoint_status.h"
#include "ui_btn.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_BLE)
#include <zmk/ble.h>
#endif

/* ── State ──────────────────────────────────────────────────────────────── */

#define STATE_DEFAULT  0
#define STATE_TRACKPAD 1
#define STATE_SCROLL   2
static unsigned char mouse_state = STATE_DEFAULT;

/* ── Object ──────────────────────────────────────────────────────────────── */

static lv_obj_t *s_rclick_btn;
static lv_obj_t *s_lclick_btn;
static lv_obj_t *s_trackpad_btn;
static lv_obj_t *s_scroll_circle;

/* ── Callbacks ──────────────────────────────────────────────────────────── */
static void rclick_btn_cb(lv_event_t *e)
{
	if ((lv_event_get_code(e) != LV_EVENT_CLICKED) && (mouse_state != STATE_DEFAULT)) {
		return;
	}

	ss_fire_key_event(INPUT_BTN_RIGHT);
}

static void lclick_btn_cb(lv_event_t *e)
{
	if ((lv_event_get_code(e) != LV_EVENT_CLICKED) && (mouse_state != STATE_DEFAULT)) {
		return;
	}

	ss_fire_key_event(INPUT_BTN_LEFT);
}

static void home_btn_cb(lv_event_t *e)
{
	if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
		return;
	}
	ss_navigate_to(PAGE_HOME);
}

/* ── Page create ────────────────────────────────────────────────────────── */

static int page_mouse_create(lv_obj_t *screen)
{
	/* Row 1: LCLICK / RCLICK */
	ui_create_circle_btn(screen, LV_SYMBOL_LEFT,   -33, 15, lclick_btn_cb,  NULL);
	ui_create_circle_btn(screen, LV_SYMBOL_RIGHT,  33, 15, rclick_btn_cb,  NULL);

	/* Row 2: HOME */
	ui_create_circle_btn(screen, LV_SYMBOL_HOME,    0, 58, home_btn_cb, NULL);

	return 0;
}

/* ── Page ops ───────────────────────────────────────────────────────────── */

const struct page_ops page_mouse_ops = {
	.name     = "mouse",
	.create   = page_mouse_create,
	.on_enter = NULL,
	.on_leave = NULL,
};
