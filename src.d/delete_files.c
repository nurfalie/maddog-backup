/*
** Alexis Megas.
** (c) 2003.
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
  int i = 0;
  int j = 0;
  int fnd = 0;
  char *tmp = NULL;
  char buffer[BUFF_SIZE];
  char indata[65536];
  char dirname[BUFF_SIZE];
  char filename[BUFF_SIZE];
  char renameto[BUFF_SIZE];
  char renamefrom[BUFF_SIZE];

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html>");
  (void) printf("<title>Mad Dog Backup System</title>\n<body>");
  (void) printf("<center>\n");

  if(argc > 1 && argv[1] != NULL)
    {
      (void) printf("<meta http-equiv=\"refresh\" "
		    "content=\"5; url=%s/display_files.cgi?"
		    "%s&%d\">", CGI_DIR, argv[1], SORTBY_NAME);

      if(fgets(indata, (int) sizeof(indata) - 1, stdin) != NULL)
	{
	  tmp = strtok(indata, "&");

	  while(tmp != NULL)
	    {
	      (void) memset(buffer, 0, sizeof(buffer));
	      (void) memset(dirname, 0, sizeof(dirname));

	      for(i = 0, j = 0, fnd = 0; i < (int) strlen(tmp); i++)
		{
		  if(fnd == 1)
		    {
		      if(tmp[i] == '-')
			{
			  j = 0;
			  (void) snprintf(dirname, sizeof(dirname), "%s",
					  buffer);
			  (void) memset(buffer, 0, sizeof(buffer));
			}
		      else
			buffer[j++] = tmp[i];
		    }

		  if(tmp[i] == '=')
		    fnd = 1;
		}

	      (void) snprintf(filename, sizeof(filename), "%s", buffer);
	      (void) snprintf(renameto, sizeof(renameto),
			      "%s/%s/files/deleted/delete.%s",
			      BACKUP_DIR, argv[1], filename);
	      (void) snprintf(renamefrom, sizeof(renamefrom),
			      "%s/%s/files/%s/%s",
			      BACKUP_DIR, argv[1], dirname, filename);
	      (void) rename(renamefrom, renameto);
	      tmp = strtok(NULL, "&");
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
