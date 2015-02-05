#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <mach/regs-gpio.h>
#include <mach/regs-lcd.h>

#include <mach/fb.h>
#if defined(CONFIG_DM9000)
#include <linux/dm9000.h>
#elif defined(CONFIG_DM9K)
#include <linux/dm9k.h>
#endif
#include <linux/platform_data/i2c-s3c2410.h>

#include <plat/clock.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/samsung-time.h>
#include "regs-mem.h"
#include "common.h"
#include "common-jz2440.h"

static struct map_desc jz2440_iodesc[] __initdata = {
		{
			.virtual	= (u32)S3C24XX_VA_ISA_WORD,
			.pfn		= __phys_to_pfn(S3C2410_CS2),
			.length		= 0x10000,
			.type		= MT_DEVICE,
		}, {
			.virtual	= (u32)S3C24XX_VA_ISA_WORD + 0x10000,
			.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
			.length		= SZ_4M,
			.type		= MT_DEVICE,
		}, {
			.virtual	= (u32)S3C24XX_VA_ISA_BYTE,
			.pfn		= __phys_to_pfn(S3C2410_CS2),
			.length		= 0x10000,
			.type		= MT_DEVICE,
		}, {
			.virtual	= (u32)S3C24XX_VA_ISA_BYTE + 0x10000,
			.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
			.length		= SZ_4M,
			.type		= MT_DEVICE,
		}
};

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE


static struct s3c2410_uartcfg jz2440_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
	/* IR port */
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = 0x3c5,
//		.ulcon	     = 0x43,
		.ulcon 	     = 0x33, /* modified by jyin */
		.ufcon	     = 0x51,
	}
};

/* LCD driver info */

static struct s3c2410fb_display jz2440_lcd_cfg __initdata = {

	.lcdcon5	= S3C2410_LCDCON5_FRM565 |
			  S3C2410_LCDCON5_INVVLINE |
			  S3C2410_LCDCON5_INVVFRAME |
			  S3C2410_LCDCON5_PWREN |
			  S3C2410_LCDCON5_HWSWP,

	.type		= S3C2410_LCDCON1_TFT,

	.width		= 240,
	.height		= 320,

	.pixclock	= 166667, /* HCLK 60 MHz, divisor 10 */
	.xres		= 240,
	.yres		= 320,
	.bpp		= 16,
	.left_margin	= 20,
	.right_margin	= 8,
	.hsync_len	= 4,
	.upper_margin	= 8,
	.lower_margin	= 7,
	.vsync_len	= 4,
};

static struct s3c2410fb_mach_info jz2440_fb_info __initdata = {
	.displays	= &jz2440_lcd_cfg,
	.num_displays	= 1,
	.default_display = 0,

#if 0
	/* currently setup by downloader */
	.gpccon		= 0xaa940659,
	.gpccon_mask	= 0xffffffff,
	.gpcup		= 0x0000ffff,
	.gpcup_mask	= 0xffffffff,
	.gpdcon		= 0xaa84aaa0,
	.gpdcon_mask	= 0xffffffff,
	.gpdup		= 0x0000faff,
	.gpdup_mask	= 0xffffffff,
#endif

	.lpcsel		= ((0xCE6) & ~7) | 1<<4,
};
/* DM9k */
#if defined (CONFIG_DM9K)
static struct resource jz2440_dm9k_resource[] = {

		[0] = DEFINE_RES_MEM(S3C24XX_DM9K_ADDR_BASE, 4),
		[1] = DEFINE_RES_MEM(S3C24XX_DM9K_DATA_BASE, 4),
		[2] = DEFINE_RES_NAMED(IRQ_EINT7, 1, NULL, IORESOURCE_IRQ \
									| IORESOURCE_IRQ_HIGHEDGE ),
};

/*
 * The DM9000 has no eeprom, and it's MAC address is set by
 * the bootloader before starting the kernel.
 */

static struct dm9000_plat_data jz2440_dm9k_pdata = {
	.flags		=  (DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM),
};

struct platform_device jz2440_device_dm9k = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz2440_dm9k_resource),
	.resource	= jz2440_dm9k_resource,
	.dev		= {
		.platform_data	= &jz2440_dm9k_pdata,
	},
};


#elif defined (CONFIG_DM9KS)
static struct resource jz2440_dm9k_resource[] = {
		[0] = DEFINE_RES_MEM(S3C24XX_DM9K_ADDR_BASE, SZ_4K),
		[1] = DEFINE_RES_NAMED(IRQ_EINT7, 1, NULL, IORESOURCE_IRQ \
									| IORESOURCE_IRQ_HIGHEDGE),

};
/*
 * The DM9000 has no eeprom, and it's MAC address is set by
 * the bootloader before starting the kernel.
 */

static struct dm9k_plat_data jz2440_dm9k_pdata = {
	.flags		= (DM9K_PLATF_16BITONLY | DM9K_PLATF_NO_EEPROM),
};

struct platform_device jz2440_device_dm9k = {
	.name		= "dm9k",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz2440_dm9k_resource),
	.resource	= jz2440_dm9k_resource,
	.dev		= {
		.platform_data	= &jz2440_dm9k_pdata,
	},
};

#endif

static struct platform_device *jz2440_devices[] __initdata = {
	&s3c_device_ohci,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c0,
	&s3c_device_iis,
	&jz2440_device_dm9k,
};

static void __init jz2440_map_io(void)
{
	s3c24xx_init_io(jz2440_iodesc, ARRAY_SIZE(jz2440_iodesc));
	s3c24xx_init_clocks(12000000);

	s3c24xx_init_uarts(jz2440_uartcfgs, ARRAY_SIZE(jz2440_uartcfgs));
	samsung_set_timer_source(SAMSUNG_PWM3, SAMSUNG_PWM4);
}


static void __init jz2440_machine_init(void)
{

	s3c24xx_fb_set_platdata(&jz2440_fb_info);
	s3c_i2c0_set_platdata(NULL);

	platform_add_devices(jz2440_devices, ARRAY_SIZE(jz2440_devices));
	jz2440_board_machine_init();
}

MACHINE_START(JZ2440, "JZ2440")
	/* Maintainer: Ben Dooks <ben-linux@fluff.org> */
	.atag_offset	= 0x100,
	.init_irq	= s3c2440_init_irq,
	.map_io		= jz2440_map_io,
	.init_machine	= jz2440_machine_init,
	.init_time	= samsung_timer_init,
	.restart	= s3c244x_restart,
	.init_time	= samsung_timer_init,
	.restart	= s3c244x_restart,
MACHINE_END


