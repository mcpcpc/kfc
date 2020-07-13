struct
{
	int i;
	int cval;
    int flag;
	int randf;
	FILE *fp;
	DIR *dr;
	char *line;
	size_t len;
	char *MODE; // "light" or "dark"
    char *CONF; // configuration path
    char SEQ[225];
    char CCUR[255]; // configuration path, current file
    char CSEQ[255]; // configuration path, sequence file
    char SEL[1025]; // selected palette
    char PRI[1025];
	char CLI[1025];
} p;
