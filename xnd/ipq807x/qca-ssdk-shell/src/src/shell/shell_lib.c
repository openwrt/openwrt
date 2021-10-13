/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include "shell_config.h"
#include "shell.h"

#define printc(isPrint, fmt, args...) if(isPrint == 1) printf(fmt, ##args)
#ifndef PROMPT_STR
#define PROMPT_STR "dev0@qca>"
#endif
static char *cmd_promptp = PROMPT_STR;       /*command prompt pointer */
static struct termios term_save;    /* terminal setting saved */
static int term_cursor;         /*terminal position */
static int cmd_cursor;          /*command position */
static char *cmd_strp;          /*command string pointer */
static int cmd_strlen;          /*command string length */

#define HISTORY_MAX_SIZE   20
static char *history_buf[HISTORY_MAX_SIZE + 1]; /* history buffer */
static int history_nr;          /* saved history lines */
static int history_cur;         /* current pointer to history line */

static void term_config(void);
static void term_restore(void);
static void term_sig_handler(int sig);
static void term_init(char *out_cmd);
static void term_echo(void);
static void cursor_backward(void);
static void cursor_forward(void);
static void handle_backward(void);
static void handle_delete(void);
static void handle_tab(void);
static void handle_up(void);
static void handle_down(void);
static void handle_left(void);
static void handle_right(void);
static void handle_normal(char *out_cmd, char c);
static void handle_help(void);
static void prompt_print(void);
static void out_cmd_print(void);
static int history_prev(void);
static int history_next(void);
static void history_record(char *out_cmd);


struct key_bind_t
{
    int is_eseq;                /*is escape sequence */
    int key_last;               /*relative key or escape sequence last character */
    void (*func) ();
};

struct key_bind_t key_bind[] =
{
    {0, '\b', handle_backward},
    {0, 127, handle_delete},
    {0, '\t', handle_tab},
    {0, '?', handle_help},
    {1, 'A', handle_up},
    {1, 'B', handle_down},
    {1, 'C', handle_right},
    {1, 'D', handle_left},
};

/* saving orignal setting and set new attrib to terminal*/
static void
term_config(void)
{
    struct termios term_tmp;
    tcgetattr(0, (void *) &term_save);
    memcpy(&term_tmp, &term_save, sizeof (struct termios));

    /*unbuffered input and turn off echo */
    term_tmp.c_lflag &= ~(ICANON | ECHO | ECHONL);

    tcsetattr(0, TCSANOW, (void *) &term_tmp);
}

/*restore termial setting*/
static void
term_restore(void)
{
    tcsetattr(0, TCSANOW, (void *) &term_save);
}

/*termial signal handler*/
static void
term_sig_handler(int sig)
{
    switch (sig)
    {
        case SIGINT:
            if (cmd_promptp)
                printf("\n%s", cmd_promptp);

            while (cmd_strlen-- > 0)
                cmd_strp[cmd_strlen] = '\0';

            cmd_strp[0] = '\0';
            cmd_strlen = 0;
			if (cmd_promptp)
				term_cursor = strlen(cmd_promptp);
            cmd_cursor = 0;

            fflush(stdout);

            break;

        case SIGUSR1:
        case SIGKILL:
        case SIGABRT:
        case SIGTERM:
        case SIGHUP:
            printf("exit.\n");
            term_restore();
            exit(0);
        default:
            break;
    }
}

/*termial initial*/
static void
term_init(char *out_cmd)
{
    term_cursor = 0;
    cmd_cursor = 0;

    cmd_strp = out_cmd;
    cmd_strlen = strlen(out_cmd);

    // Initialize signal
    signal(SIGINT, term_sig_handler);
    signal(SIGUSR1, term_sig_handler);
    signal(SIGKILL, term_sig_handler);
    signal(SIGABRT, term_sig_handler);
    signal(SIGTERM, term_sig_handler);
    signal(SIGHUP, term_sig_handler);
    //signal(SIGQUIT, SIG_IGN);

    prompt_print();
}

/* printf current char*/
static void
term_echo(void)
{

    if (cmd_strp[cmd_cursor])
    {
        putchar(cmd_strp[cmd_cursor]);
    }
    else
    {
        putchar(' ');
    }

    fflush(stdout);
    term_cursor++;
    cmd_cursor++;
}

/* cursor move back one character */
static void
cursor_backward(void)
{
    if (cmd_cursor > 0 && term_cursor > 0)
    {
        cmd_cursor--;
        term_cursor--;
        putchar('\b');
        fflush(stdout);
    }
}

/* cursor move forward one character */
static void
cursor_forward(void)
{
    if (cmd_cursor < cmd_strlen)
        term_echo();
}

/* move backward one characters. */
static void
handle_backward(void)
{
    if (cmd_cursor > 0)
    {
        cursor_backward();
        handle_delete();
    }
}

/*delete one character in front of cursor */
static void
handle_delete(void)
{
    int cur_tmp = cmd_cursor;

    /*when cursour at the end of string */
    if (cmd_cursor >= cmd_strlen)
        return;

    /*delete one character from string */
    strlcpy(cmd_strp + cur_tmp, cmd_strp + cur_tmp + 1, CMDSTR_BUF_SIZE - cur_tmp);
    cmd_strlen--;

    /*clear characters after cursor  */
    printf("\033[J");

    /*re-print from delete position */
    while (cmd_cursor <= cmd_strlen)
        term_echo();

    /*move cursor back to delete position */
    cur_tmp = cmd_cursor - cur_tmp;
    while (cur_tmp--)
        cursor_backward();
}

/* deal with up arrow*/
static void
handle_up(void)
{
    /*get previous history */
    if (history_prev() < 0)
        return;

    /*copy current history cmd to out_cmd */
    strlcpy(cmd_strp, history_buf[history_cur], CMDSTR_BUF_SIZE);

    /*print out_cmd */
    out_cmd_print();
}

/* deal with down arrow*/
static void
handle_down(void)
{
    /*get previous history */
    if (history_next() < 0)
        return;

    /*copy current history cmd to out_cmd */
    strlcpy(cmd_strp, history_buf[history_cur], CMDSTR_BUF_SIZE);

    /*print out_cmd */
    out_cmd_print();
}

/* deal with left arrow*/
static void
handle_left(void)
{
    cursor_backward();
}

/* deal with right arrow*/
static void
handle_right(void)
{
    cursor_forward();
}

static void
print_cmd_all (void)
{
    int cmd_no = 0;
    for (cmd_no = 0; GCMD_DESC_VALID(cmd_no); cmd_no++)
    {
        if (!GCMD_NAME(cmd_no))
            continue;

        printf("%-10s%s\n", GCMD_NAME(cmd_no), GCMD_MEMO(cmd_no));
    }
}


static void
print_sub_all (int cmd_id)
{
    int cmd_sub_no = 0, cmd_sub_nr = 0;
    for (cmd_sub_no = 0; GCMD_SUB_DESC_VALID(cmd_id, cmd_sub_no); cmd_sub_no++)
    {

        if (!GCMD_SUB_NAME(cmd_id, cmd_sub_no))
            continue;
        if(cmd_sub_no == 0 || strcasecmp(GCMD_SUB_NAME(cmd_id, cmd_sub_no),
                                         GCMD_SUB_NAME(cmd_id, cmd_sub_no-1)))
        {
            printf("%-10s\t", GCMD_SUB_NAME(cmd_id, cmd_sub_no));
            if(cmd_sub_nr && !((cmd_sub_nr+1) %5))
            {
                printf("\n");
            }
            cmd_sub_nr++;
        }
    }
    printf("\n");
}

/*
1. partly_cmd_nr = 0 && index = no match: none
2. partly_cmd_nr = 0 && index = matched: full matched
3. partly_cmd_nr = 1 && index = matched: partly matched & to be completed
4. partly_cmd_nr > 1 && index = matched: partly matched & to be list them
*/
#define NONE_MATCHED(pmatch_nr, pmatch_id)       ( (pmatch_nr == 0) && (pmatch_id == GCMD_DESC_NO_MATCH) )
#define FULL_MATCHED(pmatch_nr, pmatch_id)       ( (pmatch_nr == 0) && (pmatch_id != GCMD_DESC_NO_MATCH) )
#define ONE_PART_MATCHED(pmatch_nr, pmatch_id)   ( (pmatch_nr == 1) && (pmatch_id != GCMD_DESC_NO_MATCH) )
#define MULTI_PART_MATCHED(pmatch_nr, pmatch_id) ( (pmatch_nr > 1 ) && (pmatch_id != GCMD_DESC_NO_MATCH) )


static int
search_cmd_type(char *name, int *pmatch_id, int is_print)
{
    int cmd_no = 0, pmatch_nr = 0;

    *pmatch_id = GCMD_DESC_NO_MATCH;

    /*search type in glb_num_types print matched */
    for (cmd_no = 0; GCMD_DESC_VALID(cmd_no); cmd_no++)
    {

        if (!GCMD_NAME(cmd_no))
            continue;

        if (!strcasecmp(name, GCMD_NAME(cmd_no)))
        {
            /*full matched */
            *pmatch_id = cmd_no;
            break;
        }
        else if (!strncasecmp(name, GCMD_NAME(cmd_no), strlen(name)))
        {
            /*partly matched */
            printc(is_print, "%-10s%s\n", GCMD_NAME(cmd_no), GCMD_MEMO(cmd_no));
            pmatch_nr++;
            *pmatch_id = cmd_no;
        }
    }

    return pmatch_nr;
}

static int
search_cmd_sub(int cmd_id, int *pmatch_sub_id, char *sub_name, int is_print)
{

    int cmd_sub_no, pmatch_sub_nr = 0, fmatch_sub_save = GCMD_DESC_NO_MATCH;

    *pmatch_sub_id = GCMD_DESC_NO_MATCH;

    /*search for full matched */
    for (cmd_sub_no = 0; GCMD_SUB_DESC_VALID(cmd_id, cmd_sub_no); cmd_sub_no++)
    {
        if (!GCMD_SUB_NAME(cmd_id, cmd_sub_no))
            continue;

        if (!strcasecmp(sub_name, GCMD_SUB_NAME(cmd_id, cmd_sub_no)))
        {
            /*full matched */
            printc(is_print, "%-10s\t%s\n", GCMD_SUB_ACT(cmd_id, cmd_sub_no), GCMD_SUB_MEMO(cmd_id, cmd_sub_no));
            //*pmatch_sub_id = cmd_sub_no;
            if(fmatch_sub_save == GCMD_DESC_NO_MATCH)
            {
                *pmatch_sub_id = fmatch_sub_save = cmd_sub_no;
            }

        }
        else if (!strncasecmp(sub_name, GCMD_SUB_NAME(cmd_id, cmd_sub_no), strlen(sub_name)))
        {
            if(fmatch_sub_save != GCMD_DESC_NO_MATCH)
                continue;

            /*partly matched */
            if (*pmatch_sub_id == GCMD_DESC_NO_MATCH || (GCMD_SUB_NAME(cmd_id, cmd_sub_no-1) &&
                    strcasecmp(GCMD_SUB_NAME(cmd_id, cmd_sub_no), GCMD_SUB_NAME(cmd_id, cmd_sub_no-1))))
            {
                printc(is_print, "%-10s\t", GCMD_SUB_NAME(cmd_id, cmd_sub_no));
                pmatch_sub_nr++;
                *pmatch_sub_id = cmd_sub_no;
            }
        }
    }

    if (pmatch_sub_nr > 1)
        printc(is_print, "\n");

    return pmatch_sub_nr;
}

static int
search_cmd_action(int cmd_id, int *pmatch_act_id, char *sub_name, char *action, int is_print)
{
    int cmd_act_no = 0, pmatch_act_nr = 0;

    *pmatch_act_id = GCMD_DESC_NO_MATCH;

    /*search for full matched */
    for (cmd_act_no = 0; GCMD_SUB_DESC_VALID(cmd_id, cmd_act_no); cmd_act_no++)
    {

        if (strcasecmp(sub_name, GCMD_SUB_NAME(cmd_id, cmd_act_no)))
            continue;

        if (!GCMD_SUB_ACT(cmd_id, cmd_act_no))
            continue;

        if (!strcasecmp(action, GCMD_SUB_ACT(cmd_id, cmd_act_no)))
        {
            /*full matched */
            if (*pmatch_act_id == GCMD_DESC_NO_MATCH)
            {
                printc(is_print, "%-10s\n", GCMD_SUB_USAGE(cmd_id, cmd_act_no));
            }

            *pmatch_act_id = cmd_act_no;
            break;
        }
        else if (!strncasecmp(action, GCMD_SUB_ACT(cmd_id, cmd_act_no), strlen(action)))
        {
            /*partly matched */
            if (*pmatch_act_id == GCMD_DESC_NO_MATCH ||( GCMD_SUB_ACT(cmd_id, cmd_act_no-1) &&
                    strcasecmp(GCMD_SUB_ACT(cmd_id, cmd_act_no), GCMD_SUB_ACT(cmd_id, cmd_act_no-1))))
            {
                printc(is_print, "%-10s\t%s\n", GCMD_SUB_ACT(cmd_id, cmd_act_no), GCMD_SUB_MEMO(cmd_id, cmd_act_no));
                pmatch_act_nr++;
                *pmatch_act_id = cmd_act_no;
            }
        }
    }

    return pmatch_act_nr;
}

/*print help info*/
static void
handle_help(void)
{
    int pmatch_id = GCMD_DESC_NO_MATCH, pmatch_sub_id = GCMD_DESC_NO_MATCH, pmatch_act_id = GCMD_DESC_NO_MATCH;
    int cmd_nr = 0, pmatch_nr = 0, pmatch_sub_nr = 0;
    char *tmp_str[3], *cmd_strp_cp = strdup(cmd_strp), *str_save;

    if (!cmd_strp_cp)
        return;

    cmd_strp_cp[strlen(cmd_strp) - 1] = '\0';

    /* split command string into temp array */
    tmp_str[cmd_nr] = (void *) strtok_r(cmd_strp_cp, " ", &str_save);

    while (tmp_str[cmd_nr])
    {
        if (++cmd_nr == 3)
            break;
        tmp_str[cmd_nr] = (void *) strtok_r(NULL, " ", &str_save);
    }

    /*echo input ? */
    printf("?\n");

    int is_print = 0;

    /* print matched command */
    switch (cmd_nr)
    {
        case 3:
            pmatch_nr = search_cmd_type(tmp_str[0], &pmatch_id, is_print);
            if(FULL_MATCHED(pmatch_nr, pmatch_id))
                pmatch_sub_nr = search_cmd_sub(pmatch_id, &pmatch_sub_id, tmp_str[1], is_print);

            if(FULL_MATCHED(pmatch_sub_nr, pmatch_sub_id))
            {
                is_print = 1;
                search_cmd_action(pmatch_id, &pmatch_act_id, tmp_str[1], tmp_str[2], is_print);
            }
            break;

        case 2:
            pmatch_nr = search_cmd_type(tmp_str[0], &pmatch_id, is_print);
            if(FULL_MATCHED(pmatch_nr, pmatch_id))
            {
                is_print = 1;
                search_cmd_sub(pmatch_id, &pmatch_sub_id, tmp_str[1], is_print);
            }
            break;

        case 1:
            is_print = 1;
            pmatch_nr = search_cmd_type(tmp_str[0], &pmatch_id, is_print);

            if(NONE_MATCHED(pmatch_nr, pmatch_id))
            {
                print_cmd_all();
            }
            else if(FULL_MATCHED(pmatch_nr, pmatch_id))
            {
                print_sub_all(pmatch_id);
            }
            break;

        case 0:
            print_cmd_all();
            break;

        default:
            break;
    }

    printf("\n");

    /* re-print prompt */
    prompt_print();

    /* re-print from cursor */
    while (cmd_cursor < cmd_strlen)
        term_echo();

    if(cmd_strp_cp)
        free(cmd_strp_cp);
}

static void
_cmd_complete(char *matchBuf, char *fullName)
{
    //printf ("***%s-%s****", matchBuf, fullName);
    int offset = cmd_strlen - cmd_cursor;
    int diff = strlen(fullName) - strlen(matchBuf);;

    /*print prompt */
    if (cmd_promptp)
        printf("\n%s", cmd_promptp);

    /*give position to new char */
    memmove(cmd_strp + cmd_cursor + diff, cmd_strp + cmd_cursor, offset);
    /*insert new char */
	if (cmd_promptp)
		memcpy(cmd_strp + cmd_cursor, fullName + strlen(matchBuf), diff);
	else
		memcpy(cmd_strp + cmd_cursor, fullName, diff);
    /*caculate new cursor */
    cmd_cursor += diff;

    /*set new cursor and len */
    cmd_strlen = strlen(cmd_strp);
	if (cmd_promptp)
		term_cursor = cmd_strlen + strlen(cmd_promptp);
	else
		term_cursor = cmd_strlen;

    /*re-print command */
    printf("%s", cmd_strp);

    /*set terminal cursor */
    if (cmd_strlen - cmd_cursor)
        printf("\033[%dD", cmd_strlen - cmd_cursor);    /*move cursor left */
}

void _cursor_recover(void)
{
    if(strlen(cmd_strp) != cmd_cursor)
    {
        int clear = strlen(cmd_strp) - cmd_cursor;
        while(clear--)
        {
            printf("\b");
        }
    }
}

/* deal with tab completion*/
#define MATCH_BUF_MAX 100
static void
handle_tab(void)
{

    int cmd_nr = 0;
    char matchBuf[MATCH_BUF_MAX];
    char *tmp_str[3], *str_save;

    memset(matchBuf, 0, MATCH_BUF_MAX);

	if(cmd_cursor < MATCH_BUF_MAX) 
		strlcpy(matchBuf, cmd_strp, cmd_cursor+1);
	 else 
		strlcpy(matchBuf, cmd_strp, MATCH_BUF_MAX);

    printf("\n");

    /* split command string into temp array */
    tmp_str[cmd_nr] = (void *) strtok_r(matchBuf, " ", &str_save);

    if(!tmp_str[cmd_nr])
    {
        print_cmd_all();
        if (cmd_promptp)
            printf("\n%s%s", cmd_promptp, cmd_strp);
	fflush(stdout);
        _cursor_recover();
        return;
    }

    while (tmp_str[cmd_nr])
    {
        if (++cmd_nr == 3)
            break;
        tmp_str[cmd_nr] = (void *) strtok_r(NULL, " ", &str_save);
    }

    int is_print = 1, is_completed = 0;
    int pmatch_nr = 0, pmatch_id = GCMD_DESC_NO_MATCH;

    pmatch_nr = search_cmd_type(tmp_str[0], &pmatch_id, is_print);

    if (cmd_nr == 1)
    {
        if (ONE_PART_MATCHED(pmatch_nr, pmatch_id))
        {
            _cmd_complete(tmp_str[0], GCMD_NAME(pmatch_id));
            is_completed = 1;
        }

        if(NONE_MATCHED(pmatch_nr, pmatch_id))
        {
            print_cmd_all();

        }
        else if(FULL_MATCHED(pmatch_nr, pmatch_id))
        {
            print_sub_all(pmatch_id);
        }

    }
    else
    {
        if (FULL_MATCHED(pmatch_nr, pmatch_id))
        {

            int pmatch_sub_nr = 0, pmatch_sub_id = GCMD_DESC_NO_MATCH;

            if(cmd_nr == 3) is_print = 0;
            pmatch_sub_nr = search_cmd_sub(pmatch_id, &pmatch_sub_id, tmp_str[1], is_print);

            if(cmd_nr == 2)
            {
                if (ONE_PART_MATCHED(pmatch_sub_nr, pmatch_sub_id))
                {
                    _cmd_complete(tmp_str[1], GCMD_SUB_NAME(pmatch_id, pmatch_sub_id));
                    is_completed = 1;
                }

            }
            else
            {
                int pmatch_act_nr = 0, pmatch_act_id = GCMD_DESC_NO_MATCH;
                pmatch_act_nr = search_cmd_action(pmatch_id, &pmatch_act_id, tmp_str[1], tmp_str[2], is_print);

                if (ONE_PART_MATCHED(pmatch_act_nr, pmatch_act_id))
                {
                    _cmd_complete(tmp_str[2], GCMD_SUB_ACT(pmatch_id, pmatch_act_id));
                    is_completed = 1;

                }
                else if (FULL_MATCHED(pmatch_act_nr, pmatch_act_id))
                {
                    is_print = 1;
                    printc(is_print, "%-10s\n", GCMD_SUB_USAGE(pmatch_id, pmatch_act_id));
                }
            }
        }
    }

    if (is_completed == 0)
    {
        /*re-echo */
        if (cmd_promptp)
            printf("\n%s%s", cmd_promptp, cmd_strp);
	fflush(stdout);
        _cursor_recover();
    }
    //_cursor_recover();
}

/*deal with normal character*/
static void
handle_normal(char *out_cmd, char c)
{
    int tmp_cursor = cmd_cursor;

    /*buffer full */
    if (++cmd_strlen > (MATCH_BUF_MAX - 2))
        return;

    /*append operation */
    if (cmd_cursor == (cmd_strlen - 1))
    {
        *(out_cmd + cmd_cursor) = c;
        *(out_cmd + cmd_cursor + 1) = 0;
        term_echo();
    }
    else
    {
        /* Insert operation */
        /*give position to new char */
        memmove(out_cmd + tmp_cursor + 1, out_cmd + tmp_cursor,
                cmd_strlen - tmp_cursor);
        *(out_cmd + tmp_cursor) = c;

        /* re-print from cursor */
        while (cmd_cursor < cmd_strlen)
            term_echo();

        /* restore curor to insert position */
        tmp_cursor = cmd_cursor - tmp_cursor - 1;
        while (tmp_cursor--)
            cursor_backward();
    }
}

/*print prompt info*/
static void
prompt_print(void)
{
    if (cmd_promptp)
    {
        printf("%s", cmd_promptp);
    }

    if (cmd_promptp)
    {
        term_cursor = strlen(cmd_promptp);
    }
    cmd_cursor = 0;
    fflush(stdout);
}

/*print current output  command*/
static void
out_cmd_print(void)
{
    cmd_strlen = strlen(history_buf[history_cur]);

    putchar('\r');
    prompt_print();

    while (cmd_cursor < cmd_strlen)
        term_echo();

    /*clear characters after cursor  */
    printf("\033[J");

}

/* get previous history command*/
static int
history_prev(void)
{
    if (history_cur > 0)
    {
        /*record current*/
        if (cmd_strp[0] != 0 || history_buf[history_cur] == 0)
        {
            if(history_buf[history_cur])
                free(history_buf[history_cur]);
            history_buf[history_cur] = strdup(cmd_strp);
        }
        history_cur--;
        return 0;
    }

    return -1;
}

/* get next history command*/
static int
history_next(void)
{
    if ((history_cur >= 0) && (history_cur < history_nr))
    {
        history_cur++;
        return 0;
    }

    return -1;
}

/*record history command*/
static void
history_record(char *out_cmd)
{
    int i;

    /* cleanup may be saved current command line */
    if (cmd_strlen > 0)         /* no put empty line */
    {
        i = history_nr;

        free(history_buf[HISTORY_MAX_SIZE]);
        history_buf[HISTORY_MAX_SIZE] = 0;
        /* After max history, remove the oldest command */
        if (i >= HISTORY_MAX_SIZE)
        {
            free(history_buf[0]);
            for (i = 0; i < (HISTORY_MAX_SIZE - 1); i++)
                history_buf[i] = history_buf[i + 1];
        }
        history_buf[i++] = strdup(out_cmd);
        history_cur = i;
        history_nr = i;
    }
}

int
_isspace(int ch)
{
    return (unsigned int) (ch - 9) < 5u || ch == ' ';
}

/*call by main*/
int
next_cmd(char *out_cmd)
{
    unsigned char c = 0;
    int key_no = 0;
    int seq_char = 0;
    int str_valid = 0;

    /*set terminal new attrib */
    term_config();

    /*termial initial */
    term_init(out_cmd);

    /*main loop */
    while ((c = getc(stdin)) != '\n')
    {
        key_no = 0;
        seq_char = 0;

        if (!_isspace(c))
        {
            str_valid = 1;
        }

        if (c == 27)            /*escape sequence */
        {
            if ((c = getc(stdin)) == '[' || c == 'O')
            {
                c = getc(stdin);
                seq_char = 1;
            }
        }

        /*search for bind key handle function */
        while (key_no < sizeof (key_bind) / sizeof (key_bind[0]))
        {
            if ((seq_char == key_bind[key_no].is_eseq)
                    && (c == key_bind[key_no].key_last))
            {
                key_bind[key_no].func();
                break;
            }
            key_no++;
        }

        if (key_no == sizeof (key_bind) / sizeof (key_bind[0]))
            handle_normal(out_cmd, c);

    }

    /*handle enter when at the end of a line */
    if (term_cursor)
        putchar('\n');

    /* record command history without '\n' */
    history_record(out_cmd);
#if 0
    /* add '\n' to  out_cmd */
    if (str_valid)
    {
        out_cmd[cmd_strlen++] = '\n';
    }
    else
    {
        cmd_strlen = 0;
        out_cmd[cmd_strlen++] = '\n';
    }

    if (cmd_strlen > 1 && out_cmd[cmd_strlen - 1] == '\n')
        out_cmd[cmd_strlen - 1] = 0;
#else
    if (!str_valid)
        cmd_strlen = 0;
#endif
    /*retore terminal to orginal status */
    term_restore();

    fflush(stdout);

    return cmd_strlen;
}
