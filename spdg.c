
/*
 * program 	- spdg
 * author	- aaron
 * date		- 2017/11/25
 * description	- generate all the passwords 
 * 		- that are composed of specify string
 */

#define _GNU_SOURCE
#include <getopt.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EXIT_FAILURE
#define EXIT_FAILURE			1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS			0
#endif

#define STR_DIGIT			(1<<0)
#define STR_LOWER			(1<<1)
#define STR_UPPER			(1<<2)
#define STR_SYMBOL			(1<<3)
#define STR_SPECIFY			(1<<4)
#define FILE_OUTPUT			(1<<5)

#define CKALLOC(mem_ptr)		if (!mem_ptr)			\
					{				\
						perror ("Error");	\
						exit (EXIT_FAILURE);	\
					}

static char *const lower 	= 	"abcdefghijklmnopqrstuvwxyz";
static char *const upper 	= 	"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *const digit 	= 	"0123456789";
static char *const symbol 	=	 "+-*/\\=@#$%^&?'\"`~()[]{}:;,.";

static char *passwd_str 	= 	NULL;
static char *passwd		= 	NULL;

static const struct option long_opts[] = 
{
	{"digit", 0, NULL, 'd'},
	{"lower", 0, NULL, 'z'},
	{"upper", 0, NULL, 'Z'},
	{"symbol", 0, NULL, 's'},
	{"specify", 1, NULL, 'S'},
	{"output", 1, NULL, 'o'},
	{"help", 0, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

void usage ();
int is_digit (char *str);
void set_passwd_size (char **passwd, int size);
void set_passwd_str (char **passwd_str, char *const str);
void spawn_passwd (int pos, int passwd_len);

int main (int argc, char *argv[])
{
	char *specify, *file_name;
	int passwd_len_min, passwd_len_max;
	unsigned int flags = 0;
	int len, opt;
	
	if (argc == 2 && 
		(!strcmp (argv[1], "-h") || !strcmp (argv[1], "--help")))
	{
		usage ();
		exit (EXIT_SUCCESS);
	}

	if (argc < 4)
	{
		fprintf (stderr, "Too few option or argument\n");
		goto GET_HELP;
	}

	if ((!is_digit (argv[1]) || !is_digit (argv[2])))
	{
		fprintf (stderr, "Argument 1 and 2 must be a number\n");
		goto GET_HELP;
	}

	sscanf (argv[1], "%d", &passwd_len_min);
	sscanf (argv[2], "%d", &passwd_len_max);

	if (passwd_len_min > passwd_len_max)
	{
		fprintf (stderr, "Argument 1 must bigger than argument 2\n");
		goto GET_HELP;
	}
	
	set_passwd_size (&passwd, passwd_len_max);

	while ((opt = getopt_long 
		(argc, argv, ":dzZsS:o:h", long_opts, NULL)) != -1)
	{
		switch (opt)
		{
		case 'd':
			flags |= STR_DIGIT;
			break;
		case 'z':
			flags |= STR_LOWER;
			break;
		case 'Z':
			flags |= STR_UPPER;
			break;
		case 's':
			flags |= STR_SYMBOL;
			break;
		case 'S':
			flags |= STR_SPECIFY;
			specify = optarg;
			break;
		case 'o':
			flags |= FILE_OUTPUT;
			file_name = optarg;
			break;
		case 'h':
			fprintf (stderr, "Option %c must use alone\n", opt);
			goto GET_HELP;
		case '?':
			fprintf (stderr, "Unknown option: %c\n", optopt);
			goto GET_HELP;
		case ':':
			fprintf (stderr, "Option %c need a vaule\n", opt);
			goto GET_HELP;
		}
	}

	if (argc - optind > 2)
	{
		fprintf (stderr, "Program has extra argument\n");
		goto GET_HELP;
	}

	if (flags & FILE_OUTPUT)
	{
		if (!freopen (file_name, "w", stdout))
		{
			fprintf (stderr, "Open file %s failed\n", file_name);
			exit (EXIT_FAILURE);
		}
	}

	if (flags & STR_DIGIT)
	{
		set_passwd_str (&passwd_str, digit);
	}
	if (flags & STR_LOWER)
	{
		set_passwd_str (&passwd_str, lower);
	}
	if (flags & STR_UPPER)
	{
		set_passwd_str (&passwd_str, upper);
	}
	if (flags & STR_SYMBOL)
	{
		set_passwd_str (&passwd_str, symbol);
	}
	if (flags & STR_SPECIFY)
	{
		set_passwd_str (&passwd_str, specify);
	}

	for (len = passwd_len_min; len <= passwd_len_max; len++)
	{
		spawn_passwd (0, len);
	}

	free (passwd);
	free (passwd_str);

	return 0;

GET_HELP:
	fprintf (stderr, "Get help message with -h or --help\n");
	exit (EXIT_FAILURE);
}

/*
 * usage - Print help massage
 */
void usage ()
{
	printf ("Usage: spdg <min> <max> <option[s]>\n");
	printf ("\t-d, --digit		- 0 - 9\n");
	printf ("\t-z, --lower		- a - z\n");
	printf ("\t-Z, --upper		- A - Z\n");
	printf ("\t-s, --symbol		- +-*/\\=@#$%^&?'\"`~()[]{}:;,.\n");
	printf ("\t-S, --specify str	- the str is user input\n");
	printf ("\t-h, --help		- display this help message\n");
}

/*
 * is_digit - determine if it is a number
 */
int is_digit (char *str)
{
	int i = 0;

	while (str[i])
	{
		if (str[i] > '9' || str[i++] < '0')
		{
			return 0;
		}
	}

	return 1;
}

/*
 * set_passwd_size - set the size of passwd[]
 */
void set_passwd_size (char **passwd, int size)
{
	*passwd = (char *)malloc (size + 1);
	CKALLOC (*passwd);
}

/*
 * set_passwd_str - set the string to generate passwords
 */
void set_passwd_str (char **passwd_str, char *const str)
{
	if (*passwd_str == NULL)
	{
		*passwd_str = (char *)malloc (strlen (str) + 1);
		CKALLOC (*passwd_str);
		strcpy (*passwd_str, str);
	}
	else
	{
		*passwd_str = (char *)realloc (*passwd_str, 
			strlen (*passwd_str) + strlen (str) + 1);
		CKALLOC (*passwd_str);
		strcat (*passwd_str, str);
	}
}

/*
 * spawn_passwd - generate passwords
 */
void spawn_passwd (int pos, int passwd_len)
{
	int i = 0;

	if (passwd_len == 0)
	{
		passwd[pos] = '\0';
		printf ("%s\n", passwd);
		return ;
	}

	while (passwd_str[i])
	{
		passwd[pos] = passwd_str[i++];
		spawn_passwd (pos + 1, passwd_len - 1);
	}
}
