/*
** Alexis Megas.
** (c) 2003 - 2014.
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
  char *tmp1 = 0;
  char tmp2[BUFF_SIZE];
  size_t i = 0;

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html>");
  (void) printf("<title>Mad Dog Backup System</title>\n<body>");
  (void) printf("<meta http-equiv=\"refresh\" "
		"content=\"5; url=%s\">", HOME);
  (void) printf("<center>\n");

  if(argc > 1 && argv[1] != 0)
    {
      (void) memset(tmp2, 0, sizeof(tmp2));
      (void) snprintf(tmp2, sizeof(tmp2), "%s", argv[1]);

      for(i = 0; i < strlen(tmp2); i++)
	if(tmp2[i] == '-')
	  {
	    tmp2[i] = '\0';
	    break;
	  }

      if(strlen(tmp2) == 0)
	return EXIT_FAILURE;

      if((tmp1 = getenv("REMOTE_ADDR")) == 0)
	tmp1 = "1";

      (void) snprintf(buffer, sizeof(buffer), "%s/data/bcksys.loggedin.%s.%s",
		      BACKUP_DIR, tmp2, tmp1);
      (void) remove(buffer);
    }

  (void) printf("You have logged out. Please wait to be redirected "
		"<a href=\"%s\">home</a>.", HOME);
  (void) printf("</center>\n");
  (void) printf("</body></html>");
  return EXIT_SUCCESS;
}
