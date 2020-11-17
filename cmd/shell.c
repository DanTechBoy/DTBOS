/**

________  _________  ________  ________  ________
|\   ___ \|\___   ___\\   __  \|\   __  \|\   ____\
\ \  \_|\ \|___ \  \_\ \  \|\ /\ \  \|\  \ \  \___|_
\ \  \ \\ \   \ \  \ \ \   __  \ \  \\\  \ \_____  \
 \ \  \_\\ \   \ \  \ \ \  \|\  \ \  \\\  \|____|\  \
	\ \_______\   \ \__\ \ \_______\ \_______\____\_\  \
	 \|_______|    \|__|  \|_______|\|_______|\_________\
																					 \|_________|

shell.c - DTBOS shell


 * SPDX-FileCopyrightText: 2020 Anuradha Weeraman <anuradha@weeraman.com> & 2020 DanTechBoy
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cmd/shell.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <mm/region.h>
#include <sys/panic.h>

#if ARCH_X86
#include <ppm/splash.h>
#include <sys/tty.h>
#include <x86/io.h>
#include <x86/boot/modules.h>
#include <x86/boot/cpuid.h>
#endif

static char line[MAX_CMD_LENGTH];
static int  retcode;

static const struct kcmd valid_cmds[] = {
#if ARCH_X86
	{ .cmd="clear",     .func=cmd_clear             },
	{ .cmd="splash",    .func=cmd_splash            },
	{ .cmd="exception", .func=cmd_trigger_exception },
	{ .cmd="modules",   .func=cmd_modules           },
	{ .cmd="canary",    .func=cmd_canary            },
	{ .cmd="dtbfetch",  .func=cmd_dtbfetch         },

#endif
	{ .cmd="exit",      .func=cmd_exit              },
	{ .cmd="help",      .func=cmd_help              },
	{ .cmd="osver",     .func=cmd_osver             },
	{ .cmd="ret",       .func=cmd_ret               },
	{ .cmd="memmap",    .func=cmd_memmap            },
};

#if ARCH_X86
int cmd_modules()
{
	print_boot_modules();
	return 0;
}

int cmd_canary()
{
	size_t i = 0;

	void (*canary)() = (void *) get_module_by_idx(0);

	if (canary == 0) {
		printk("Module not found!\n");
		return 1;
	}

	(*canary)();
	asm("\t movl %%eax, %0" : "=r"(i));

	if (i != CANARY_MAGIC_STRING) {
		printk("The canary is dead.\n");
		return 1;
	}

	printk("The canary is alive.\n");
	return 0;
}

int cmd_clear()
{
	clear_screen();
	return 0;
}

int cmd_trigger_exception()
{
	// Call panic function
	panic("User initiated crash");

	// Won't get here
	return 0;
}

int cmd_splash()
{

#ifdef CONFIG_FRAMEBUFFER_RGB

	clear_screen();

	struct fb_info *fbi = get_fb_info();
	size_t pad_cols = 0;
	size_t pad_rows = 0;

	if ((fbi->width - PPM_COLS) > 0) {
		if ((fbi->height - PPM_ROWS) > 0) {
			pad_cols = (fbi->width  / 2) - (PPM_COLS / 2);
			pad_rows = (fbi->height / 2) - (PPM_ROWS / 2);
		}
	}

	for (int prows = 0; prows < PPM_ROWS; prows++) {
		for (int pcols = 0; pcols < PPM_COLS; pcols++) {

			size_t pos = ((prows * PPM_COLS) + pcols) * 3;
			size_t ci  = ((ppm_array[pos]   << 16) & 0xff0000) |
				     ((ppm_array[pos+1] << 8)  & 0xff00)   |
				      (ppm_array[pos+2]        & 0xff);

			draw_pixel(pad_rows + prows, pad_cols + pcols, ci);
		}
	}


	getchar(); /* block on input */

	clear_screen();

#endif /* CONFIG_FRAMEBUFFER_RGB */

	return 0;
}
#endif /* ARCH_X86 */

int cmd_exit()
{
	// TODO Use ACPI for shutdown on real hardware

#if ARCH_X86
	// in QEMU
	outw(0x604, 0x2000);

	// in Virtualbox
	outw(0x4004, 0x3400);
	// Now safe to power off
	printk("\nIt is now safe to turn off the computer.");
	// halt the cpu
	while(1) {
		asm("hlt");
	}

#endif

	// We won't get here unless architecture is arm
	return 0;
}

int cmd_help()
{
	printk("Available commands:\n");
	for (size_t i = 0; i < (sizeof(valid_cmds) / sizeof(struct kcmd)); i++) {
		printk("  %s\n", valid_cmds[i].cmd);
	}
	return 0;
}

int cmd_osver()
{
	printk("DTBOS v%s\n",
		STRINGIFY(CONFIG_VERSION_MAJOR) "." \
		STRINGIFY(CONFIG_VERSION_MINOR));
	return 0;
}

int cmd_ret()
{
	printk("%d\n", retcode);
	return 0;
}

int cmd_memmap()
{
	print_mem_regions();
	return 0;
}

static int run(const char *cmdline)
{
	for (size_t i = 0; i < (sizeof(valid_cmds) / sizeof(struct kcmd)); i++) {
		if (strncmp(valid_cmds[i].cmd, line, MAX_CMD_LENGTH) == 0)
			return (*valid_cmds[i].func)();
		else if (strncmp("", line, MAX_CMD_LENGTH) == 0)
			return 0;
	}

	printk ("Unknown command: %s\n", cmdline);
	return 1;
}

int cmd_dtbfetch()
{
	printk(R"EOF(
		________  _________  ________  ________  ________
	  |\   ___ \|\___   ___\\   __  \|\   __  \|\   ____\
	  \ \  \_|\ \|___ \  \_\ \  \|\ /\ \  \|\  \ \  \___|_
	  \ \  \ \\ \   \ \  \ \ \   __  \ \  \\\  \ \_____  \
	   \ \  \_\\ \   \ \  \ \ \  \|\  \ \  \\\  \|____|\  \
	  	\ \_______\   \ \__\ \ \_______\ \_______\____\_\  \
	  	 \|_______|    \|__|  \|_______|\|_______|\_________\
 )EOF");

 printk("\n\n\n\n");


	printk("Kernel version: v%s\n",
	STRINGIFY(CONFIG_VERSION_MAJOR) "." \
	STRINGIFY(CONFIG_VERSION_MINOR-dtbos));
	cpuidvendor();
	printk("Compiled on: %s\n",
	STRINGIFY(__DATE__));
	return 0;
}

void start_interactive_shell()
{

	printk("\n\nWelcome to DTBOS v%s\n",
		STRINGIFY(CONFIG_VERSION_MAJOR) "." \
		STRINGIFY(CONFIG_VERSION_MINOR));

	printk("Type help for a list of commands\n");


#if (CONFIG_KEYBOARD || CONFIG_SERIAL) && ARCH_X86
	while(1) {
		printk("# ");
		getstr(line, MAX_CMD_LENGTH);
		retcode = run(line);
	}
#elif ARCH_ARM
	printk("Serial input for ARM not supported yet\n");
#else
	printk("Enable keyboard driver for interactive shell\n");

	while(1) {
#if ARCH_X86
		asm("hlt");
#endif /* ARCH_X86 */
	}
#endif /* (CONFIG_KEYBOARD || CONFIG_SERIAL) && ARCH_ARM */
}
