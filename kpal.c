#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "kpal.h"

int
find_palettes(void)
{
	if ( access("/etc/kpal/palettes", F_OK) == 0 )
    {
		strcpy(p.SEQ, "/etc/kpal/palettes");
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
	size_t len = 255;
	char *line = malloc(sizeof(char) * len);
	char *envvar, *envval;
	FILE *fp = fopen(p.SEL, "r");
	setenv("LC_ALL", "C", 1);
	system("exec 6>&1 >/dev/null");
	while(fgets(line, len, fp) != NULL)
	{
		envvar = strtok(line, "=");
		envval = strtok(NULL, "=");
		setenv(envvar, envval, 1);
	}
	fclose(fp);
	free(line);
	sprintf(p.PRI, "printf %%b \" \\e]4;0;$(echo $color00)\\e\\ \
\\e]4;1;$(echo $color01)\\e\\ \
\\e]4;2;$(echo $color02)\\e\\ \
\\e]4;3;$(echo $color03)\\e\\ \
\\e]4;4;$(echo $color04)\\e\\ \
\\e]4;5;$(echo $color05)\\e\\ \
\\e]4;6;$(echo $color06)\\e\\ \
\\e]4;7;$(echo $color07)\\e\\ \
\\e]4;8;$(echo $color08)\\e\\ \
\\e]4;9;$(echo $color09)\\e\\ \
\\e]4;10;$(echo $color10)\\e\\ \
\\e]4;11;$(echo $color11)\\e\\ \
\\e]4;12;$(echo $color12)\\e\\ \
\\e]4;12;$(echo $color13)\\e\\ \
\\e]4;12;$(echo $color14)\\e\\ \
\\e]4;12;$(echo $color15)\\e\\ \
\\e]10;$(echo $foreground)\\e\\ \
\\e]11;$(echo $background)\\e\\ \
\\e]12;$(echo $cursor)\\e\\ \
\" > %s/sequence", p.CONF);
	printf("%s\n", p.PRI);
	system(p.PRI);
    sprintf(p.PRI, "printf '%s' > %s/current", p.SEL, p.CONF);
	system(p.PRI);
	return 0;
}

int
random_palette(void)
{
    return 0;
}

int
list_palette(void)
{
	struct dirent *de;
	DIR *dr = opendir(p.SEL);

	if (dr == NULL)
	{
		printf("Could not open directory");
		return 1;
	}

	while((de = readdir(dr)) != NULL)
	{
		printf("->%s\n", de->d_name);
	}

	closedir(dr);
    return 0;
}

int
print_palette(void)
{
	FILE *fp = fopen(p.CONF, "r");
	fclose(fp);
	printf("\nUsing: %s\n", p.SEL);
    for (int i = 0; i < 15; i++)
	{
		printf("\e[48;5;%dm  \e[0m", i);

		if (i == 7)
		{
			printf("\n");
		}
	}
	printf("\n\n");
	return 0;
}

int
main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind, optopt;
	p.MODE = "dark";
	if ( (p.CONF = getenv("XDG_CONFIG_HOME")) == NULL )
    {
        fprintf(stderr, "XDG_CONFIG_HOME not defined\n");
        exit(2);
    }
	strcat(p.CONF, "/kpal");
    
    if (find_palettes() == 1)
    {
        fprintf(stderr, "Palette directory not found\n");
        exit(2);
    }

    while ( ( p.cval = getopt(argc, argv, "rlLpvhs:" )) != -1 )
    {
        switch (p.cval)
        {
            case 'r':
                random_palette();
                break;
            case 'l':
				sprintf(p.SEL, "%s/%s", p.SEQ, p.MODE);
				list_palette();
                break;
            case 'L':
                p.MODE = "light";
                break;
            case 'p':
				print_palette();
				break;
            case 'v':
                printf("0.0.1\n");
                break;
            case 'h':
                p.errf++;
                break;
            case 's':
				sprintf(p.SEL, "%s/%s/%s", p.SEQ, p.MODE, optarg);
				select_palette();
				print_palette();
                break;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                p.errf++;
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                p.errf++;
        }
    }
    if (p.errf)
    {
        fprintf(stderr, "\
usage: cpal [-s palette|-r|-L] [l|-v|-p]\n \
-s palette  Select a palette\n \
-l          List all palettes\n \
-p          Print current palette\n \
-r          Select a random palette\n \
-L          Set light themes (modifier for -s/-r)\n \
-h          Show this information\n \
-v          Show version information\n");
            exit(2);
    }
    return 0;
}
