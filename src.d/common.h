/*
** Copyright (c) 2003 - Eternity Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*
** -- Numeric Defines --
*/

#define BUFF_SIZE 512
#define FALSE 0
#define PERMISSIONS (S_IRWXU | S_IRWXG)
#define TRUE 1

/*
** -- String Defines --
*/

#define ERROR "<center>An unexpected error occurred at line %d in file %s." \
  "<br><a href=\"%s\">Return</a> to the main screen.</center>\n"
#define FBEG "<font size=3 color=\"wheat\"><b>"
#define FEND "</b></font>"
#define SORTBY_DATE 2
#define SORTBY_NAME 1
#define SORTBY_SIZE 3

/*
** -- Global Variables --
*/

#include "USERS.dat"

/*
** -- Function Prototypes --
*/

void displayFiles(const char *, const int);
static int date_cmp(const void *, const void *);
static int name_cmp(const void *, const void *);
static int size_cmp(const void *, const void *);
static void getRel(const char *, char *, const size_t);

/*
** -- Structures --
*/

struct file_info
{
  char dirname[_POSIX_PATH_MAX];
  char location[BUFF_SIZE];
  char shortname[_POSIX_PATH_MAX];
  int deleted;
  off_t size;
  time_t date;
};

/*
** -- Functions --
*/

void displayFiles(const char *userid, const int sortby)
{
  DIR *dirp1 = 0;
  DIR *dirp2 = 0;
  FILE *fp = 0;
  char *tmp = 0;
  char buffer[BUFF_SIZE];
  char newdate[BUFF_SIZE];
  char reldir[BUFF_SIZE];
  int adminmode = FALSE;
  int deleted = 0;
  size_t i = 0;
  size_t ct = 0;
  struct dirent *dirent1 = 0;
  struct dirent *dirent2 = 0;
  struct file_info *files = 0;
  struct stat stbuf;
  struct tm *tmstr = 0;

  /*
  ** Create the /tmp/bcksys.loggedin.userid file.
  */

  if((tmp = getenv("REMOTE_ADDR")) == 0)
    tmp = "1";

  (void) snprintf(buffer, sizeof(buffer), "/%s/data/bcksys.loggedin.%s.%s",
		  BACKUP_DIR, userid, tmp);
  (void) fchmod(creat(buffer, (mode_t) (O_CREAT | O_TRUNC)),
		(mode_t) (S_IRUSR | S_IWUSR));

  if(userid && strcmp(userid, "admin") == 0)
    adminmode = TRUE;
  else
    adminmode = FALSE;

  (void) printf("<noscript><blink>JavaScript Required!</blink><br>");
  (void) printf(" | <a href=\"%s/logout_user.cgi?%s\">Logout</a> |<br>\n",
		CGI_DIR, userid ? userid : "user");
  (void) printf("</noscript>\n");

  if(adminmode == FALSE)
    {
      (void) printf("<script>\n");
      (void) printf("function addFile()\n"
		    "{\n"
		    "var w = window.open"
		    "(\"%s/add_file.cgi?%s\","
		    "\"MadDogBackupSystem\","
		    "\"resizable=yes,width=800,height=230,scrollbars=yes\")"
		    ";\n",
		    CGI_DIR, userid ? userid : "user");
      (void) printf("}\n");
      (void) printf("</script>");
      (void) printf("<input type=\"button\" onClick=\"javascript:addFile()\" "
		    "value=\"Add New File\">\n");
    }

  (void) printf("<input type=\"button\" onClick='window.location=\""
		"%s/logout_user.cgi?%s\"' "
		"value=\"Logout\">\n",
		CGI_DIR, userid ? userid : "user");
  (void) printf("<hr>\n");

  if(adminmode == TRUE)
    {
      (void) snprintf(buffer, sizeof(buffer), "%s/data", BACKUP_DIR);

      if((dirp1 = opendir(buffer)) != 0)
	{
	  (void) printf("<center>\n");
	  (void) printf("<table cellpadding=1 cellspacing=0 border=0 "
			"width=\"50%%\" "
			"bgcolor=\"DarkSlateGray\"><tr><td>\n");
	  (void) printf("<a href=\"%s\">%sMad Dog Backup System Home</a> "
			">> User Accounts%s",
			HOME,
			FBEG, FEND);
	  (void) printf("\n<table width=\"100%%\" border=0 "
			"cellpadding=0 cellspacing=1>\n");
	  (void) printf("<tr size=20>\n");
	  (void) printf("<th bgcolor=\"CornflowerBlue\">%sUser Account%s"
			"</th>\n", FBEG, FEND);
	  (void) printf("<th bgcolor=\"CornflowerBlue\">%sPassword "
			"%s</th>\n",
			FBEG, FEND);
	  (void) printf("</tr>\n");

	  while((dirent1 = readdir(dirp1)) != 0)
	    if(!(strcmp(dirent1->d_name, ".") == 0 ||
		 strcmp(dirent1->d_name, "..") == 0))
	      if(strncmp(dirent1->d_name, "passwd", 6) == 0)
		{
		  (void) snprintf(buffer, sizeof(buffer), "%s/data/%s",
				  BACKUP_DIR, dirent1->d_name);

		  if((fp = fopen(buffer, "r")) != 0)
		    {
		      (void) printf("<tr bgcolor=\"SlateGray\">\n");
		      (void) printf("<th>%s%s%s</th>\n",
				    FBEG, strrchr(dirent1->d_name, '.') + 1,
						 FEND);
		      (void) memset(buffer, 0, sizeof(buffer));

		      if(fgets(buffer, (int) sizeof(buffer), fp) != 0)
			(void) printf("<th>%s%s%s</th>\n",
				      FBEG, strrchr(buffer, '=') + 1, FEND);
		      else
			(void) printf("<th>%sUnknown%s</th>\n", FBEG, FEND);

		      (void) printf("</tr>\n");
		      (void) fclose(fp);
		    }
		}

	  (void) printf("</td></tr></table>\n");
	  (void) printf("</table>\n");
	  (void) printf("</center>\n");
	  (void) closedir(dirp1);
	}

      return;
    }

  (void) printf("<form name=\"mainform\" ");
  (void) printf("action=\"%s/delete_files.cgi?%s\" "
		"method=\"post\">", CGI_DIR, userid ? userid : "user");

  /*
  ** Create and populate the table.
  */

  (void) printf("<table cellpadding=1 cellspacing=0 border=0 width=\"100%%\" "
		"bgcolor=\"DarkSlateGray\"><tr><td>\n");
  (void) printf("<a href=\"%s\">%sMad Dog Backup System Home</a> >> Files%s",
		HOME,
		FBEG, FEND);
  (void) printf("\n<table width=\"100%%\" border=0 "
		"cellpadding=0 cellspacing=1>\n");
  (void) printf("<tr size=20>\n");
  (void) printf("<th bgcolor=\"CornflowerBlue\">&nbsp</th>");
  (void) printf("<th bgcolor=\"CornflowerBlue\">"
		"<a href=\"./display_files.cgi"
		"?%s&%d\">%sFile Name%s</a></th>\n",
		userid ? userid : "user", SORTBY_NAME, FBEG, FEND);
  (void) printf("<th bgcolor=\"CornflowerBlue\">"
		"<a href=\"./display_files.cgi"
		"?%s&%d\">%sArchival Date%s</a></th>\n",
		userid ? userid : "user", SORTBY_DATE, FBEG, FEND);
  (void) printf("<th bgcolor=\"CornflowerBlue\">%sDeleted%s</th>\n",
		FBEG, FEND);
  (void) printf("<th bgcolor=\"CornflowerBlue\">"
		"<a href=\"./display_files.cgi"
		"?%s&%d\">%sSize (KB)%s</a></th>\n",
		userid ? userid : "user", SORTBY_SIZE, FBEG, FEND);
  (void) printf("</tr>\n");

  /*
  ** Read all of the files and create tables for them.
  */

  (void) snprintf(buffer, sizeof(buffer), "%s/%s/files", BACKUP_DIR,
		  userid ? userid : "user");

  if((dirp1 = opendir(buffer)) != 0)
    {
      ct = 0;
      dirp2 = 0;

      while((dirent1 = readdir(dirp1)) != 0)
	if(!(strcmp(dirent1->d_name, ".") == 0 ||
	     strcmp(dirent1->d_name, "..") == 0))
	  {
	    (void) snprintf(buffer, sizeof(buffer), "%s/%s/files/%s",
			    BACKUP_DIR, userid ? userid : "user",
			    dirent1->d_name);

	    if((dirp2 = opendir(buffer)) != 0)
	      {
		while((dirent2 = readdir(dirp2)) != 0)
		  if(!(strcmp(dirent2->d_name, ".") == 0 ||
		       strcmp(dirent2->d_name, "..") == 0))
		    ct += 1;

		(void) closedir(dirp2);
		dirp2 = 0;
	      }
	  }

      (void) rewinddir(dirp1);

      if(dirp2 != 0)
	(void) closedir(dirp2);

      dirp2 = 0;

      if(ct == 0)
	{
	  (void) closedir(dirp1);
	  dirp1 = 0;
	  goto done_label;
	}

      if((files = (struct file_info *)
	  malloc((size_t) ct * sizeof(struct file_info))) == 0)
	{
	  (void) closedir(dirp1);
	  dirp1 = 0;
	  goto done_label;
	}

      i = 0;

      while((dirent1 = readdir(dirp1)) != 0)
	if(!(strcmp(dirent1->d_name, ".") == 0 ||
	     strcmp(dirent1->d_name, "..") == 0))
	  {
	    (void) snprintf(buffer, sizeof(buffer), "%s/%s/files/%s",
			    BACKUP_DIR, userid ? userid : "user",
			    dirent1->d_name);

	    if((dirp2 = opendir(buffer)) != 0)
	      {
		while((dirent2 = readdir(dirp2)) != 0)
		  if(!(strcmp(dirent2->d_name, ".") == 0 ||
		       strcmp(dirent2->d_name, "..") == 0))
		    {
		      if(strncmp(dirent1->d_name, "deleted", (size_t) 7) == 0)
			deleted = 1;
		      else
			deleted = 0;

		      files[i].deleted = deleted;
		      (void) memset(reldir, 0, sizeof(reldir));
		      getRel(BACKUP_DIR, reldir, sizeof(reldir));

		      if(strlen(reldir) > 0)
			(void) snprintf(files[i].location,
					sizeof(files[0].location),
					"../../%s/%s/files/%s/%s",
					reldir, userid ? userid : "user",
					dirent1->d_name, dirent2->d_name);
		      else
			(void) snprintf(files[i].location,
					sizeof(files[0].location),
					"../../%s/files/%s/%s",
					userid ? userid : "user",
					dirent1->d_name, dirent2->d_name);

		      if(deleted > 0)
			(void) snprintf(files[i].dirname,
					sizeof(files[0].dirname), "%s",
					dirent2->d_name);
		      else
			(void) snprintf(files[i].dirname,
					sizeof(files[0].dirname), "%s",
					dirent1->d_name);

		      (void) snprintf(files[i].shortname,
				      sizeof(files[0].shortname), "%s",
				      dirent2->d_name);
		      (void) snprintf(buffer,
				      sizeof(buffer),
				      "%s/%s/files/%s/%s",
				      BACKUP_DIR, userid ? userid : "user",
				      dirent1->d_name,
				      dirent2->d_name);

		      if(stat(buffer, &stbuf) != -1)
			{
			  files[i].date = stbuf.st_mtime;
			  files[i].size = stbuf.st_size / 1024;
			}
		      else
			{
			  files[i].date = time((time_t *) 0);
			  files[i].size = 0;
			}

		      i += 1;

		      if(i >= ct)
			break;
		    }

		(void) closedir(dirp2);
	      }

	    if(i >= ct)
	      break;
	  }

      (void) closedir(dirp1);
    }

  /*
  ** Sort the files by the selected attribute.
  */

  if(files != 0)
    switch(sortby)
      {
      case SORTBY_DATE:
	{
	  qsort((void *) files,
		(size_t) ct,
		sizeof(struct file_info),
		date_cmp);
	  break;
	}
      case SORTBY_NAME:
	{
	  qsort((void *) files,
		(size_t) ct,
		sizeof(struct file_info),
		name_cmp);
	  break;
	}
      case SORTBY_SIZE:
	{
	  qsort((void *) files,
		(size_t) ct,
		sizeof(struct file_info),
		size_cmp);
	  break;
	}
      default:
	{
	  break;
	}
      }

  if(files != 0)
    for(i = 0; i < ct; i++)
      {
	if(i % 2 == 0)
	  (void) printf("<tr bgcolor=\"SlateGray\">\n");
	else
	  (void) printf("<tr bgcolor=\"SteelBlue\">\n");

	if(files[i].deleted > 0)
	  (void) printf("<th>&nbsp</th>\n");
	else
	  (void) printf("<th><input type=\"checkbox\" "
			"name=\"cblist\" value=\"%s-%s\">"
			"</th>\n", files[i].dirname, files[i].shortname);

	(void) printf("<th align=left>&nbsp%s%s%s</th>\n", FBEG,
		      files[i].dirname, FEND);

	if((tmstr = localtime(&files[i].date)) != 0)
	  (void) strftime(newdate, sizeof(newdate),
			  "%m/%d/%Y %H:%M:%S", tmstr);
	else
	  (void) snprintf(newdate, sizeof(newdate), "%s", "???");

	(void) printf("<th><a href=\"%s\">%s%s%s</a>"
		      "</th>\n", files[i].location, FBEG,
		      newdate, FEND);

	if(files[i].deleted > 0)
	  (void) printf("<th>%sYES%s</th>\n", FBEG, FEND);
	else
	  (void) printf("<th>&nbsp</th>\n");

	(void) printf("<th align=left>&nbsp%s%lu%s\n",
		      FBEG, (unsigned long) files[i].size, FEND);
	(void) printf("</tr>\n");
      }

 done_label:
  if(files != 0)
    {
      free(files);
      files = 0;
    }

  (void) printf("</td></tr></table>\n");
  (void) printf("</table>\n");

  if(ct > 0)
    {
      (void) printf("<hr>\n");
      (void) printf("| <a href=\"javascript:"
		    "checkBoxSelect(true)\">Select "
		    "All</a> | ");
      (void) printf("<a href=\"javascript:"
		    "checkBoxSelect(false)\">Deselect "
		    "All</a> |\n");
      (void) printf("<input type=\"submit\" value=\"Delete Selected "
		    "File(s)\">\n");
    }

  (void) printf("</form>\n");
}

static void getRel(const char *fulldir, char reldir[], const size_t size)
{
  int found = 0;
  size_t i = 0;
  size_t idx = 0;
  size_t j = 0;

  if(fulldir && strlen(fulldir) > 0)
    for(i = strlen(fulldir) - 1; i == 0 || i > 0; i--)
      {
	if(fulldir[i] == '/')
	  {
	    found = 1;
	    idx = i;
	    break;
	  }

	if(i == 0)
	  break;
      }

  (void) memset(reldir, 0, size);

  if(found && fulldir)
    for(i = idx + 1; i < strlen(fulldir); i++)
      {
	if(j < size)
	  reldir[j++] = fulldir[i];
	else
	  break;
      }
}

static int date_cmp(const void *e1, const void *e2)
{
  if(e1 == 0 || e2 == 0)
    return 0;

  return (int) difftime(((struct file_info *) e1)->date,
			((struct file_info *) e2)->date);
}

static int name_cmp(const void *e1, const void *e2)
{
  if(e1 == 0 || e2 == 0)
    return 0;

  return strcmp(((struct file_info *) e1)->dirname,
		((struct file_info *) e2)->dirname);
}

static int size_cmp(const void *e1, const void *e2)
{
  if(e1 == 0 || e2 == 0)
    return 0;

  return (int) (((struct file_info *) e1)->size -
		((struct file_info *) e2)->size);
}

#endif
