/*
** Alexis Megas.
** (c) 2003, 2006.
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
  int set = 1;
  int sortby = SORTBY_NAME;
  char one[] = "1";
  char *tmp1 = NULL;
  char *tmp2 = NULL;
  char indata[128];
  char buffer[BUFF_SIZE];
  char deldir[BUFF_SIZE + 7]; /* strlen("deleted") */
  char *userid = NULL;
  char unknown[] = "UNKNOWN";
  FILE *fp = NULL;
  mode_t mode;

  (void) userid;
  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html><body>\n");
  (void) printf("<title>Mad Dog Backup System</title>\n");

  if(argc > 1 && argv[1] != NULL)
    {
      if((tmp1 = strtok(argv[1], "\\&")) != NULL)
	userid = tmp1;
      else
	userid = unknown;

      if((tmp1 = strtok(NULL, "\\&")) != NULL)
	sortby = atoi(tmp1);
    }
  else
    userid = unknown;

  if(argc > 1 && argv[1] != NULL && strcmp(argv[1], "admin") != 0)
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

  if(argc > 1 && argv[1] != NULL)
    {
      if((tmp1 = getenv("REMOTE_ADDR")) == NULL)
	tmp1 = one;

      (void) snprintf(buffer, sizeof(buffer), "/%s/data/bcksys.loggedin.%s."
		      "%s", BACKUP_DIR, argv[1], tmp1);

      if(access(buffer, F_OK) == 0)
	displayFiles(argv[1], sortby);
      else
	set = 0;
    }
  else
    set = -1;

  if(set == 0)
    {
      (void) memset(indata, 0, sizeof(indata));
      (void) snprintf(buffer, sizeof(buffer), "%s/data/passwd.%s",
		      BACKUP_DIR, argv[1]);

      if(fgets(indata, (int) sizeof(indata), stdin) == NULL)
	(void) printf(ERROR, __LINE__, __FILE__, HOME);
      else
	{
	  if(access(buffer, F_OK) == 0)
	    {
	      if((fp = fopen(buffer, "r")) != NULL)
		{
		  if(fgets(buffer, (int) sizeof(buffer), fp) != NULL)
		    {
		      if((tmp1 = indata) != NULL)
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
		}
	      else
		(void) printf(ERROR, __LINE__, __FILE__, HOME);
	    }
	  else if((tmp1 = strtok(indata, "&")) == NULL)
	    (void) printf(ERROR, __LINE__, __FILE__, HOME);
	  else if((tmp2 = strtok(NULL, "&")) == NULL)
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

	      if((fp = fopen(buffer, "w")) != NULL)
		{
		  if(fputs(tmp1, fp) == EOF)
		    (void) printf(ERROR, __LINE__, __FILE__, HOME);

		  (void) fclose(fp);

		  /*
		  ** Create the files and deleted directories.
		  */

		  mode = (mode_t) PERMISSIONS;
		  (void) umask(~mode);
		  (void) snprintf(buffer, sizeof(buffer), "%s/%s/files",
				  BACKUP_DIR, argv[1]);
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
