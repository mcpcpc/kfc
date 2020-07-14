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
		strcpy(p.seq, "/usr/share/kfc/palettes");
        return 0;
    }

    if ( access("palettes", F_OK) == 0 )
    {
		strcpy(p.seq, "palettes");
        return 0;
    }

    return 1;
}

int
select_palette(void)
{
	p.len = 255;
	p.line = malloc(sizeof(char) * p.len);
	p.fp = fopen(p.sel, "r");

	if (p.fp == NULL)
	{
		fprintf(stderr, "Selected palette does not exist.\n");
		return 1;
	}

	while(fgets(p.line, p.len, p.fp) != NULL)
	{
		p.evar = strtok(p.line, "=");
		p.eval = strtok(NULL, "=");
		setenv(p.evar, p.eval, 1);
	}

	fclose(p.fp);
	free(p.line);
	sprintf(p.pri, "printf %%b \"\
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
	p.fp = fopen(p.cseq, "w");
	fprintf(p.fp, "%s", p.pri);
	fclose(p.fp);
	p.fp = fopen(p.ccur, "w");
	fprintf(p.fp, "%s", p.sel);
	fclose(p.fp);
	sprintf(p.cli, "%s", p.pri);
	system(p.cli);
	return 0;
}

int
list_palette(void)
{
	p.dr = opendir(p.sel);

	if ( p.dr == NULL )
	{
		fprintf(stderr, "Could not open directory\n");
		return 1;
	}

	while( (de = readdir(p.dr)) != NULL )
	{
		if ( de->d_type == 8 )
		{
			puts(de->d_name);
		}
	}

	closedir(p.dr);
    return 0;
}

int
random_palette(void)
{
	p.dr = opendir(p.sel);
	p.randf = 0;

	if ( p.dr == NULL )
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
	p.dr = opendir(p.sel);
	while( (de = readdir(p.dr)) != NULL )
	{
		if (p.i == p.randf)
		{
			strcat(p.sel, "/");
			strcat(p.sel, de->d_name);
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
    p.fp = fopen(p.ccur, "r");

	if (p.fp == NULL)
	{
		fprintf(stderr, "No palette set or 'current' file missing\n");
		return 1;
	}
	fgets(p.line,p.len, p.fp);
	puts(p.line);
	fclose(p.fp);
	free(p.line);

    for ( p.i  = 0; p.i < 15; p.i++ )
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
print_usage(void)
{
    printf("\
usage: kfc [-l|-L|-r|-s palette] [-v|-p]\n \
-l          List all palettes\n \
-L          Set light themes (modifier for -s/-r)\n \
-r          Select a random palette\n \
-s palette  Select a palette\n \
-p          Print current palette\n \
-h          Show this information\n \
-v          Show version information\n");
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
	int eflag, sflag, rflag, lflag;
	eflag = sflag = rflag = lflag = 0;
	p.mode = "dark";

	if ( (p.conf = getenv("XDG_CONFIG_HOME")) == NULL )
    {
        fprintf(stderr, "XDG_CONFIG_HOME not defined\n");
        exit(2);
    }

	strcat(p.conf, "/kfc");
	
	if ( mkdir(p.conf, 0777) == 0 )
	{
		puts("Created 'kfc' directory in XDG_CONFIG_HOME.");
	}

	snprintf(p.ccur, sizeof(p.ccur), "%s/current", p.conf);
	snprintf(p.cseq, sizeof(p.cseq), "%s/sequence", p.conf);
	
    if (find_palettes() == 1)
    {
        fprintf(stderr, "Palette directory not found\n");
        exit(2);
    }

    while ( ( p.cval = getopt(argc, argv, "rlLpvhs:" ) ) != -1 )
    {
        switch (p.cval)
        {
            case 'v':
                puts("0.0.9");
                break;
            case 'h':
				print_usage();
                break;
            case 'L':
                p.mode = "light";
                break;
            case 'l':
				lflag++;
                break;
            case 'r':
				sflag++;
				rflag++;
				break;
            case 's':
				if (rflag)
				{
					fprintf(stderr, "Cannot specify -r with -s\n");
					eflag++;
				}
				sflag++;
				p.sval = optarg;
                break;
            case 'p':
				print_palette();
				break;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                eflag++;
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                eflag++;
        }
	}

    if (eflag)
    {
		print_usage();
		exit(2);
	}

	snprintf(p.sel, sizeof(p.sel), "%s/%s", p.seq, p.mode);
	
	if (lflag)
	{
		list_palette();
	}

	if (sflag)
	{
		if (rflag)
		{
            random_palette();
		}
		else
		{
			snprintf(p.sel, sizeof(p.sel), "%s/%s/%s", p.seq, p.mode, p.sval);
		}
		select_palette();
	}

    return 0;
}
