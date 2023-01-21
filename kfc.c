/*
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020-3033 Michael Czigler
 */
 
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define SEQUENCE "printf \"\
\\033]4;0;#$(echo $color00)\\033\\ \
\\033]4;1;#$(echo $color01)\\033\\ \
\\033]4;2;#$(echo $color02)\\033\\ \
\\033]4;3;#$(echo $color03)\\033\\ \
\\033]4;4;#$(echo $color04)\\033\\ \
\\033]4;5;#$(echo $color05)\\033\\ \
\\033]4;6;#$(echo $color06)\\033\\ \
\\033]4;7;#$(echo $color07)\\033\\ \
\\033]4;8;#$(echo $color08)\\033\\ \
\\033]4;9;#$(echo $color09)\\033\\ \
\\033]4;10;#$(echo $color10)\\033\\ \
\\033]4;11;#$(echo $color11)\\033\\ \
\\033]4;12;#$(echo $color12)\\033\\ \
\\033]4;13;#$(echo $color13)\\033\\ \
\\033]4;14;#$(echo $color14)\\033\\ \
\\033]4;15;#$(echo $color15)\\033\\ \
\\033]10;#$(echo $foreground)\\033\\ \
\\033]11;#$(echo $background)\\033\\ \
\\033]12;#$(echo $cursor)\\033\\ \
\\033[21D\""

#define PALETTE "\
\033[48;5;0m  \033[0m\
\033[48;5;1m  \033[0m\
\033[48;5;2m  \033[0m\
\033[48;5;3m  \033[0m\
\033[48;5;4m  \033[0m\
\033[48;5;5m  \033[0m\
\033[48;5;6m  \033[0m\
\033[48;5;7m  \033[0m\n\
\033[48;5;8m  \033[0m\
\033[48;5;9m  \033[0m\
\033[48;5;10m  \033[0m\
\033[48;5;11m  \033[0m\
\033[48;5;12m  \033[0m\
\033[48;5;13m  \033[0m\
\033[48;5;14m  \033[0m\
\033[48;5;15m  \033[0m\n"

static char *seq;           /* path to palette source directory  */
static char *conf;          /* path to configuration file */
static char *mode = "dark"; /* selected mode string */
static char *sval = NULL;   /* selected palette string */

static void
find_palettes(void) {
    if (access("/usr/share/kfc/palettes", F_OK) == 0) {
        seq = "/usr/share/kfc/palettes";
    }
    else if (access("palettes", F_OK) == 0) {
        seq = "palettes";
    }
    else {
        fprintf(stderr, "Palette source directory not found\n");
        exit(1);
    }
}

static void
select_palette(char *sel) {
    char *evar, *eval, *line;
    FILE *fp = fopen(sel, "r");
    size_t len = 255;

    line = malloc(sizeof(char) * len);

    if (fp == NULL) {
        fprintf(stderr, "Selected palette does not exist\n");
        exit(1);
    }

    while (fgets(line, len, fp) != NULL) {
        evar = strtok(line, "=");
        eval = strtok(NULL, "=");
        setenv(evar, eval, 1);
    }

    fclose(fp);
    free(line);
    fp = fopen(conf, "w");
    fprintf(fp, "%s", sel);
    fclose(fp);
    fp = popen(SEQUENCE, "w");
    pclose(fp);
}

static void
list_palette(char *sel) {
    struct dirent **de;
    int n = scandir(sel, &de, NULL, alphasort);

    while (n--) {
        if (de[n]->d_type == 8) printf("%s/%s\n", mode, de[n]->d_name);
    }

    free(de);
}

static void
random_palette(char *sel) {
    struct dirent **de;
    int n = scandir(sel, &de, NULL, alphasort);

    srand(time(0));
    int i = (rand() % (n - 0 + 1)) + 0;

    while (n--) {
        if (i == n) sval = de[n]->d_name;
    }

    free(de);
}

static void
print_palette(void) {
    FILE *fp = fopen(conf, "r");
    size_t len = 255;
    char *line;

    line = malloc(sizeof(char) * len);

    if (fp == NULL) {
        fprintf(stderr, "No palette set or config file missing\n");
        exit(1);
    }

    fgets(line, len, fp);
    puts(line);
    fclose(fp);
    free(line);

    printf(PALETTE);
}

int
main(int argc, char **argv) {
    int cval;
    int rflag = 0, lflag = 0, pflag = 0;
    size_t len;
    char *sel, *tmp;

    if (argc == 1) {
        fprintf(stderr, "No argument(s) provided\n");
        return 1;
    }

    tmp = getenv("XDG_CONFIG_HOME");

    if (tmp != NULL) {
        len = strlen(tmp) + sizeof("/kfc/current");
        conf = malloc(sizeof(char) * len);
        snprintf(conf, len, "%s", tmp);
    }
    else {
        puts("XDG_CONFIG_HOME is not defined. Defaulting to HOME/.config");
        tmp = getenv("HOME");
        len = strlen(tmp) + sizeof("/.config/kfc/current");
        conf = malloc(sizeof(char) * len);
        snprintf(conf, len, "%s/.config", tmp);
    }

    strcat(conf, "/kfc");

    if (mkdir(conf, 0777) == 0) {
        puts("Created 'kfc' directory in XDG_CONFIG_HOME");
    }

    strcat(conf, "/current");
    find_palettes();

    while ((cval = getopt(argc, argv, "rlLpvs:" )) != -1) {
        switch (cval) {
            case 'v': puts("kfc 0.1.2");  break;
            case 'L': mode = "light";     break;
            case 'l': lflag++;            break;
            case 'r': rflag++;            break;
            case 'p': pflag++;            break;
            case 's':
                if (rflag) {
				    fprintf(stderr, "Cannot specify -r with -s\n");
					return 1;
				}
				sval = optarg;
				break;
            case '?': return 1;
        }
    }

    len = strlen(seq)
        + sizeof("/")
        + strlen(mode) 
        + (sval ? strlen(sval) + 1 : 0);

    sel = malloc(sizeof(char) * len);

    if (lflag) {
        snprintf(sel, len, "%s/%s", seq, mode);
        list_palette(sel);
    }

    if (rflag) {
        snprintf(sel, len, "%s/%s", seq, mode);
        random_palette(sel);
        len = strlen(sel)
            + sizeof("/")
            + strlen(sval);
        sel = (char *)realloc(sel, sizeof(char) * len);
    }

    if (sval) {
        snprintf(sel, len, "%s/%s/%s", seq, mode, sval);
        select_palette(sel);
    }

    if (pflag) print_palette();

    free(sel);

    return 0;
}
