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
  DIR *dirp1= NULL;
  DIR *dirp2 = NULL;
  char buffer[BUFF_SIZE];
  struct dirent *dirent1 = NULL;
  struct dirent *dirent2 = NULL;

  (void) printf("Content-type: text/html\n\n");
  (void) printf("<html>");
  (void) printf("<title>Mad Dog Backup System</title>\n<body>");
  (void) printf("<center>\n");

  if(argc > 1 && argv[1] != NULL)
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

      (void) snprintf(buffer, sizeof(buffer), "%s/%s/files", BACKUP_DIR,
		      argv[1]);

      if((dirp1 = opendir(buffer)) != NULL)
	{
	  while((dirent1 = readdir(dirp1)) != NULL)
	    if(!(strcmp(dirent1->d_name, ".") == 0 ||
		 strcmp(dirent1->d_name, "..") == 0))
	      {
		(void) snprintf(buffer, sizeof(buffer), "%s/%s/files/%s",
				BACKUP_DIR, argv[1], dirent1->d_name);

		if((dirp2 = opendir(buffer)) != NULL)
		  while((dirent2 = readdir(dirp2)) != NULL)
		    if(!(strcmp(dirent2->d_name, ".") == 0 ||
			 strcmp(dirent2->d_name, "..") == 0 ||
			 strncmp(dirent2->d_name, "delete.", (size_t) 7) == 0))
		      {
			(void) printf("<option>%s</option>\n",
				      dirent1->d_name);
			break;
		      }

		(void) closedir(dirp2);
	      }

	  (void) closedir(dirp1);
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

  (void) printf("<hr>| <a href=\"javascript:window.close()\">Close</a> |\n");
  (void) printf("</center>\n");
  (void) printf("</body></html>");
  return EXIT_SUCCESS;
}
