# /etc/inputrc - global inputrc for libreadline
# See readline(3readline) and `info readline' for more information.

# Be 8 bit clean.
set input-meta on
set output-meta on
set bell-style visible

# To allow the use of 8bit-characters like the german umlauts, comment out
# the line below. However this makes the meta key not work as a meta key,
# which is annoying to those which don't need to type in 8-bit characters.

# set convert-meta off

"\e0d": backward-word
"\e0c": forward-word
"\e[h": beginning-of-line
"\e[f": end-of-line
"\e[1~": beginning-of-line
"\e[4~": end-of-line
#"\e[5~": beginning-of-history
#"\e[6~": end-of-history
"\e[3~": delete-char
"\e[2~": quoted-insert

# Common standard keypad and cursor
# (codes courtsey Werner Fink, <werner@suse.de>)
#"\e[1~": history-search-backward
"\e[2~": yank
"\e[3~": delete-char
#"\e[4~": set-mark
"\e[5~": history-search-backward
"\e[6~": history-search-forward
# Normal keypad and cursor of xterm
"\e[F": end-of-line
"\e[H": beginning-of-line
# Application keypad and cursor of xterm
"\eOA": previous-history
"\eOC": forward-char
"\eOB": next-history
"\eOD": backward-char
"\eOF": end-of-line
"\eOH": beginning-of-line

