#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "kc.h"

int
find_palettes(void)
{
	if ( access("/etc/kc/palettes", F_OK) == 0 )
    {
		strcpy(p.SEQ, "/etc/kc/palettes");
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
	char *envvar, *envval;
	p.fp = fopen(p.SEL, "r");
	while(fgets(p.line, p.len, p.fp) != NULL)
	{
		envvar = strtok(p.line, "=");
		envval = strtok(NULL, "=");
		setenv(envvar, envval, 1);
	}
	fclose(p.fp);
	free(p.line);
	sprintf(p.PRI, "printf %%b \"\
\\e]4;0;#$(echo $color00)\\e\\ \
\\e]4;1;#$(echo $color01)\\e\\ \
\\e]4;2;#$(echo $color02)\\e\\ \
\\e]4;3;#$(echo $color03)\\e\\ \
\\e]4;4;#$(echo $color04)\\e\\ \
\\e]4;5;#$(echo $color05)\\e\\ \
\\e]4;6;#$(echo $color06)\\e\\ \
\\e]4;7;#$(echo $color07)\\e\\ \
\\e]4;8;#$(echo $color08)\\e\\ \
\\e]4;9;#$(echo $color09)\\e\\ \
\\e]4;10;#$(echo $color10)\\e\\ \
\\e]4;11;#$(echo $color11)\\e\\ \
\\e]4;12;#$(echo $color12)\\e\\ \
\\e]4;12;#$(echo $color13)\\e\\ \
\\e]4;12;#$(echo $color14)\\e\\ \
\\e]4;12;#$(echo $color15)\\e\\ \
\\e]10;#$(echo $foreground)\\e\\ \
\\e]11;#$(echo $background)\\e\\ \
\\e]12;#$(echo $cursor)\\e\\ \"", p.CONF);
	p.fp = fopen(p.CSEQ, "w");
	fprintf(p.fp, "%s", p.PRI);
	fclose(p.fp);
	p.fp = fopen(p.CCUR, "w");
	fprintf(p.fp, "%s", p.SEL);
	fclose(p.fp);
	sprintf(p.CLI, "%s > /dev/fd/0", p.PRI);
	system(p.CLI);
	sprintf(p.CLI, "%s > /dev/pts/0", p.PRI);
	system(p.CLI);
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
	p.dr = opendir(p.SEL);

	if (p.dr == NULL)
	{
		printf("Could not open directory");
		return 1;
	}

	while((de = readdir(p.dr)) != NULL)
	{
		printf("->%s\n", de->d_name);
	}

	closedir(p.dr);
    return 0;
}

int
print_palette(void)
{
	p.len = 255;
	p.line = malloc(sizeof(char) * p.len);
    p.fp = fopen(p.CCUR, "r");
	fgets(p.line,p.len, p.fp);
	printf("\nUsing: %s\n", p.line);
	fclose(p.fp);
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
	strcat(p.CONF, "/kc");
	if ( mkdir(p.CONF,0777) == 0 )
	{
		printf("Created 'kc' directory in XDG_CONFIG_HOME.");
	}
	sprintf(p.CCUR, "%s/current", p.CONF);
	sprintf(p.CSEQ, "%s/sequence", p.CONF);

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
usage: kc [-s palette|-r|-L] [l|-v|-p]\n \
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
