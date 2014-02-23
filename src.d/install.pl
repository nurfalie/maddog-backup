#!/usr/bin/perl -W
# Alexis Megas (c) 2004.

use POSIX;
use File::Path;
use File::stat;

# Macros.

$EXIT_SUCCESS = 0;
$EXIT_FAILURE = 1;

# Script must be run as root.

if(geteuid() != 0)
{
    print("This installation program must be run with special " .
	  "privileges, i.e., as root.\n");
    exit($EXIT_FAILURE);
}

# Warn the user.

print("********************************************************" .
      "********************\n");
print("WARNING: Interrupting this process at any step will not undo " .
      "previous steps!\n");
print("********************************************************" .
      "********************\n");

# Specify the directory where this installation should run.

$val = $rundir = getcwd();

do
{
    printf("Which directory contains Mad Dog Backup System's source? [%s] ",
	   $rundir);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $rundir;
    }

    if(! -d $val)
    {
	printf("\"%s\" doesn't appear to exist or is an invalid " .
	       "directory.\n", $val);
    }
}
while(! -d $val);

if(!chdir($rundir = $val))
{
    printf("Unable to chdir() to %s. Exiting.\n", $rundir);
    exit($EXIT_FAILURE);
}

# Determine where the cgi files will reside.

$val = $cgidir = "/usr/lib/cgi-bin/maddog-backup.d";

do
{
    printf("Where would you like the cgi files to reside? [%s] ", $cgidir);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $cgidir;
    }

    if(! -d $val)
    {
	printf("\"%s\" doesn't appear to exist or is an invalid " .
	       "directory.\n", $val);
    }
}
while(! -d $val);

$cgidir = $val;

# Determine the backup directory.

$val = $backupdir = "/var/www/maddog-backup.d";

do
{
    printf("What is the backup directory? [%s] ", $backupdir);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $backupdir;
    }

    if(! -d $val)
    {
	printf("\"%s\" doesn't appear to exist or is an invalid " .
	       "directory.\n", $val);
    }
}
while(! -d $val);

$backupdir = $val;

# Determine the owner and the group.

$val = $usernam = "www-data";

do
{
    printf("What user does Apache run under? [%s] ", $usernam);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $usernam;
    }

    if(!getpwnam($val))
    {
	printf("\"%s\" does not appear to be a valid user on " .
	       "this system.\n", $val);
    }
}
while(!getpwnam($val));

$usernam = $val;

$val = $groupnam = "www-data";

do
{
    printf("What group should the cgi files belong to? [%s] ", $groupnam);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $groupnam;
    }

    if(!getgrnam($val))
    {
	printf("\"%s\" does not appear to be a valid group on " .
	       "this system.\n", $val);
    }
}
while(!getgrnam($val));

$groupnam = $val;

# Create the $cgidir directory, if it doesn't exist.

if(! -d $cgidir)
{
    if(!mkdir($cgidir, 0777))
    {
	printf("Unable to create %s. Exiting.\n", $cgidir);
	exit($EXIT_FAILURE);
    }
    else
    {
	printf("%d was created successfully.\n", $cgidir);
    }
}

@dirs = ($cgidir, $backupdir);

foreach $dir (@dirs)
{
    # Check ownership.

    if(!($info = stat($dir)))
    {
	printf("Unable to stat() %s.\n", $dir);
    }
    elsif((getpwnam($usernam))[2] != $info->uid ||
	  (getgrnam($groupnam))[2] != $info->gid)
    {
	$val = "N";

	do
	{
	    printf("Change %s's owner and group to %s and %s, " .
		   "respectively? [N/y] ", $dir, $usernam, $groupnam);
	    chop($val = <STDIN>);
	    
	    if(!$val)
	      {
		$val = "N";
	      }
	  }
	while(!($val eq "N" || $val eq "y"));

	# Set $dir's ownership, but only if the user requests it.

	if($val eq "y")
	{
	    if(!chown((getpwnam($usernam))[2],
		      (getgrnam($groupnam))[2], $dir))
	    {
		printf("Unable to chown() %s.\n", $dir);
	    }
	    else
	    {
		printf("Changed permissions for %s.\n", $dir);
	    }
	}
    }
    else
    {
	printf("%s appears to have the correct permissions. Skipping.\n",
	       $dir);
    }
}

# Determine the cgi URL.

$cgiurl = "https://192.168.178.1/cgi-bin/maddog-backup.d";

do
{
    printf("What is the full cgi URL of your Mad Dog Backup System? [%s] ",
	   $cgiurl);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $cgiurl;
    }
}
while(!$val);

$cgiurl = $val;

# Determine the home URL.

$homeurl = "https://192.168.178.1/maddog-backup.d";

do
{
    printf("What is the full home URL of your Mad Dog Backup System? [%s] ", 
	   $homeurl);
    chop($val = <STDIN>);

    if(!$val)
    {
	$val = $homeurl;
    }
}
while(!$val);

$homeurl = $val;

# Makefile.tmp string.

$makefile_tmp = "INSTALL_OPS = -o " . $usernam . " -g " . $groupnam . "\n" .
    "CGI_PATH = " . $cgidir . "\n" .
    "LDEFINES = -DBACKUP_DIR='\"" . $backupdir . "\"'\\\n" .
    "-DCGI_DIR='\"" . $cgiurl . "\"'\\\n" .
    "-DHOME='\"" . $homeurl . "\"'\n" .
    "\n" .
    "install:\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) add_file \$(CGI_PATH)/add_file.cgi\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) logout_user \$(CGI_PATH)/" .
    "logout_user.cgi\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) lookup_user \$(CGI_PATH)/" .
    "lookup_user.cgi\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) delete_files \$(CGI_PATH)/" .
    "delete_files.cgi\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) display_files \$(CGI_PATH)/" .
    "display_files.cgi\n" .
    "\t\$(INSTALL) \$(INSTALL_OPS) upload_script.pl \$(CGI_PATH)\n";

# It's OK if the file is owned by the privileged user.

if(!open(OUTPUT, "+> " . $rundir . "/Makefile.tmp"))
{
    printf("Unable to open() %s. Exiting.\n", $rundir . "/Makefile.tmp");
    exit($EXIT_FAILURE);
}

print OUTPUT $makefile_tmp;

close(OUTPUT);

# Create the password directory.

if(-d $backupdir . "/data")
{
    printf("%s already exists. Skipping.\n", $backupdir . "/data");
}
elsif(!mkdir($backupdir . "/data", 0777))
{
    printf("Unable to create %s. Exiting.\n", $backupdir . "/data");
}
else
{
    printf("%s created successfully.\n", $backupdir . "/data");
}

if(!chown((getpwnam($usernam))[2],
	  (getgrnam($groupnam))[2], $backupdir . "/data"))
{
    printf("Unable to chown() %s.\n", $backupdir . "/data");
}
else
{
    printf("Changed permissions for %s.\n", $backupdir . "/data");
}

# Create the users' directories.

if(!open(INPUT, "< " . $rundir . "/USERS.dat"))
{
    printf("Unable to open() %s. Exiting.\n", $rundir . "/USERS.dat");
    exit($EXIT_FAILURE);
}

$found = 0;

while(($str = <INPUT>))
{
    if($found > 0)
    {
	if($str =~ /\}\;/)
	{
	    $found = 0;
	}
	else
	{
	    $userid = $str;
	    $userid =~ s/^\s+"//;
	    $userid =~ s/",\s+$//;
	    $userid =~ s/"$//;
	    $userid =~ s/\n//g;

	    if($userid)
	    {
		$userdir = $backupdir . "/" . $userid . "/files";

		if(-d $userdir)
		{
		    printf("%s exists. Skipping.\n", $userdir);
		}
		elsif(!mkpath($userdir, 0777))
		{
		    printf("Unable to create %s.\n", $userdir);
		}

		if(!($info = stat($userdir)))
		{
		    printf("Unable to stat() %s.\n", $userdir);
		}
		elsif((getpwnam($usernam))[2] != $info->uid ||
		      (getgrnam($groupnam))[2] != $info->gid)
		{
		    if(!chown((getpwnam($usernam))[2],
			      (getgrnam($groupnam))[2], $userdir))
		    {
			printf("Unable to chown() %s.\n", $userdir);
		    }
		    else
		    {
			printf("Changed permissions for %s.\n", $userdir);
		    }
		}
		else
		{
		    printf("%s appears to have the correct permissions. " .
			   "Skipping.\n", $userdir);
		}
	    }
	}
    }
    elsif($str =~ /users/)
    {
	$found = 1;
    }
}

close(INPUT);
