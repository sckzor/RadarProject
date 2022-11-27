/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "common/tusb_common.h"
#include "device/usbd.h"
#include "device/usbd_pvt.h"

#include "usb_descriptors.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

static void radar_driver_init(void);
static void radar_driver_reset(uint8_t);
static uint16_t radar_driver_open(uint8_t, tusb_desc_interface_t const*, uint16_t);
static int64_t led_on_callback(alarm_id_t, void*);
static bool radar_driver_control_xfer_cb(uint8_t, uint8_t, tusb_control_request_t const*);
static bool radar_driver_control_request(uint8_t, tusb_control_request_t const *);
static bool radar_driver_control_complete(uint8_t, tusb_control_request_t const *);
static bool radar_driver_xfer_cb(uint8_t, uint8_t, xfer_result_t, uint32_t);


static usbd_class_driver_t const _usbd_driver[] =
{
    {
  #if CFG_TUSB_DEBUG >= 2
        .name             = "Radar",
  #endif
        .init             = radar_driver_init,
        .reset            = radar_driver_reset,
        .open             = radar_driver_open,
        .control_xfer_cb  = radar_driver_control_xfer_cb,
        .xfer_cb          = radar_driver_xfer_cb,
        .sof              = NULL
    },
};

usbd_class_driver_t const* usbd_app_driver_get_cb(uint8_t* driver_count) {
	*driver_count += TU_ARRAY_SIZE(_usbd_driver);

	return _usbd_driver;
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * req) {
	printf("tud_vendor_control_xfer_cb\n");
	if (stage == CONTROL_STAGE_SETUP) {
		return radar_driver_control_request(rhport, req);
	} else if (stage == CONTROL_STAGE_DATA) {
		return radar_driver_control_complete(rhport, req);
	}
	return true;
}


int main() {
	board_init();
	tusb_init();

	while(1) {
		tud_task();
		//sensor_task();
	}

	return 0;
}

static void radar_driver_init(void) {
	printf("radar_driver_init\n");
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_put(LED_PIN, 1);
}

static void radar_driver_reset(uint8_t rhport) {
	printf("radar_driver_reset\n");
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_put(LED_PIN, 1);
}

static uint16_t radar_driver_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len) {
	printf("radar_driver_open\n");
	TU_VERIFY(TUSB_CLASS_VENDOR_SPECIFIC == itf_desc->bInterfaceClass, 0);
	
	uint16_t const drv_len = sizeof(tusb_desc_interface_t) + itf_desc->bNumEndpoints*sizeof(tusb_desc_endpoint_t);
	TU_VERIFY(max_len >= drv_len, 0);

	tusb_desc_endpoint_t const * desc_ep = (tusb_desc_endpoint_t const *) tu_desc_next(itf_desc);
	
	TU_ASSERT(usbd_edpt_open(rhport, desc_ep), 0);

	return drv_len;
}

static int64_t led_on_callback(alarm_id_t id, void *user_data) {
	gpio_put(LED_PIN, 1);
	return 0;
}

static bool radar_driver_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
	printf("radar_driver_control_xfer_cb\n");
	return false;
}

static bool radar_driver_control_request(uint8_t rhport, tusb_control_request_t const * request) {
	printf("radar_driver_control_request\n");
	gpio_put(LED_PIN, 0);
	add_alarm_in_ms(1000 * request->wValue, led_on_callback, NULL, false);
	return true;
}

static bool radar_driver_control_complete(uint8_t rhport, tusb_control_request_t const * request) {
	printf("radar_driver_control_complete\n");
	return true;
}

static bool radar_driver_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
	printf("radar_driver_xfer_cb\n");
	return true;
}
