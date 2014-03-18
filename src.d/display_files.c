/*
** Copyright (c) 2003 - Eternity Alexis Megas.
*/

/*
** -- Local Includes --
*/

#include "common.h"

/*
** -- Local Globals --
*/

int main(int argc, char *argv[])
{
  FILE *fp = 0;
  char buffer[BUFF_SIZE];
  char deldir[BUFF_SIZE + 7]; /* strlen("deleted") */
  char indata[BUFF_SIZE];
  char one[] = "1";
  char *tmp1 = 0;
  char *tmp2 = 0;
  char unknown[] = "UNKNOWN";
  char *userid = 0;
  int set = 1;
  int sortby = SORTBY_NAME;
  mode_t mode;

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html><body>\n");
  (void) printf("<title>Mad Dog Backup System</title>\n");
  (void) userid;

  if(argc > 1 && argv[1] != 0)
    {
      if((tmp1 = strtok(argv[1], "\\&")) != 0)
	userid = tmp1;
      else
	userid = unknown;

      if((tmp1 = strtok(0, "\\&")) != 0)
	sortby = atoi(tmp1);
    }
  else
    userid = unknown;

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
	tmp1 = one;

      (void) memset(buffer, 0, sizeof(buffer));
      (void) snprintf(buffer, sizeof(buffer), "/%s/data/bcksys.loggedin.%s."
		      "%s", BACKUP_DIR, argv[1], tmp1);

      if(access(buffer, F_OK) == 0)
	displayFiles(argv[1], sortby);
      else
	set = 0;
    }
  else
    set = -1;

  if(argc > 1 && argv[1] != 0 && set == 0)
    {
      (void) memset(buffer, 0, sizeof(buffer));
      (void) memset(indata, 0, sizeof(indata));
      (void) snprintf(buffer, sizeof(buffer), "%s/data/passwd.%s",
		      BACKUP_DIR, argv[1]);

      if(fgets(indata, (int) sizeof(indata), stdin) == 0)
	(void) printf(ERROR, __LINE__, __FILE__, HOME);
      else
	{
	  if(access(buffer, F_OK) == 0)
	    {
	      if((fp = fopen(buffer, "r")) != 0)
		{
		  if(fgets(buffer, (int) sizeof(buffer), fp) != 0)
		    {
		      if((tmp1 = indata) != 0)
			{
			  if(strcmp(buffer, tmp1) != 0)
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
		(void) printf(ERROR, __LINE__, __FILE__, HOME);
	    }
	  else if((tmp1 = strtok(indata, "&")) == 0)
	    (void) printf(ERROR, __LINE__, __FILE__, HOME);
	  else if((tmp2 = strtok(0, "&")) == 0)
	    (void) printf(ERROR, __LINE__, __FILE__, HOME);
	  else if(strcmp(tmp1, tmp2) != 0 || strcmp(tmp1, "pass=") == 0)
	    (void) printf("<center>Password entries do not match. Please "
			  "<a href=\"%s/lookup_user.cgi?%s\">re-enter</a> "
			  "your password.</center>\n", CGI_DIR, argv[1]);
	  else if(strlen(tmp1) < 10 + strlen("pass="))
	    (void) printf("<center>Password must be at least ten characters "
			  "long. "
			  "Please "
			  "<a href=\"%s/lookup_user.cgi?%s\">re-enter</a> "
			  "your password.</center>\n", CGI_DIR, argv[1]);
	  else if(access(buffer, F_OK) != 0)
	    {
	      (void) umask((mode_t) ~S_IRWXU);

	      if((fp = fopen(buffer, "w")) != 0)
		{
		  if(fputs(tmp1, fp) == EOF)
		    (void) printf(ERROR, __LINE__, __FILE__, HOME);

		  (void) fclose(fp);
		  fp = 0;

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
	    }
	}
    }
  else if(set == -1)
    (void) printf(ERROR, __LINE__, __FILE__, HOME);

  (void) printf("</body></html>\n");
  return EXIT_SUCCESS;
}
