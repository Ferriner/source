/*
 * GainStrong MiniBox-V3 board support
 *
 * Copyright (c) 2012 Qualcomm Atheros
 * Copyright (c) 2012 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (c) 2016 Jens Steinhauser <jens.steinhauser@gmail.com>
 * Copyright (c) 2017 Dmitriy Ferriner <arknort@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define MINIBOX_V3_GPIO_BTN_RESET	17
#define MINIBOX_V3_GPIO_LED_SYSTEM	14

#define MINIBOX_V3_KEYS_POLL_INTERVAL	20 /* msecs */
#define MINIBOX_V3_KEYS_DEBOUNCE_INTERVAL (3 * MINIBOX_V3_KEYS_POLL_INTERVAL)

#define MINIBOX_V3_WMAC_CALDATA_OFFSET	0x1000

static const char *minibox_v3_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data minibox_v3_flash_data = {
	.part_probes = minibox_v3_part_probes,
};

static struct gpio_led minibox_v3_leds_gpio[] __initdata = {
	{
		.name		= "minibox-v3:green:system",
		.gpio		= MINIBOX_V3_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button minibox_v3_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = MINIBOX_V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MINIBOX_V3_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static void __init minibox_v3_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_m25p80(&minibox_v3_flash_data);
	ath79_register_leds_gpio(-1,
				 ARRAY_SIZE(minibox_v3_leds_gpio),
				 minibox_v3_leds_gpio);
	ath79_register_gpio_keys_polled(-1,
				        MINIBOX_V3_KEYS_POLL_INTERVAL,
				        ARRAY_SIZE(minibox_v3_gpio_keys),
				        minibox_v3_gpio_keys);

	ath79_register_mdio(0, 0x0);

	/* LAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, -1);
	ath79_register_eth(0);

	/* WAN */
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_register_eth(1);

	ath79_register_wmac(art + MINIBOX_V3_WMAC_CALDATA_OFFSET, mac);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_GS_MINIBOX_V3, "MINIBOX-V3", "MiniBox V3",
	     minibox_v3_setup);