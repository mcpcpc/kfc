#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "kpal.h"

int
find_palettes(void)
{
	sprintf(p.SEQ, "/etc/kpal/palettes/%s", p.MODE);

	if ( access(p.SEQ, F_OK) == 0 )
    {
        return 0;
    }

	sprintf(p.SEQ, "palettes/%s", p.MODE);

    if ( access(p.SEQ, F_OK) == 0 )
    {
        return 0;
    }

	sprintf(p.SEQ, "%s", p.MODE);

    if ( access(p.SEQ, F_OK) == 0 )
    {
        return 0;
    }
    
    return 1;
}

int
select_palette(void)
{
	size_t len = 255;
	char *line = malloc(sizeof(char) * len);
	int i = 0;
	char *envvar;
	strcat(p.SEQ, "/");
	strcat(p.SEQ, p.SELE);
	FILE *fp = fopen(p.SEQ, "r");
	setenv("LC_ALL", "C", 1);
	system("exec 6>&1 >/dev/null");
	while(fgets(line, len, fp) != NULL)
	{
		envvar = strtok(line, "=");

        if (strcmp(line, "color00") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;0;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color01") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;1;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color02") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;2;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color03") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;3;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color04") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;4;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color05") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;5;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color06") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;6;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color07") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;7;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color08") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;8;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color09") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;9;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color10") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;10;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color11") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;11;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color12") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;12;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color13") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;13;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color14") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;14;#%s\\033\\", envvar);
		}
		if (strcmp(line, "color15") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;15;#%s\\033\\", envvar);
		}
		if (strcmp(line, "foreground") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]10;#%s\\033\\", envvar);
		}
		if (strcmp(line, "background") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]11;#%s\\033\\", envvar);
		}
		if (strcmp(line, "cursor") == 0)
        {
		    envvar = strtok(NULL, "=");
		    sprintf(p.CLI, "\\033]4;12;#%s\\033\\", envvar);
		}
		strcat(p.PRI, p.CLI);
	}
	fclose(fp);
	free(line);
	printf("string: %s", p.PRI);
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
	DIR *dr = opendir(p.SEQ);

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
	printf("\nUsing: %s\n", p.SEQ);
    for (int i = 0; i < 15; i++)
	{
		printf("\033[48;5;%dm  \033[0m", i);

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
				p.SELE = optarg;
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
