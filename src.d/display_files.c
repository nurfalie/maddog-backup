/*
** Copyright (c) 2003 - eternity, Alexis Megas.
*/

#include <ctype.h>

/*
** -- Local Includes --
*/

#include "common.h"

extern char *crypt(const char *key, const char *salt);

static int isPasswordCorrect(const char *line, const char *password)
{
  char *output = 0;
  char salt[20];
  int rc = 0;
  size_t i = 0;

  /*
  ** A newline is stored in line.
  */

  if(!line || !password ||
     strnlen(line, 107) != 107 || strnlen(password, 35) == 35)
    {
      rc = 0;
      goto done_label;
    }

  (void) memset(salt, 0, sizeof(salt));
  (void) strncpy(salt, line, sizeof(salt) - 1);
  output = crypt(password, salt);

  if(output && strnlen(output, 106) == 106)
    {
      /*
      ** Both line and output should be at least 106 characters long.
      ** x ^ y returns zero if x and y are identical.
      */

      for(i = 0; i < strlen(line) - 1; i++)
	rc |= line[i] ^ output[i];

      if(rc == 0)
	rc = 1;
      else
	rc = 0;
    }
  else
    rc = 0;

 done_label:
  return rc;
}

static int savePassword(const char *password, FILE *fp)
{
  char c;
  char *output = 0;
  char salt[17];
  char sha512[128];
  int fd = -1;
  int rc = 1;
  size_t i = 0;

  if(!fp || !password ||
     strnlen(password, 32) == 0 || strnlen(password, 35) == 35)
    {
      rc = 1;
      goto done_label;
    }

  if((fd = open("/dev/random", O_RDONLY)) == -1)
    {
      rc = 1;
      goto done_label;
    }

  (void) memset(salt, 0, sizeof(salt));

  while(1)
    {
      c = 0;

      if(read(fd, &c, 1) == 1)
	{
	  if(c == '.' || c == '/' || isalnum(c))
	    {
	      salt[i] = c;
	      i += 1;

	      if(i >= sizeof(salt) - 1)
		{
		  rc = 0;
		  break;
		}
	    }
	}
      else
	{
	  rc = 1;
	  break;
	}
    }

  if(rc == 1)
    goto done_label;

  (void) memset(sha512, 0, sizeof(sha512));
  (void) snprintf(sha512, sizeof(sha512), "$6$%s", salt);
  output = crypt(password, sha512);

  if(output)
    {
      if(fputs(output, fp) != EOF && fputs("\n", fp) != EOF)
	rc = 0;
      else
	rc = 1;
    }
  else
    rc = 1;

 done_label:
  close(fd);

  return rc;
}

static void secure_memset(char *buffer, const char c, const size_t size)
{
  size_t s = size;
  volatile char *ptr = buffer;

  if(!ptr)
    return;

  while(s--)
    *ptr++ = c;
}

int main(int argc, char *argv[])
{
  FILE *fp = 0;
  char buffer[BUFF_SIZE];
  char deldir[BUFF_SIZE + 7]; /* strlen("deleted") */
  char *endptr;
  char indata[BUFF_SIZE];
  char *tmp1 = 0;
  char *tmp2 = 0;
  char unknown[] = "unknown";
  char *userid = 0;
  int set = 1;
  int sortby = SORTBY_NAME;
  mode_t mode;
  struct stat stat_buf;

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html><body>\n");
  (void) printf("<title>Mad Dog Backup System</title>\n");

  if(argc > 1 && argv[1] != 0)
    {
      if((tmp1 = strtok(argv[1], "\\&")) != 0)
	userid = tmp1;
      else
	userid = unknown;

      if((tmp1 = strtok(0, "\\&")) != 0)
	{
	  sortby = (int) strtol(tmp1, &endptr, 10);

	  if(errno == EINVAL || errno == ERANGE || endptr == tmp1)
	    sortby = SORTBY_NAME;
	}
    }
  else
    userid = unknown;

  if(!isValidId(userid))
    {
      (void) printf(ERROR, __LINE__, __FILE__, HOME);
      return EXIT_SUCCESS;
    }

  if(argc > 1 && argv[1] != 0 && strcmp(argv[1], "admin") != 0)
    {
      (void) printf("<script>\n");
      (void) printf("function checkBoxSelect(val)\n"
		    "{\n"
		    "if(!document.forms) return;\n"
		    "if(document.forms.mainform.cblist && "
		    "document.forms.mainform."
		    "cblist.length)"
		    "for(var i = 0; i < document.forms.mainform"
		    ".cblist.length; i++)"
		    "document.forms.mainform.cblist[i].checked = val;"
		    "else if(document.forms.mainform.cblist)"
		    "document.forms.mainform.cblist.checked = val;\n"
		    "}\n");
      (void) printf("</script>\n");
    }

  if(argc > 1 && argv[1] != 0)
    {
      if((tmp1 = getenv("REMOTE_ADDR")) == 0)
	tmp1 = "1";

      (void) memset(buffer, 0, sizeof(buffer));
      (void) snprintf(buffer, sizeof(buffer), "/%s/data/bcksys.loggedin.%s."
		      "%s", BACKUP_DIR, argv[1], tmp1);

      if(stat(buffer, &stat_buf) == 0 && S_ISREG(stat_buf.st_mode))
	displayFiles(argv[1], sortby);
      else
	set = 0;
    }
  else
    set = -1;

  if(argc > 1 && argv[1] != 0 && set == 0 && stdin != 0)
    {
      (void) memset(buffer, 0, sizeof(buffer));
      (void) memset(indata, 0, sizeof(indata));
      (void) snprintf(buffer, sizeof(buffer), "%s/data/passwd.%s",
		      BACKUP_DIR, argv[1]);

      if(fgets(indata, (int) sizeof(indata), stdin) == 0)
	(void) printf(ERROR, __LINE__, __FILE__, HOME);
      else
	{
	  if((fp = fopen(buffer, "r")) != 0)
	    {
	      if(stat(buffer, &stat_buf) == 0 && S_ISREG(stat_buf.st_mode))
		{
		  if(fgets(buffer, (int) sizeof(buffer), fp) != 0)
		    {
		      if((tmp1 = indata) != 0)
			{
			  if(!isPasswordCorrect(buffer, tmp1))
			    (void) printf("<center>Password is not correct. "
					  "Please "
					  "<a href=\"%s/lookup_user.cgi?%s\">"
					  "re-enter</a> "
					  "your password.</center>\n",
					  CGI_DIR,
					  argv[1]);
			  else
			    displayFiles(argv[1], sortby);
			}
		      else
			(void) printf(ERROR, __LINE__, __FILE__, HOME);
		    }
		  else
		    (void) printf(ERROR, __LINE__, __FILE__, HOME);

		  (void) fclose(fp);
		  fp = 0;
		}
	      else
		{
		  (void) fclose(fp);
		  fp = 0;
		  (void) printf(ERROR, __LINE__, __FILE__, HOME);
		}
	    }
	  else if((tmp1 = strtok(indata, "&")) == 0)
	    (void) printf(ERROR, __LINE__, __FILE__, HOME);
	  else if((tmp2 = strtok(0, "&")) == 0)
	    (void) printf(ERROR, __LINE__, __FILE__, HOME);
	  else if(strcmp(tmp1, tmp2) != 0 || strcmp(tmp1, "pass=") == 0)
	    (void) printf("<center>Password entries do not match. Please "
			  "<a href=\"%s/lookup_user.cgi?%s\">re-enter</a> "
			  "your password.</center>\n", CGI_DIR, argv[1]);
	  else if(strnlen(tmp1, 35) < 10 + strlen("pass="))
	    (void) printf("<center>Password must be at least ten characters "
			  "long. "
			  "Please "
			  "<a href=\"%s/lookup_user.cgi?%s\">re-enter</a> "
			  "your password.</center>\n", CGI_DIR, argv[1]);
	  else if(stat(buffer, &stat_buf) != 0)
	    {
	      (void) umask((mode_t) ~S_IRWXU);

	      if((fp = fopen(buffer, "w")) != 0)
		{
		  if(savePassword(tmp1, fp) == 0)
		    {
		      /*
		      ** Create the files and deleted directories.
		      */

		      mode = (mode_t) PERMISSIONS;
		      (void) umask(~mode);
		      (void) memset(buffer, 0, sizeof(buffer));
		      (void) snprintf(buffer, sizeof(buffer), "%s/%s/files",
				      BACKUP_DIR, argv[1]);
		      (void) memset(deldir, 0, sizeof(deldir));
		      (void) snprintf(deldir, sizeof(deldir),
				      "%s/%s/files/deleted",
				      BACKUP_DIR, argv[1]);

		      if(mkdir(buffer, mode) != 0 && errno != EEXIST)
			(void) printf(ERROR, __LINE__, __FILE__, HOME);
		      else if(mkdir(deldir, mode) != 0 && errno != EEXIST)
			(void) printf(ERROR, __LINE__, __FILE__, HOME);
		      else
			displayFiles(argv[1], sortby);
		    }
		  else
		    (void) printf(ERROR, __LINE__, __FILE__, HOME);

		  fclose(fp);
		  fp = 0;
		}
	      else
		(void) printf(ERROR, __LINE__, __FILE__, HOME);
	    }
	}
    }
  else if(set == -1)
    (void) printf(ERROR, __LINE__, __FILE__, HOME);

  (void) printf("</body></html>\n");
  secure_memset(buffer, 0, sizeof(buffer));
  secure_memset(indata, 0, sizeof(indata));
  tmp1 = tmp2 = 0;
  return EXIT_SUCCESS;
}
