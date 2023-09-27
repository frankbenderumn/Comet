/**
 * @file color.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Definitions and descriptions ommited for brevity. This file manages
 * color-based terminal input utilizing preprocessor directives.
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_COLOR_H_
#define COMET_COLOR_H_

#include <stdio.h>

void red(void);
void blue(void);
void yellow(void);
void green(void);
void cyan(void);
void purple(void);
void magenta(void);
void black(void);
void white(void);
void grey(void);
void lred(void);
void lgreen(void);
void lyellow(void);
void lblue(void);
void lpurple(void);
void lmagenta(void);
void lcyan(void);
void lwhite(void);

void redbg(void);
void bluebg(void);
void yellowbg(void);
void greenbg(void);
void cyanbg(void);
void purplebg(void);
void magentabg(void);
void blackbg(void);
void whitebg(void);
void greybg(void);
void lredbg(void);
void lgreenbg(void);
void lyellowbg(void);
void lbluebg(void);
void lpurplebg(void);
void lmagentabg(void);
void lcyanbg(void);
void lwhitebg(void);

void bold(void);
void underscore(void);
void blink(void);
void reverse(void);
void conceal(void);
void clearcolor(void);

void br(void);

#define RED(...) red(); printf(__VA_ARGS__); clearcolor();
#define BLU(...) blue(); printf(__VA_ARGS__); clearcolor();
#define YEL(...) yellow(); printf(__VA_ARGS__); clearcolor();
#define MAG(...) magenta(); printf(__VA_ARGS__); clearcolor();
#define GRE(...) green(); printf(__VA_ARGS__); clearcolor();
#define CYA(...) cyan(); printf(__VA_ARGS__); clearcolor();
#define WHI(...) white(); printf(__VA_ARGS__); clearcolor();
#define GRY(...) grey(); printf(__VA_ARGS__); clearcolor();
#define BLA(...) black(); printf(__VA_ARGS__); clearcolor();

#define BRED(...) red(); bold(); printf(__VA_ARGS__); clearcolor();
#define BBLU(...) blue(); bold(); printf(__VA_ARGS__); clearcolor();
#define BYEL(...) yellow(); bold(); printf(__VA_ARGS__); clearcolor();
#define BMAG(...) magenta(); bold(); printf(__VA_ARGS__); clearcolor();
#define BGRE(...) green(); bold(); printf(__VA_ARGS__); clearcolor();
#define BCYA(...) cyan(); bold(); printf(__VA_ARGS__); clearcolor();
#define BWHI(...) white(); bold(); printf(__VA_ARGS__); clearcolor();
#define BGRY(...) grey(); bold(); printf(__VA_ARGS__); clearcolor();
#define BBLA(...) black(); bold(); printf(__VA_ARGS__); clearcolor();

typedef unsigned char* bytes_t;
typedef unsigned char BYTE;

#endif