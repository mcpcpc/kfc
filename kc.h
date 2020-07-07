struct
{
    int cval;
    int errf;
    char *MODE; // "light" or "dark"
    char *CONF; // configuration path
    char SEL[1024]; // selected palette
    char SEQ[1024];
    char PRI[1024];
	char CLI[1024];
} p;
