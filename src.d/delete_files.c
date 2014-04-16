/*
** Copyright (c) 2003 - Eternity, Alexis Megas.
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
  char buffer[BUFF_SIZE];
  char dirname[BUFF_SIZE];
  char filename[BUFF_SIZE];
  char indata[65536];
  char renamefrom[BUFF_SIZE];
  char renameto[BUFF_SIZE];
  char *tmp = 0;
  int fnd = 0;
  size_t i = 0;
  size_t j = 0;
  struct stat stat_buf;

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html>");
  (void) printf("<title>Mad Dog Backup System</title>\n<body>");
  (void) printf("<center>\n");

  if((tmp = getenv("REMOTE_ADDR")) == 0)
    tmp = "1";

  (void) memset(buffer, 0, sizeof(buffer));

  if(argc > 1 && argv[1] != 0)
    (void) snprintf(buffer, sizeof(buffer), "/%s/data/bcksys.loggedin.%s."
		    "%s", BACKUP_DIR, argv[1], tmp);

  if(argc > 1 && argv[1] != 0 && isValidId(argv[1]) &&
     stat(buffer, &stat_buf) == 0 && stdin != 0)
    {
      (void) printf("<meta http-equiv=\"refresh\" "
		    "content=\"5; url=%s/display_files.cgi?"
		    "%s&%d\">", CGI_DIR, argv[1], SORTBY_NAME);
      (void) memset(indata, 0, sizeof(indata));

      if(fgets(indata, (int) sizeof(indata), stdin) != 0)
	{
	  tmp = strtok(indata, "&");

	  while(tmp != 0)
	    {
	      (void) memset(buffer, 0, sizeof(buffer));
	      (void) memset(dirname, 0, sizeof(dirname));

	      for(fnd = 0, i = 0, j = 0; i < strlen(tmp); i++)
		{
		  if(fnd == 1)
		    {
		      if(tmp[i] == '-')
			{
			  j = 0;
			  (void) memset(dirname, 0, sizeof(dirname));
			  (void) snprintf(dirname, sizeof(dirname), "%s",
					  buffer);
			  (void) memset(buffer, 0, sizeof(buffer));
			}
		      else if(j < sizeof(buffer))
			buffer[j++] = tmp[i];
		    }

		  if(tmp[i] == '=')
		    fnd = 1;
		}

	      (void) memset(filename, 0, sizeof(filename));
	      (void) snprintf(filename, sizeof(filename), "%s", buffer);
	      (void) memset(renameto, 0, sizeof(renameto));
	      (void) snprintf(renameto, sizeof(renameto),
			      "%s/%s/files/deleted/delete.%s",
			      BACKUP_DIR, argv[1], filename);
	      (void) memset(renamefrom, 0, sizeof(renamefrom));
	      (void) snprintf(renamefrom, sizeof(renamefrom),
			      "%s/%s/files/%s/%s",
			      BACKUP_DIR, argv[1], dirname, filename);
	      (void) rename(renamefrom, renameto);
	      tmp = strtok(0, "&");
	    }

	  (void) printf("Your file(s) has(have) been scheduled for deletion. "
			"Please wait to be "
			"<a href=\"%s/display_files.cgi?%s&%d"
			"\">redirected</a>.", CGI_DIR, argv[1], SORTBY_NAME);
			
	}
      else
	(void) printf("You have not selected any files to be deleted. "
		      "Please wait to be "
		      "<a href=\"%s/display_files.cgi?%s&%d\">"
		      "redirected</a>.", CGI_DIR, argv[1], SORTBY_NAME);
    }
  else
    (void) printf(ERROR, __LINE__, __FILE__, HOME);

  (void) printf("</center>\n");
  (void) printf("</body></html>");
  return EXIT_SUCCESS;
}
