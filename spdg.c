
/*
 * program 	- spdg
 * author	- aaron
 * date		- 2017/11/25
 * description	- generate all the passwords 
 * 		- that are composed of specify string
 *
 * update	- 2019/9/25
 *	hsociety() - generate password based on informations
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

#define LIMIT_INFO			64

#define STR_DIGIT			(1<<0)
#define STR_LOWER			(1<<1)
#define STR_UPPER			(1<<2)
#define STR_SYMBOL			(1<<3)
#define STR_SPECIFY			(1<<4)
#define STR_INFOS			(1<<5)
#define FILE_OUTPUT			(1<<6)
#define REPEAT				(1<<7)

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
static char *gotyou[LIMIT_INFO];
static char **infos;

static const struct option opts[] =
{
	{ "digit", 0, NULL, 'd' },
	{ "lower", 0, NULL, 'z' },
	{ "upper", 0, NULL, 'Z' },
	{ "symbol", 0, NULL, 's' },
	{ "specify", 1, NULL, 'S' },
	{ "infos", 0, NULL, 'i' },
	{ "repeat", 0, NULL, 'r' },
	{ "output", 1, NULL, 'o' },
	{ "help", 0, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

void usage();
void set_passwd_size(char **passwd, int size);
void set_passwd_str(char **passwd_str, char *const str);
void spawn_passwd(int cur, int len, int repeat);
void hsociety(int cur, int len, int repeat);

int is_digit(char *str);
int is_repeat_char(char *str, char target, int n);
int is_repeat_str(char *gotyou[], char *target, int n);

int main (int argc, char *argv[])
{
	char *specify, *file_name;
	int min, max, opt, info_flag = 0;
	unsigned int flags = 0;
	
	if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
	{
		usage();
		exit(EXIT_SUCCESS);
	}

	if (argc < 4)
	{
		fprintf(stderr, "Too few option or argument\n");
		goto GET_HELP;
	}

	if (!strcmp(argv[3], "-i") || !strcmp(argv[3], "--infos"))
		info_flag = 1;

	if (!is_digit(argv[1]) || !is_digit(argv[2]))
	{
		fprintf(stderr, "Argument 1 and 2 must be a number\n");
		goto GET_HELP;
	}

	sscanf(argv[1], "%d", &min);
	sscanf(argv[2], "%d", &max);

	if (min > max)
	{
		fprintf(stderr, "Argument 1 must bigger than argument 2\n");
		goto GET_HELP;
	}	

	while ((opt = getopt_long(argc, argv, ":dzZsS:iro:h", opts, NULL)) != -1)
	{
		if (info_flag && opt != 'i')
		{
			fprintf(stderr, "Option -i can only be used alone\n");
			goto GET_HELP;
		}

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
		case 'i':
			flags |= STR_INFOS;
			break;
		case 'r':
			flags |= REPEAT;
			break;
		case 'o':
			flags |= FILE_OUTPUT;
			file_name = optarg;
			break;
		case 'h':
			fprintf (stderr, "Option %c can only be used alone\n", opt);
			goto GET_HELP;
		case '?':
			fprintf (stderr, "Unknown option: %c\n", optopt);
			goto GET_HELP;
		case ':':
			fprintf (stderr, "Option %c need a vaule\n", opt);
			goto GET_HELP;
		}
	}

	if ((flags & FILE_OUTPUT) && !freopen(file_name, "w", stdout))
	{
		fprintf (stderr, "Open file %s failed\n", file_name);
		exit (EXIT_FAILURE);
	}

	if (flags & STR_INFOS)
	{
		if (!(infos = &argv[optind+2]))
		{
			fprintf(stderr, "There is no more informations\n");
			goto GET_HELP;
		}
		for (int i = min; i <= max; i++) hsociety(0, i, flags & REPEAT);

		goto DONE;
	}

	set_passwd_size(&passwd, max);

	if (flags & STR_DIGIT) set_passwd_str(&passwd_str, digit);
	if (flags & STR_LOWER) set_passwd_str(&passwd_str, lower);
	if (flags & STR_UPPER) set_passwd_str(&passwd_str, upper);
	if (flags & STR_SYMBOL) set_passwd_str(&passwd_str, symbol);
	if (flags & STR_SPECIFY) set_passwd_str(&passwd_str, specify);

	for (int i = min; i <= max; i++) spawn_passwd (0, i, flags & REPEAT);

	free (passwd);
	free (passwd_str);

DONE:
	return 0;

GET_HELP:
	fprintf (stderr, "Get help message with -h or --help\n");
	exit (EXIT_FAILURE);
}

void usage()
{
	printf("Usage: spdg <min> <max> <option[s]>\n");
	printf("\t-d, --digit			- 0 - 9\n");
	printf("\t-z, --lower			- a - z\n");
	printf("\t-Z, --upper			- A - Z\n");
	printf("\t-s, --symbol			- +-*/\\=@#$%^&?'\"`~()[]{}:;,.\n");
	printf("\t-S, --specify charset		- Use specified charset\n");
	printf("\t-i, --infos str1, str2...	- Generate password based on informations\n");
	printf("\t-h, --help			- Show help message\n");
}

int is_digit(char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
		if (str[i] > '9' || str[i] < '0')
			return 0;
	return 1;
}

void set_passwd_size (char **passwd, int size)
{
	*passwd = (char *)malloc(size + 1);
	CKALLOC(*passwd);
}

/* Set the charset */
void set_passwd_str(char **passwd_str, char *const str)
{
	if (*passwd_str == NULL)
	{
		*passwd_str = (char *)malloc(strlen (str) + 1);
		CKALLOC(*passwd_str);
		strcpy(*passwd_str, str);
	}
	else
	{
		*passwd_str = (char *)realloc(*passwd_str, 
			strlen(*passwd_str) + strlen(str) + 1);
		CKALLOC(*passwd_str);
		strcat(*passwd_str, str);
	}
}

int is_repeat_char(char *str, char target, int n)
{
	for (int i = 0; i < n; i++)
		if (str[i] == target) return 1;
	return 0;
}

/* Generate password based on charset */
void spawn_passwd (int cur, int len, int repeat)
{
	if (cur == len)
	{
		passwd[cur] = '\0';
		printf("%s\n", passwd);
		return;
	}

	for (int i = 0; passwd_str[i] != '\0'; i++)
	{
		if (!repeat && is_repeat_char(passwd, passwd_str[i], cur))
			continue;
		passwd[cur] = passwd_str[i];
		spawn_passwd(cur + 1, len, repeat);
	}
}

int is_repeat_str(char *gotyou[], char *target, int n)
{
	for (int i = 0; i < n; i++)
		if (gotyou[i] == target) return 1;
	return 0;
}

/* Generate password based on informations */
void hsociety(int cur, int len, int repeat)
{
	if (cur == len)
	{
		for (int i = 0; i < len; i++)
			printf("%s", gotyou[i]);
		printf("\n");
		return;
	}

	for (int i = 0; infos[i] != NULL; i++)
	{
		if (!repeat && is_repeat_str(gotyou, infos[i], cur))
			continue;
		gotyou[cur] = infos[i];
		hsociety(cur + 1, len, repeat);
	}
}
