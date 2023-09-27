#include "comet/util/color.h"

void red(void) { printf("\033[0;31m"); }
void blue(void) { printf("\033[0;34m"); }
void yellow(void) { printf("\033[0;33m"); }
void green(void) { printf("\033[0;32m"); }
void cyan(void) { printf("\033[0;36m"); }
void purple(void) { printf("\033[0;35m"); }
void magenta(void) { printf("\033[0;35m"); }
void black(void) { printf("\033[0;30m"); }
void white(void) { printf("\033[0;37m"); }
void grey(void) { printf("\033[0;90m"); }
void lred(void) { printf("\033[0;91m"); }
void lgreen(void) { printf("\033[0;92m"); }
void lyellow(void) { printf("\033[0;93m"); }
void lblue(void) { printf("\033[0;94m"); }
void lpurple(void) { printf("\033[0;95m"); }
void lmagenta(void) { printf("\033[0;95m"); }
void lcyan(void) { printf("\033[0;96m"); }
void lwhite(void) { printf("\033[0;97m"); }

void redbg(void) { printf("\033[0;41m"); }
void bluebg(void) { printf("\033[0;44m"); }
void yellowbg(void) { printf("\033[0;43m"); }
void greenbg(void) { printf("\033[0;42m"); }
void cyanbg(void) { printf("\033[0;46m"); }
void purplebg(void) { printf("\033[0;45m"); }
void magentabg(void) { printf("\033[0;45m"); }
void blackbg(void) { printf("\033[0;40m"); }
void whitebg(void) { printf("\033[0;47m"); }
void greybg(void) { printf("\033[0;100m"); }
void lredbg(void) { printf("\033[0;101m"); }
void lgreenbg(void) { printf("\033[0;102m"); }
void lyellowbg(void) { printf("\033[0;103m"); }
void lbluebg(void) { printf("\033[0;104m"); }
void lpurplebg(void) { printf("\033[0;105m"); }
void lmagentabg(void) { printf("\033[0;105m"); }
void lcyanbg(void) { printf("\033[0;106m"); }
void lwhitebg(void) { printf("\033[0;107m"); }

void bold(void) { printf("\033[1m"); }
void underscore(void) { printf("\033[4m"); }
void blink(void) { printf("\033[5m"); }
void reverse(void) { printf("\033[7m"); }
void conceal(void) { printf("\033[8m"); }
void clearcolor(void) { printf("\033[0m"); }

void br(void) { printf("\n"); }