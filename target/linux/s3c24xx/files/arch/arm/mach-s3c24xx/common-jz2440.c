/*
 * common-jz2440.c
 *
 *  Created on: Dec 24, 2014
 *      Author: jyin
 */



#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/io.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <asm/irq.h>

#include <mach/regs-gpio.h>
#include <mach/gpio-samsung.h>
#include <linux/platform_data/leds-s3c24xx.h>
#include <linux/platform_data/mtd-nand-s3c2410.h>

#include <plat/gpio-cfg.h>
#include <plat/devs.h>
#include <plat/pm.h>

#include "common-smdk.h"

/* LED devices */

static struct s3c24xx_led_platdata jz2440_pdata_led4 = {
	.gpio		= S3C2410_GPF(4),
	.flags		= S3C24XX_LEDF_ACTLOW | S3C24XX_LEDF_TRISTATE,
	.name		= "led4",
	.def_trigger	= "timer",
};

static struct s3c24xx_led_platdata jz2440_pdata_led5 = {
	.gpio		= S3C2410_GPF(5),
	.flags		= S3C24XX_LEDF_ACTLOW | S3C24XX_LEDF_TRISTATE,
	.name		= "led5",
	.def_trigger	= "nand-disk",
};

static struct s3c24xx_led_platdata jz2440_pdata_led6 = {
	.gpio		= S3C2410_GPF(6),
	.flags		= S3C24XX_LEDF_ACTLOW | S3C24XX_LEDF_TRISTATE,
	.name		= "led6",
};

static struct s3c24xx_led_platdata jz2440_pdata_led7 = {
	.gpio		= S3C2410_GPF(7),
	.flags		= S3C24XX_LEDF_ACTLOW | S3C24XX_LEDF_TRISTATE,
	.name		= "led7",
};

static struct platform_device jz2440_led4 = {
	.name		= "s3c24xx_led",
	.id		= 0,
	.dev		= {
		.platform_data = &jz2440_pdata_led4,
	},
};

static struct platform_device jz2440_led5 = {
	.name		= "s3c24xx_led",
	.id		= 1,
	.dev		= {
		.platform_data = &jz2440_pdata_led5,
	},
};

static struct platform_device jz2440_led6 = {
	.name		= "s3c24xx_led",
	.id		= 2,
	.dev		= {
		.platform_data = &jz2440_pdata_led6,
	},
};

static struct platform_device jz2440_led7 = {
	.name		= "s3c24xx_led",
	.id		= 3,
	.dev		= {
		.platform_data = &jz2440_pdata_led7,
	},
};

/* NAND parititon from 2.4.18-swl5 */

static struct mtd_partition jz2440_default_nand_part[] = {
	[0] = {
		.name	= "bootloader", 	/* uboot size 256k */
		.size	= 0x00040000,
		.offset	= 0,
	},
	[1] = {
		.name	= "param",
		.offset = MTDPART_OFS_APPEND,
		.size	= 0x00020000,		/* boot params size 128k */
	},
	[2] = {
		.name	= "kernel",
		.offset = MTDPART_OFS_APPEND,
		.size	= 0x00700000,		/* kernel size 4M */
	},
	[3] = {
		.name	= "root",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct s3c2410_nand_set jz2440_nand_sets[] = {
	[0] = {
		.name		= "NAND",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(jz2440_default_nand_part),
		.partitions	= jz2440_default_nand_part,
	},
};

/* choose a set of timings which should suit most 512Mbit
 * chips and beyond.
*/

static struct s3c2410_platform_nand jz2440_nand_info = {
	.tacls		= 20,
	.twrph0		= 60,
	.twrph1		= 20,
	.nr_sets	= ARRAY_SIZE(jz2440_nand_sets),
	.sets		= jz2440_nand_sets,
};



/* devices we initialise */

static struct platform_device __initdata *jz2440_devs[] = {
	&s3c_device_nand,
	&jz2440_led4,
	&jz2440_led5,
	&jz2440_led6,
	&jz2440_led7,
};

static const struct gpio jz2440_led_gpios[] = {
	{ S3C2410_GPF(4), GPIOF_OUT_INIT_HIGH, NULL },
	{ S3C2410_GPF(5), GPIOF_OUT_INIT_HIGH, NULL },
	{ S3C2410_GPF(6), GPIOF_OUT_INIT_HIGH, NULL },
	{ S3C2410_GPF(7), GPIOF_OUT_INIT_HIGH, NULL },
};



void __init jz2440_board_machine_init(void)
{
	/* Configure the LEDs (even if we have no LED support)*/

	int ret = gpio_request_array(jz2440_led_gpios,
				     ARRAY_SIZE(jz2440_led_gpios));
	if (!WARN_ON(ret < 0))
		gpio_free_array(jz2440_led_gpios, ARRAY_SIZE(jz2440_led_gpios));

	if (machine_is_smdk2443())
		jz2440_nand_info.twrph0 = 50;

	s3c_nand_set_platdata(&jz2440_nand_info);

	platform_add_devices(jz2440_devs, ARRAY_SIZE(jz2440_devs));

	s3c_pm_init();
}
