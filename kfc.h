struct
{
	int i;
	int cval;
	int randf;
	FILE *fp;
	DIR *dr;
	char *line;
	size_t len;
    char *sval;
	char *mode; // "light" or "dark"
    char *conf; // configuration path
	char *evar; // environment variable name
	char *eval; // environment variable value
    char seq[225];
    char ccur[255]; // configuration path, current file
    char cseq[255]; // configuration path, sequence file
    char sel[1023]; // selected palette
    char pri[1023];
	char cli[1023];
} p;

struct dirent *de;
