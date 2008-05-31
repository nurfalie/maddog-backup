#!/usr/bin/perl -w

print("Content-type: text/html\n\n");
print("<html><title>Mad Dog Backup System</title>\n");
print("<body>\n");

use CGI;
use CGI::Upload;
use POSIX;

my $error = 0;
my $datetime = strftime("%Y%m%d%H%M%S", localtime);
my $cgi = CGI::new;
my $upload = CGI::Upload->new;
my $userid = $cgi->param("userid");
my $where_to = $cgi->param("files");
my $file_name = $upload->file_name("uploaded");
my $upload_dir = "/scsi/www/backup_system/$userid/files";

# Remove special characters from the filename.

if($file_name)
  {
    $file_name =~ tr/-=+!@#$%^&*()~`|\\{[}]:;"'<,>?\/ //d;

    if($where_to eq "New File")
      {
	mkdir("$upload_dir/$file_name.d", 0770);

	if(!chdir("$upload_dir/$file_name.d"))
	  {
	    $error = 1;
	    print("<center>Unable to upload your file." .
		  "<hr>" .
		  "| <a href=\"javascript:history.go(-1)\">Back</a> | " .
		  "<a href=\"javascript:window.close()\">Close</a> |\n" .
		  "</center>");
	    return;
	  }
      }
    else
      {
	if(!chdir("$upload_dir/$where_to"))
	  {
	    $error = 1;
	    print("<center>Unable to upload your file." .
		  "<hr>" .
		  "| <a href=\"javascript:history.go(-1)\">Back</a> | " .
		  "<a href=\"javascript:window.close()\">Close</a> |\n" .
		  "</center>");
	    return;
	  }
      }

    my $SIZE = 7;
    my $file_size = 0;
    my $file_handle = $upload->file_handle("uploaded");
    open(UPLOADFILE, ">" . $datetime . $file_name);

    while(<$file_handle>)
      {
	print(UPLOADFILE);
      }

    close(UPLOADFILE);

    # If the file's size is not greater than 0 bytes, remove it and notify
    # the user to upload a valid file.

    if((stat($datetime . $file_name))[$SIZE] > 0)
      {
	print("<center>Your file has been uploaded successfully." .
	      "<hr>" .
	      "| <a href=\"javascript:window.close()\">Close</a> |\n" .
	      "</center>");

	# Zip the file.

	system("zip " .
	       $datetime . $file_name . ".zip " . $datetime . $file_name);
	unlink($datetime . $file_name);
      }
    else
      {
	$error = 1;
	unlink($datetime . $file_name);

	if($where_to eq "New File")
	  {
	    rmdir("$upload_dir/$file_name.d");
	  }

	print("<center>Please upload a non-empty file." .
	      "<hr>" .
	      "| <a href=\"javascript:history.go(-1)\">Back</a> | " .
	      "<a href=\"javascript:window.close()\">Close</a> |\n" .
	      "</center>");
      }
  }
else
  {
    $error = 1;
    print("<center>Unable to upload your file." .
	  "<hr>" .
	  "| <a href=\"javascript:history.go(-1)\">Back</a> | " .
	  "<a href=\"javascript:window.close()\">Close</a> |\n" .
	  "</center>");
  }

END_LABEL:

print("</body>\n");
print("</html>\n");

if($error == 0)
  {
    print("<script>" .
	  "if(this != null && this.window != null && this.window.opener != " .
	  "null && " .
	  "   this.window.opener.window != null && " .
	  "    this.window.opener.window.location != null) " .
	  "this.window.opener.window.location.reload();" .
	  "</script>");
  }
