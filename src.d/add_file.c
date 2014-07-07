/*
** Copyright (c) 2003 - eternity, Alexis Megas.
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
  DIR *dirp1= 0;
  DIR *dirp2 = 0;
  char buffer[BUFF_SIZE];
  char *tmp = 0;
  struct dirent *dirent1 = 0;
  struct dirent *dirent2 = 0;
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
     stat(buffer, &stat_buf) == 0 &&
     S_ISREG(stat_buf.st_mode) && strcmp(argv[1], "admin") != 0)
    {
      (void) printf("<table cellpadding=0 width=\"100%%\" cellspacing=1 "
		    "border=0 bgcolor=\"DarkSlateGray\"><tr><td>\n");
      (void) printf("<table align=\"center\" width=\"100%%\" "
		    "cellpading=0 cellspacing=0 border=0 "
		    "bordercolor=\"#111111\">\n");
      (void) printf("<tr>\n");
      (void) printf("<td width=\"100%%\""
		    "bgcolor=\"#F4F5FB\" height=30 colspan=2>\n");
      (void) printf("<form action=\"%s/upload_script.pl\" method=\"post\" "
		    "enctype=\"multipart/form-data\">\n",
		    CGI_DIR);
      (void) printf("<input type=hidden value=\"%s\" name=\"userid\">",
		    argv[1]);
      (void) printf("<b>Step 1: </b>Press the <b>Browse</b> "
		    "button to select a file.<br>"
		    "<center><input type=\"file\" "
		    "size=64 name=\"uploaded\"></center>");
      (void) printf("<b>Step 2: </b>Select the directory that you wish "
		    "to save the file under.<br>");
      (void) printf("<center><select name=\"files\" size=1 single>\n");
      (void) printf("<option>New File</option>\n");

      /*
      ** Add the file names to the Selection List.
      */

      (void) memset(buffer, 0, sizeof(buffer));
      (void) snprintf(buffer, sizeof(buffer), "%s/%s/files", BACKUP_DIR,
		      argv[1]);

      if((dirp1 = opendir(buffer)) != 0)
	{
	  while((dirent1 = readdir(dirp1)) != 0)
	    if(!(strcmp(dirent1->d_name, ".") == 0 ||
		 strcmp(dirent1->d_name, "..") == 0))
	      {
		(void) memset(buffer, 0, sizeof(buffer));
		(void) snprintf(buffer, sizeof(buffer), "%s/%s/files/%s",
				BACKUP_DIR, argv[1], dirent1->d_name);

		if((dirp2 = opendir(buffer)) != 0)
		  while((dirent2 = readdir(dirp2)) != 0)
		    if(!(strcmp(dirent2->d_name, ".") == 0 ||
			 strcmp(dirent2->d_name, "..") == 0 ||
			 strncmp(dirent2->d_name, "delete.", 7) == 0))
		      {
			(void) printf("<option>%s</option>\n",
				      dirent1->d_name);
			break;
		      }

		if(dirp2 != 0)
		  {
		    (void) closedir(dirp2);
		    dirp2 = 0;
		  }
	      }

	  (void) closedir(dirp1);
	  dirp1 = 0;
	}

      (void) printf("</select></center>\n");
      (void) printf("<b>Step 3: </b>Press the <b>Upload</b> button to "
		    "upload the file.");
      (void) printf("<center>");
      (void) printf("<input type=\"submit\" value=\"Upload\"></form>");
      (void) printf("</center>");
      (void) printf("</td>\n");
      (void) printf("</tr>\n");
      (void) printf("</table>");
      (void) printf("</td></tr></table>");
    }
  else
    (void) printf(ERROR, __LINE__, __FILE__, HOME);

  (void) printf("| <a href=\"javascript:window.close()\">Close</a> |\n");
  (void) printf("</center>\n");
  (void) printf("</body></html>");
  return EXIT_SUCCESS;
}
