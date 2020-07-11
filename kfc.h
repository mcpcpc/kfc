struct
{
	int i;
	int cval;
    int errf;
	int randf;
	FILE *fp;
	DIR *dr;
	char *line;
	size_t len;
	char *MODE; // "light" or "dark"
    char *CONF; // configuration path
    char CCUR[255]; // configuration path, current file
    char CSEQ[255]; // configuration path, sequence file
    char SEL[1024]; // selected palette
    char SEQ[1024];
    char PRI[1024];
	char CLI[1024];
} p;
