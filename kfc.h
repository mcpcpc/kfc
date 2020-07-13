struct
{
	int i;
	int cval;
	int randf;
	FILE *fp;
	DIR *dr;
	char *line;
	size_t len;
    int FLAG; // flag to print useage page
	char *MODE; // "light" or "dark"
    char *CONF; // configuration path
	char *EVAR; // environment variable name
	char *EVAL; // environment variable value
    char SEQ[225];
    char CCUR[255]; // configuration path, current file
    char CSEQ[255]; // configuration path, sequence file
    char SEL[1023]; // selected palette
    char PRI[1023];
	char CLI[1023];
} p;

struct dirent *de;
