# ~/.bash_logout: executed by bash(1) when login shell exits.

# when leaving the console clear the screen to increase privacy

case "`tty`" in
    /dev/tty[0-9]*) clear
esac
