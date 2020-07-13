#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "kfc.h"

int
find_palettes(void)
{
	if ( access("/usr/share/kfc/palettes", F_OK) == 0 )
    {
		strcpy(p.SEQ, "/usr/share/kfc/palettes");
        return 0;
    }

    if ( access("palettes", F_OK) == 0 )
    {
		strcpy(p.SEQ, "palettes");
        return 0;
    }

    return 1;
}

int
select_palette(void)
{
	p.len = 255;
	p.line = malloc(sizeof(char) * p.len);
	p.fp = fopen(p.SEL, "r");

	if (p.fp == NULL)
	{
		fprintf(stderr, "Selected palette does not exist.\n");
		return 1;
	}

	while(fgets(p.line, p.len, p.fp) != NULL)
	{
		p.EVAR = strtok(p.line, "=");
		p.EVAL = strtok(NULL, "=");
		setenv(p.EVAR, p.EVAL, 1);
	}

	fclose(p.fp);
	free(p.line);
	sprintf(p.PRI, "printf %%b \"\
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
\\033]4;12;#$(echo $color13)\\033\\ \
\\033]4;12;#$(echo $color14)\\033\\ \
\\033]4;12;#$(echo $color15)\\033\\ \
\\033]10;#$(echo $foreground)\\033\\ \
\\033]11;#$(echo $background)\\033\\ \
\\033]12;#$(echo $cursor)\\033\\ \" \033[21D");
	p.fp = fopen(p.CSEQ, "w");
	fprintf(p.fp, "%s", p.PRI);
	fclose(p.fp);
	p.fp = fopen(p.CCUR, "w");
	fprintf(p.fp, "%s", p.SEL);
	fclose(p.fp);
	sprintf(p.CLI, "%s", p.PRI);
	system(p.CLI);
	return 0;
}

int
list_palette(void)
{
	p.dr = opendir(p.SEL);

	if (p.dr == NULL)
	{
		fprintf(stderr, "Could not open directory\n");
		return 1;
	}

	while( (de = readdir(p.dr)) != NULL )
	{
		puts(de->d_name);
	}

	closedir(p.dr);
    return 0;
}

int
random_palette(void)
{
	p.dr = opendir(p.SEL);
	p.randf = 0;

	if (p.dr == NULL)
	{
		fprintf(stderr, "Could not open directory\n");
		return 1;
	}

	while( (de = readdir(p.dr)) != NULL )
	{
		p.randf++;
	}

	closedir(p.dr);
	srand(time(0));
	p.i = (rand() % (p.randf - 2 + 1)) + 2;
	p.randf = 0;
	p.dr = opendir(p.SEL);
	while(( de = readdir(p.dr)) != NULL )
	{
		if (p.i == p.randf)
		{
			strcat(p.SEL, "/");
			strcat(p.SEL, de->d_name);
		}
		p.randf++;
	}
	return 0;
}

int
print_palette(void)
{
	p.len = 255;
	p.line = malloc(sizeof(char) * p.len);
    p.fp = fopen(p.CCUR, "r");

	if (p.fp == NULL)
	{
		fprintf(stderr, "No palette set or 'current' file missing\n");
		return 1;
	}
	fgets(p.line,p.len, p.fp);
	puts(p.line);
	fclose(p.fp);
	free(p.line);

    for (p.i  = 0; p.i < 15; p.i++)
	{
		printf("\033[48;5;%dm  \033[0m", p.i);

		if (p.i == 7)
		{
			printf("\n");
		}
	}

	printf("\n");
	return 0;
}

int
main(int argc, char **argv)
{
    if (argc == 1)
	{
        fprintf(stderr, "No argumemts provided\n");
        exit(2);
	}
	
	extern char *optarg;
	extern int optind, optopt;
	p.MODE = "dark";

	if ( (p.CONF = getenv("XDG_CONFIG_HOME")) == NULL )
    {
        fprintf(stderr, "XDG_CONFIG_HOME not defined\n");
        exit(2);
    }

	strcat(p.CONF, "/kfc");
	
	if ( mkdir(p.CONF,0777) == 0 )
	{
		puts("Created 'kfc' directory in XDG_CONFIG_HOME.");
	}

	snprintf(p.CCUR, sizeof(p.CCUR), "%s/current", p.CONF);
	snprintf(p.CSEQ, sizeof(p.CSEQ), "%s/sequence", p.CONF);
	
    if (find_palettes() == 1)
    {
        fprintf(stderr, "Palette directory not found\n");
        exit(2);
    }

    while ( ( p.cval = getopt(argc, argv, "rlLpvhs:" ) ) != -1 )
    {
        switch (p.cval)
        {
            case 'r':
				snprintf(p.SEL, sizeof(p.SEL), "%s/%s", p.SEQ, p.MODE);
                random_palette();
				select_palette();
				break;
            case 'l':
				snprintf(p.SEL, sizeof(p.SEL), "%s/%s", p.SEQ, p.MODE);
				list_palette();
                break;
            case 'L':
                p.MODE = "light";
                break;
            case 'v':
                printf("0.0.8\n");
                break;
            case 'h':
                p.FLAG++;
                break;
            case 's':
				snprintf(p.SEL, sizeof(p.SEL), "%s/%s/%s", p.SEQ, p.MODE, optarg);
				select_palette();
                break;
            case 'p':
				print_palette();
				break;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                p.FLAG++;
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                p.FLAG++;
        }
    }
    if (p.FLAG)
    {
        printf("\
usage: kfc [-s palette|-r|-L] [l|-v|-p]\n \
-s palette  Select a palette\n \
-l          List all palettes\n \
-p          Print current palette\n \
-r          Select a random palette\n \
-L          Set light themes (modifier for -s/-r)\n \
-h          Show this information\n \
-v          Show version information\n");
    }
    return 0;
}
