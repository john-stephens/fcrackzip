#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else
typedef enum { FALSE = 0, TRUE = 1 } bool;
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "getopt.h"
#endif
#ifdef HAVE_GETTIMEOFDAY
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#endif

#include <string.h>

#ifdef USE_UNIX_REDIRECTION
#define DEVNULL ">/dev/null 2>&1"
#else
#define DEVNULL ">NUL 2>&1"
#endif

#include "crack.h"
#include "callbacks.h"
#include "validate.h"
#include "brutegen.h"
#include "dictgen.h"

int use_unzip;

static method *crack_method = methods;
static int method_number = -1;
static int residuent = 0;
static int modul = 1;

int REGPARAM
check_unzip (const char *pw)
{
  char buff[1024];
  int status;

  sprintf (buff, "unzip -qqtP \"%s\" %s " DEVNULL, pw, file_path[0]);
  status = system (buff);

#undef REDIR

  if (status == EXIT_SUCCESS)
    {
      printf("\n\nPASSWORD FOUND!!!!: pw == %s\n", pw);
      exit (EXIT_SUCCESS);
    }

  return !status;
}

static int benchmark_count;

static int
benchmark_gen (void)
{
  if (!--benchmark_count)
    return 0;

  return brute_force_gen ();
}

static void
benchmark (void)
{
#ifdef HAVE_GETTIMEOFDAY
  int i;
  long j, k;
  struct timeval tv1, tv2;

  do
    {
      for (i = 0; i < HEADER_SIZE * 3; i++)
        files[i] = i ^ (i * 3);

      file_count = 3;
      strcpy (pw, "abcdefghij");
      parse_charset ("a");
      benchmark_count = BENCHMARK_LOOPS;

      verbosity = 0;

      printf ("%c%s: ",
              (crack_method - methods == default_method) ? '*' : ' ',
              crack_method->desc);

      if (strncmp ("zip", crack_method->desc, 3))
        printf ("(skipped)");
      else
        {
          fflush (stdout);

          crack_method->init_crack_pw ();
          gettimeofday (&tv1, 0);
          crack_method->crack_pw (benchmark_gen, false_callback);
          gettimeofday (&tv2, 0);
          tv2.tv_sec -= tv1.tv_sec;
          tv2.tv_usec -= tv1.tv_usec;

          j = tv2.tv_sec * 1000000 + tv2.tv_usec;
          k = BENCHMARK_LOOPS;

          printf ("cracks/s = ");

          for (i = 7; i--;)
            printf ("%ld", k / j), k = (k - k / j * j) * 10;
        }

      printf ("\n");
      crack_method++;
    }
  while (method_number < 0 && crack_method->desc);
#else
  fprintf (stderr, "This executable was compiled without support for benchmarking\n");
  exit (1);
#endif
}

static void
usage (int ec)
{
  printf ("\n"
          PACKAGE " version " VERSION ", a fast/free zip password cracker\n"
          "written by Marc Lehmann <pcg@goof.com> You can find more info on\n"
          "http://www.goof.com/pcg/marc/\n"
          "\n"
          "USAGE: fcrackzip\n"
          "          [-b|--brute-force]            use brute force algorithm\n"
          "          [-D|--dictionary]             use a dictionary\n"
          "          [-B|--benchmark]              execute a small benchmark\n"
          "          [-c|--charset characterset]   use characters from charset\n"
          "          [-h|--help]                   show this message\n"
          "          [--version]                   show the version of this program\n"
          "          [-V|--validate]               sanity-check the algortihm\n"
          "          [-v|--verbose]                be more verbose\n"
          "          [-p|--init-password string]   use string as initial password/file\n"
          "          [-l|--length min-max]         check password with length min to max\n"
          "          [-u|--use-unzip]              use unzip to weed out wrong passwords\n"
          "          [-m|--method num]             use method number \"num\" (see below)\n"
          "          [-2|--modulo r/m]             only calculcate 1/m of the password\n"
          "          file...                    the zipfiles to crack\n"
          "\n"
    );

  printf ("methods compiled in (* = default):\n\n");
  for (crack_method = methods; crack_method->desc; crack_method++)
    printf ("%c%d: %s\n",
            (crack_method - methods == default_method) ? '*' : ' ',
            crack_method - methods,
            crack_method->desc);

  printf ("\n");
  exit (ec);
}

static struct option options[] =
{
  {"version", no_argument, 0, 'R'},
  {"brute-force", no_argument, 0, 'b'},
  {"dictionary", no_argument, 0, 'D'},
  {"benchmark", no_argument, 0, 'B'},
  {"charset", required_argument, 0, 'c'},
  {"help", no_argument, 0, 'h'},
  {"validate", no_argument, 0, 'V'},
  {"verbose", no_argument, 0, 'v'},
  {"init-password", required_argument, 0, 'p'},
  {"length", required_argument, 0, 'l'},
  {"use-unzip", no_argument, 0, 'u'},
  {"method", required_argument, 0, 'm'},
  {"modulo", required_argument, 0, 2},
  {0, 0, 0, 0},
};

int
main (int argc, char *argv[])
{
  int c;
  int option_index = 0;
  char *charset = "aA1!";
  enum { m_benchmark, m_brute_force, m_dictionary } mode = m_brute_force;
  int min_length = -1;
  int max_length = -1;

  while ((c = getopt_long (argc, argv, "DbBc:hVvp:l:um:2:", options, &option_index)) != -1)
    switch (c)
      {
      case 'b':
        mode = m_brute_force;
        break;

      case 'D':
        mode = m_dictionary;
        break;

      case 'p':
        strcpy (pw, optarg);
        break;

      case 'l':
        pw[0] = 0;
        switch (sscanf (optarg, "%d-%d", &min_length, &max_length))
          {
          default:
            fprintf (stderr, "'%s' is an incorrect length specification\n", optarg);
            exit (1);
          case 1:
            max_length = min_length;
          case 2:
            ;
          }
        break;

      case 2:
        if (sscanf (optarg, "%d/%d", &residuent, &modul) != 2)
          fprintf (stderr, "malformed --modulo option, expected 'residuent/modul'\n"), exit (1);

        if (residuent < 0 || modul <= 0)
          fprintf (stderr, "residuent and modul must be positive\n"), exit (1);

        if (residuent >= modul)
          fprintf (stderr, "residuent must be less than modul\n"), exit (1);

        break;

      case 'B':
        mode = m_benchmark;
        benchmark ();
        exit (0);

      case 'v':
        verbosity++;
        break;

      case 'm':
        {
          for (method_number = 0; methods[method_number].desc; method_number++)
            if (!strncmp (methods[method_number].desc, optarg, strlen (optarg)))
              break;

          if (!methods[method_number].desc)
            method_number = atoi (optarg);

          crack_method = methods + method_number;
        }
        break;

      case 'V':
        // @todo BUG: Depends on method being passed before validate flag
        validate (crack_method);
        exit (0);

      case 'c':
        charset = optarg;
        break;

      case 'u':
        use_unzip = 1;
        break;

      case 'h':
        usage (0);
      case 'R':
        printf (PACKAGE " version " VERSION "\n");
        exit (0);

      case ':':
        fprintf (stderr, "required argument missing\n");
        exit (1);

      case '?':
        fprintf (stderr, "unknown option\n");
        exit (1);

      default:
        usage (1);
      }

  if (method_number < 0)
    {
      method_number = default_method;
      crack_method = methods + default_method;
    }

  if (optind >= argc)
    {
      fprintf (stderr, "you have to specify one or more zip files (try --help)\n");
      exit (1);
    }

  for (; optind < argc; optind++)
    if (file_count < MAX_FILES)
      crack_method->load_file (argv[optind]);
    else if (verbosity)
      printf ("%d file maximum reached, ignoring '%s'\n", MAX_FILES, argv[optind]);

  if (file_count == 0)
    {
      fprintf (stderr, "no usable files found\n");
      exit (1);
    }

  crack_method->init_crack_pw ();

  switch (mode)
    {
    case m_brute_force:
      parse_charset (charset);

      if (!pw[0])
        {
          if (min_length < 0)
            {
              fprintf (stderr, "you have to specify either --init-password or --length with --brute-force\n");
              exit (1);
            }
          else
            {
              set_brute_force_length (min_length, max_length);
            }
        }

      if (residuent)
        {
          int xmodul = modul;
          modul = residuent;
          pw_end = pw + strlen (pw);
          brute_force_gen ();
          printf ("%s\n", pw);
          modul = xmodul;
          printf ("WARNING: residuent mode NOT supported YET!\n");
        }

      crack_method->crack_pw (brute_force_gen, print_callback);
      break;

    case m_dictionary:
      if (!pw[0])
        {
          fprintf (stderr, "you have to specify a file to read passwords from using the -p switch\n");
          exit (1);
        }

      if (init_dictionary_gen (pw))
        {
          perror (pw);
          exit (1);
        }
      else
        {
          crack_method->crack_pw (dictionary_gen, print_callback);
          finish_dictionary_gen ();
        }

      break;

    default:
      fprintf (stderr, "specified mode not supported in this version\n");
      exit (1);
    }

  return 0;
}
