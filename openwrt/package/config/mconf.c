/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 *
 * Introduced single menu mode (show all sub-menus in one large tree).
 * 2002-11-06 Petr Baudis <pasky@ucw.cz>
 *
 * Directly use liblxdialog library routines.
 * 2002-11-14 Petr Baudis <pasky@ucw.cz>
 */

#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/termios.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "dialog.h"

#define LKC_DIRECT_LINK
#include "lkc.h"

static char menu_backtitle[128];
static const char menu_instructions[] =
	"Arrow keys navigate the menu.  "
	"<Enter> selects submenus --->.  "
	"Highlighted letters are hotkeys.  "
	"Pressing <Y> selectes a feature, while <N> will exclude a feature.  "
	"Press <Esc><Esc> to exit, <?> for Help.  "
	"Legend: [*] feature is selected  [ ] feature is excluded",
radiolist_instructions[] =
	"Use the arrow keys to navigate this window or "
	"press the hotkey of the item you wish to select "
	"followed by the <SPACE BAR>. "
	"Press <?> for additional information about this option.",
inputbox_instructions_int[] =
	"Please enter a decimal value. "
	"Fractions will not be accepted.  "
	"Use the <TAB> key to move from the input field to the buttons below it.",
inputbox_instructions_hex[] =
	"Please enter a hexadecimal value. "
	"Use the <TAB> key to move from the input field to the buttons below it.",
inputbox_instructions_string[] =
	"Please enter a string value. "
	"Use the <TAB> key to move from the input field to the buttons below it.",
setmod_text[] =
	"This feature depends on another which has been configured as a module.\n"
	"As a result, this feature will be built as a module.",
nohelp_text[] =
	"There is no help available for this option.\n",
load_config_text[] =
	"Enter the name of the configuration file you wish to load.  "
	"Accept the name shown to restore the configuration you "
	"last retrieved.  Leave blank to abort.",
load_config_help[] =
	"\n"
	"For various reasons, one may wish to keep several different Buildroot\n"
	"configurations available on a single machine.\n"
	"\n"
	"If you have saved a previous configuration in a file other than the\n"
	"Buildroot's default, entering the name of the file here will allow you\n"
	"to modify that configuration.\n"
	"\n"
	"If you are uncertain, then you have probably never used alternate\n"
	"configuration files.  You should therefor leave this blank to abort.\n",
save_config_text[] =
	"Enter a filename to which this configuration should be saved "
	"as an alternate.  Leave blank to abort.",
save_config_help[] =
	"\n"
	"For various reasons, one may wish to keep different Buildroot\n"
	"configurations available on a single machine.\n"
	"\n"
	"Entering a file name here will allow you to later retrieve, modify\n"
	"and use the current configuration as an alternate to whatever\n"
	"configuration options you have selected at that time.\n"
	"\n"
	"If you are uncertain what all this means then you should probably\n"
	"leave this blank.\n",
top_menu_help[] =
	"\n"
	"Use the Up/Down arrow keys (cursor keys) to highlight the item\n"
	"you wish to change or submenu wish to select and press <Enter>.\n"
	"Submenus are designated by \"--->\".\n"
	"\n"
	"Shortcut: Press the option's highlighted letter (hotkey).\n"
	"\n"
	"You may also use the <PAGE UP> and <PAGE DOWN> keys to scroll\n"
	"unseen options into view.\n"
;

static char filename[PATH_MAX+1] = ".config";
static int indent = 0;
static struct termios ios_org;
static int rows, cols;
static struct menu *current_menu;
static int child_count;
static int single_menu_mode;

static struct dialog_list_item *items[16384]; /* FIXME: This ought to be dynamic. */
static int item_no;

static void conf(struct menu *menu);
static void conf_choice(struct menu *menu);
static void conf_string(struct menu *menu);
static void conf_load(void);
static void conf_save(void);
static void show_textbox(const char *title, const char *text, int r, int c);
static void show_helptext(const char *title, const char *text);
static void show_help(struct menu *menu);
static void show_readme(void);

static void init_wsize(void)
{
	struct winsize ws;
	char *env;

	if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
		rows = 24;
		cols = 80;
	} else {
		rows = ws.ws_row;
		cols = ws.ws_col;
		if (!rows) {
			env = getenv("LINES");
			if (env)
				rows = atoi(env);
			if (!rows)
				rows = 24;
		}
		if (!cols) {
			env = getenv("COLUMNS");
			if (env)
				cols = atoi(env);
			if (!cols)
				cols = 80;
		}
	}

	if (rows < 19 || cols < 80) {
		fprintf(stderr, "Your display is too small to run Menuconfig!\n");
		fprintf(stderr, "It must be at least 19 lines by 80 columns.\n");
		exit(1);
	}

	rows -= 4;
	cols -= 5;
}

static void cinit(void)
{
	item_no = 0;
}

static void cmake(void)
{
	items[item_no] = malloc(sizeof(struct dialog_list_item));
	memset(items[item_no], 0, sizeof(struct dialog_list_item));
	items[item_no]->tag = malloc(32); items[item_no]->tag[0] = 0;
	items[item_no]->name = malloc(512); items[item_no]->name[0] = 0;
	items[item_no]->namelen = 0;
	item_no++;
}

static int cprint_name(const char *fmt, ...)
{
	va_list ap;
	int res;

	if (!item_no)
		cmake();
	va_start(ap, fmt);
	res = vsnprintf(items[item_no - 1]->name + items[item_no - 1]->namelen,
			512 - items[item_no - 1]->namelen, fmt, ap);
	if (res > 0)
		items[item_no - 1]->namelen += res;
	va_end(ap);

	return res;
}

static int cprint_tag(const char *fmt, ...)
{
	va_list ap;
	int res;

	if (!item_no)
		cmake();
	va_start(ap, fmt);
	res = vsnprintf(items[item_no - 1]->tag, 32, fmt, ap);
	va_end(ap);

	return res;
}

static void cdone(void)
{
	int i;

	for (i = 0; i < item_no; i++) {
		free(items[i]->tag);
		free(items[i]->name);
		free(items[i]);
	}

	item_no = 0;
}

static void build_conf(struct menu *menu)
{
	struct symbol *sym;
	struct property *prop;
	struct menu *child;
	int type, tmp, doint = 2;
	tristate val;
	char ch;

	if (!menu_is_visible(menu))
		return;

	sym = menu->sym;
	prop = menu->prompt;
	if (!sym) {
		if (prop && menu != current_menu) {
			const char *prompt = menu_get_prompt(menu);
			switch (prop->type) {
			case P_MENU:
				child_count++;
				cmake();
				cprint_tag("m%p", menu);

				if (single_menu_mode) {
					cprint_name("%s%*c%s",
						menu->data ? "-->" : "++>",
						indent + 1, ' ', prompt);
				} else {
					cprint_name("   %*c%s  --->", indent + 1, ' ', prompt);
				}

				if (single_menu_mode && menu->data)
					goto conf_childs;
				return;
			default:
				if (prompt) {
					child_count++;
					cmake();
					cprint_tag(":%p", menu);
					cprint_name("---%*c%s", indent + 1, ' ', prompt);
				}
			}
		} else
			doint = 0;
		goto conf_childs;
	}

	cmake();
	type = sym_get_type(sym);
	if (sym_is_choice(sym)) {
		struct symbol *def_sym = sym_get_choice_value(sym);
		struct menu *def_menu = NULL;

		child_count++;
		for (child = menu->list; child; child = child->next) {
			if (menu_is_visible(child) && child->sym == def_sym)
				def_menu = child;
		}

		val = sym_get_tristate_value(sym);
		if (sym_is_changable(sym)) {
			cprint_tag("t%p", menu);
			switch (type) {
			case S_BOOLEAN:
				cprint_name("[%c]", val == no ? ' ' : '*');
				break;
			case S_TRISTATE:
				switch (val) {
				case yes: ch = '*'; break;
				case mod: ch = 'M'; break;
				default:  ch = ' '; break;
				}
				cprint_name("<%c>", ch);
				break;
			}
		} else {
			cprint_tag("%c%p", def_menu ? 't' : ':', menu);
			cprint_name("   ");
		}

		cprint_name("%*c%s", indent + 1, ' ', menu_get_prompt(menu));
		if (val == yes) {
			if (def_menu) {
				cprint_name(" (%s)", menu_get_prompt(def_menu));
				cprint_name("  --->");
				if (def_menu->list) {
					indent += 2;
					build_conf(def_menu);
					indent -= 2;
				}
			}
			return;
		}
	} else {
                if (menu == current_menu) {
                        cprint_tag(":%p", menu);
                        cprint_name("---%*c%s", indent + 1, ' ', menu_get_prompt(menu));
                        goto conf_childs;
                }

		child_count++;
		val = sym_get_tristate_value(sym);
		if (sym_is_choice_value(sym) && val == yes) {
			cprint_tag(":%p", menu);
			cprint_name("   ");
		} else {
			switch (type) {
			case S_BOOLEAN:
				cprint_tag("t%p", menu);
				if (sym_is_changable(sym))
					cprint_name("[%c]", val == no ? ' ' : '*');
				else
					cprint_name("---");
				break;
			case S_TRISTATE:
				cprint_tag("t%p", menu);
				switch (val) {
				case yes: ch = '*'; break;
				case mod: ch = 'M'; break;
				default:  ch = ' '; break;
				}
				if (sym_is_changable(sym))
					cprint_name("<%c>", ch);
				else
					cprint_name("---");
				break;
			default:
				cprint_tag("s%p", menu);
				tmp = cprint_name("(%s)", sym_get_string_value(sym));
				tmp = indent - tmp + 4;
				if (tmp < 0)
					tmp = 0;
				cprint_name("%*c%s%s", tmp, ' ', menu_get_prompt(menu),
					(sym_has_value(sym) || !sym_is_changable(sym)) ?
					"" : " (NEW)");
				goto conf_childs;
			}
		}
		cprint_name("%*c%s%s", indent + 1, ' ', menu_get_prompt(menu),
			(sym_has_value(sym) || !sym_is_changable(sym)) ?
			"" : " (NEW)");
		if (menu->prompt->type == P_MENU) {
			cprint_name("  --->");
			return;
		}
	}

conf_childs:
	indent += doint;
	for (child = menu->list; child; child = child->next)
		build_conf(child);
	indent -= doint;
}

static void conf(struct menu *menu)
{
	struct dialog_list_item *active_item = NULL;
	struct menu *submenu;
	const char *prompt = menu_get_prompt(menu);
	struct symbol *sym;
	char active_entry[40];
	int stat, type;

	unlink("lxdialog.scrltmp");
	active_entry[0] = 0;
	while (1) {
		indent = 0;
		child_count = 0;
  		current_menu = menu;
		cdone(); cinit();
		build_conf(menu);
		if (!child_count)
			break;
		if (menu == &rootmenu) {
			cmake(); cprint_tag(":"); cprint_name("--- ");
			cmake(); cprint_tag("L"); cprint_name("Load an Alternate Configuration File");
			cmake(); cprint_tag("S"); cprint_name("Save Configuration to an Alternate File");
		}
		dialog_clear();
		stat = dialog_menu(prompt ? prompt : "Main Menu",
				menu_instructions, rows, cols, rows - 10,
				active_entry, item_no, items);
		if (stat < 0)
			return;

		if (stat == 1 || stat == 255)
			break;

		active_item = first_sel_item(item_no, items);
		if (!active_item)
			continue;
		active_item->selected = 0;
		strncpy(active_entry, active_item->tag, sizeof(active_entry));
		active_entry[sizeof(active_entry)-1] = 0;
		type = active_entry[0];
		if (!type)
			continue;

		sym = NULL;
		submenu = NULL;
		if (sscanf(active_entry + 1, "%p", &submenu) == 1)
			sym = submenu->sym;

		switch (stat) {
		case 0:
			switch (type) {
			case 'm':
				if (single_menu_mode)
					submenu->data = (void *) (long) !submenu->data;
				else
					conf(submenu);
				break;
			case 't':
				if (sym_is_choice(sym) && sym_get_tristate_value(sym) == yes)
					conf_choice(submenu);
				else if (submenu->prompt->type == P_MENU)
					conf(submenu);
				break;
			case 's':
				conf_string(submenu);
				break;
			case 'L':
				conf_load();
				break;
			case 'S':
				conf_save();
				break;
			}
			break;
		case 2:
			if (sym)
				show_help(submenu);
			else
				show_readme();
			break;
		case 3:
			if (type == 't') {
				if (sym_set_tristate_value(sym, yes))
					break;
				if (sym_set_tristate_value(sym, mod))
					show_textbox(NULL, setmod_text, 6, 74);
			}
			break;
		case 4:
			if (type == 't')
				sym_set_tristate_value(sym, no);
			break;
		case 5:
			if (type == 't')
				sym_set_tristate_value(sym, mod);
			break;
		case 6:
			if (type == 't')
				sym_toggle_tristate_value(sym);
			else if (type == 'm')
				conf(submenu);
			break;
		}
	}
}

static void show_textbox(const char *title, const char *text, int r, int c)
{
	int fd;

	fd = creat(".help.tmp", 0777);
	write(fd, text, strlen(text));
	close(fd);
	while (dialog_textbox(title, ".help.tmp", r, c) < 0)
		;
	unlink(".help.tmp");
}

static void show_helptext(const char *title, const char *text)
{
	show_textbox(title, text, rows, cols);
}

static void show_help(struct menu *menu)
{
	const char *help;
	char *helptext;
	struct symbol *sym = menu->sym;

	help = sym->help;
	if (!help)
		help = nohelp_text;
	if (sym->name) {
		helptext = malloc(strlen(sym->name) + strlen(help) + 16);
		sprintf(helptext, "%s:\n\n%s", sym->name, help);
		show_helptext(menu_get_prompt(menu), helptext);
		free(helptext);
	} else
		show_helptext(menu_get_prompt(menu), help);
}

static void show_readme(void)
{
	show_helptext("Help", top_menu_help);
}

static void conf_choice(struct menu *menu)
{
	const char *prompt = menu_get_prompt(menu);
	struct menu *child;
	struct symbol *active;

	active = sym_get_choice_value(menu->sym);
	while (1) {
		current_menu = menu;
		cdone(); cinit();
		for (child = menu->list; child; child = child->next) {
			if (!menu_is_visible(child))
				continue;
			cmake();
			cprint_tag("%p", child);
			cprint_name("%s", menu_get_prompt(child));
			if (child->sym == sym_get_choice_value(menu->sym))
				items[item_no - 1]->selected = 1; /* ON */
			else if (child->sym == active)
				items[item_no - 1]->selected = 2; /* SELECTED */
			else
				items[item_no - 1]->selected = 0; /* OFF */
		}

		switch (dialog_checklist(prompt ? prompt : "Main Menu",
					radiolist_instructions, 15, 70, 6,
					item_no, items, FLAG_RADIO)) {
		case 0:
			if (sscanf(first_sel_item(item_no, items)->tag, "%p", &child) != 1)
				break;
			sym_set_tristate_value(child->sym, yes);
			return;
		case 1:
			if (sscanf(first_sel_item(item_no, items)->tag, "%p", &child) == 1) {
				show_help(child);
				active = child->sym;
			} else
				show_help(menu);
			break;
		case 255:
			return;
		}
	}
}

static void conf_string(struct menu *menu)
{
	const char *prompt = menu_get_prompt(menu);

	while (1) {
		char *heading;

		switch (sym_get_type(menu->sym)) {
		case S_INT:
			heading = (char *) inputbox_instructions_int;
			break;
		case S_HEX:
			heading = (char *) inputbox_instructions_hex;
			break;
		case S_STRING:
			heading = (char *) inputbox_instructions_string;
			break;
		default:
			heading = "Internal mconf error!";
			/* panic? */;
		}

		switch (dialog_inputbox(prompt ? prompt : "Main Menu",
					heading, 10, 75,
					sym_get_string_value(menu->sym))) {
		case 0:
			if (sym_set_string_value(menu->sym, dialog_input_result))
				return;
			show_textbox(NULL, "You have made an invalid entry.", 5, 43);
			break;
		case 1:
			show_help(menu);
			break;
		case 255:
			return;
		}
	}
}

static void conf_load(void)
{
	while (1) {
		switch (dialog_inputbox(NULL, load_config_text, 11, 55,
					filename)) {
		case 0:
			if (!dialog_input_result[0])
				return;
			if (!conf_read(dialog_input_result))
				return;
			show_textbox(NULL, "File does not exist!", 5, 38);
			break;
		case 1:
			show_helptext("Load Alternate Configuration", load_config_help);
			break;
		case 255:
			return;
		}
	}
}

static void conf_save(void)
{
	while (1) {
		switch (dialog_inputbox(NULL, save_config_text, 11, 55,
					filename)) {
		case 0:
			if (!dialog_input_result[0])
				return;
			if (!conf_write(dialog_input_result))
				return;
			show_textbox(NULL, "Can't create file!  Probably a nonexistent directory.", 5, 60);
			break;
		case 1:
			show_helptext("Save Alternate Configuration", save_config_help);
			break;
		case 255:
			return;
		}
	}
}

static void conf_cleanup(void)
{
	tcsetattr(1, TCSAFLUSH, &ios_org);
	unlink(".help.tmp");
}

static void winch_handler(int sig)
{
	struct winsize ws;

	if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
		rows = 24;
		cols = 80;
	} else {
		rows = ws.ws_row;
		cols = ws.ws_col;
	}

	if (rows < 19 || cols < 80) {
		end_dialog();
		fprintf(stderr, "Your display is too small to run Menuconfig!\n");
		fprintf(stderr, "It must be at least 19 lines by 80 columns.\n");
		exit(1);
	}

	rows -= 4;
	cols -= 5;

}

int main(int ac, char **av)
{
	int stat;
	char *mode;
	struct symbol *sym;

	conf_parse(av[1]);
	conf_read(NULL);

	sym = sym_lookup("VERSION", 0);
	sym_calc_value(sym);
	snprintf(menu_backtitle, 128, "Buildroot v%s Configuration",
		sym_get_string_value(sym));

	mode = getenv("MENUCONFIG_MODE");
	if (mode) {
		if (!strcasecmp(mode, "single_menu"))
			single_menu_mode = 1;
	}

	tcgetattr(1, &ios_org);
	atexit(conf_cleanup);
	init_wsize();
	init_dialog();
	signal(SIGWINCH, winch_handler);
	conf(&rootmenu);
	end_dialog();

	/* Restart dialog to act more like when lxdialog was still separate */
	init_dialog();
	do {
		stat = dialog_yesno(NULL,
				"Do you wish to save your new Buildroot configuration?", 5, 60);
	} while (stat < 0);
	end_dialog();

	if (stat == 0) {
		conf_write(NULL);
		printf("\n\n"
			"*** End of Buildroot configuration.\n"
			"*** Check the top-level Makefile for additional configuration options.\n\n");
	} else
		printf("\n\nYour Buildroot configuration changes were NOT saved.\n\n");

	return 0;
}
