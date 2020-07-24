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

struct dirent *de;

static int i, cval, randf, rflag, lflag, pflag = 0;
static size_t len = BUFF;
static char *line, *sval, *mode, *conf, *evar, *eval, *seq = NULL;
static char sel[BUFF];
static FILE *fp;
static DIR *dr;

static void
find_palettes(void)
{
	if ( access("/usr/share/kfc/palettes", F_OK) == 0 )
    {
		setenv("KFC_SEQ", "/usr/share/kfc/palettes", 1);
    }
    else if ( access("palettes", F_OK) == 0 )
    {
        setenv("KFC_SEQ", "palettes", 1);
    }
    else
    {
        fprintf(stderr, "Palette source directory not found\n");
        exit(1);
    }
}

static void
select_palette(void)
{
	line = malloc(sizeof(char) * len);
	fp = fopen(sel, "r");

	if (fp == NULL)
	{
		fprintf(stderr, "Selected palette does not exist\n");
		exit(1);
	}

	while(fgets(line, len, fp) != NULL)
	{
		evar = strtok(line, "=");
		eval = strtok(NULL, "=");
		setenv(evar, eval, 1);
	}

	fclose(fp);
	free(line);
	fp = fopen(conf, "w");
	fprintf(fp, "%s", sel);
	fclose(fp);
	system(SEQ);
}

static void
list_palette(void)
{
	dr = opendir(sel);

	if ( dr == NULL )
	{
		fprintf(stderr, "Could not open directory\n");
		exit(1);
	}

	while( (de = readdir(dr)) != NULL )
	{
		if ( de->d_type == 8 )
		{
			puts(de->d_name);
		}
	}

	closedir(dr);
}

static void
random_palette(void)
{
	dr = opendir(sel);
	randf = 0;

	if ( dr == NULL )
	{
		fprintf(stderr, "Could not open directory\n");
		exit(1);
	}

	while( (de = readdir(dr)) != NULL )
	{
		randf++;
	}

	closedir(dr);
	srand(time(0));
	i = (rand() % (randf - 0 + 1)) + 0;
	randf = 0;
	dr = opendir(sel);
	while( (de = readdir(dr)) != NULL )
	{
		if (i == randf)
		{
			strcat(sel, "/");
			strcat(sel, de->d_name);
		}
		randf++;
	}
}

static void
print_palette(void)
{
	line = malloc(sizeof(char) * len);
    fp = fopen(conf, "r");

	if (fp == NULL)
	{
		fprintf(stderr, "No palette set or 'current' file missing\n");
		exit(1);
	}

	fgets(line,len, fp);
	puts(line);
	fclose(fp);
	free(line);

    printf(PALETTE);
}

static void
usage(void)
{
    fprintf(stderr, "\
usage: kfc [-L] [-r|-s palette] [-l|-p|-v]\n \
-L          Set light themes (modifier for -s/-r)\n \
-r          Select a random palette (dark theme by default)\n \
-s palette  Select a palette (dark theme by default)\n \
-l          List all palettes (dark themes by default)\n \
-p          Print current palette\n \
-v          Show version information\n");
	exit(1);
}

int
main(int argc, char **argv)
{
    if (argc == 1)
	{
        fprintf(stderr, "No argument(s) provided\n");
        exit(1);
	}
	
	extern char *optarg;
	extern int optind, optopt;

	mode = "dark";

	if ( (conf = getenv("XDG_CONFIG_HOME")) == NULL )
    {
        fprintf(stderr, "XDG_CONFIG_HOME not defined\n");
        exit(1);
    }

	strcat(conf, "/kfc");
	if ( mkdir(conf, 0777) == 0 )
	{
		puts("Created 'kfc' directory in XDG_CONFIG_HOME");
	}

	strcat(conf, "/current");
    find_palettes();

    while ( ( cval = getopt(argc, argv, "rlLpvs:" ) ) != -1 )
    {
        switch (cval)
        {
            case 'v':
                puts("0.1.1");
                break;
            case 'L':
                mode = "light";
                break;
            case 'l':
				lflag++;
                break;
            case 'r':
				rflag++;
				break;
            case 's':
				if (rflag)
				{
					fprintf(stderr, "Cannot specify -r with -s\n");
					usage();
				}
				sval = optarg;
                break;
            case 'p':
				pflag++;
				break;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                usage();
				break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                usage();
				break;
        }
	}

	seq = getenv("KFC_SEQ");
	snprintf(sel, sizeof(sel), "%s/%s", seq, mode);

	if (lflag)
	{
		list_palette();
	}

	if (rflag)
	{
        random_palette();
		select_palette();
	}
	else if (sval)
	{
		snprintf(sel, sizeof(sel), "%s/%s/%s", seq, mode, sval);
		select_palette();
	}

	if (pflag)
	{
	    print_palette();
	}

    return 0;
}
