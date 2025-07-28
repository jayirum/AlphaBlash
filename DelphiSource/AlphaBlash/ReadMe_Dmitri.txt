You know, no hardcoded structure.
A logic is a following :
In the settings.ini (which in the same folder where app exe file loacted) file we have a host name definition :

[General]
HostName = http://project2020.fun

First thing : application try to download file by address :
http://project2020.fun/version.ini

No file - no update.

If file exists program compare his own version number with version number at the version.ini file.

if version in the version.ini file is a bigger than his own version it start to parse file and try to find these 3 sections "

[EXPERTS]
[FILES]
[LIBRARIES]

each section has a FilesNumber = N riow
also N rows with a filenames.

All of files (including version.ini) must be located in the root folder of domain.


phpinfo()
http://project2020.fun

http://project2020.fun/version.ini
http://project2020.fun
It seems like I didn't delete files from server, so you can take a look how it works.

오후 9:10
Thanks. I will look into your explanation.
By the way, I found "Update.bat" in the codes, Do I need this file as well?

Delphi, 오후 9:11
No, it's created by exe file

오후 9:11
Oh I see.

Delphi, 오후 9:12
When exe file retrieved files from server it creates update.bat, run it and exit from itself

오후 9:12
I will dig into the codes deeply. Thanks Dmitri.

Delphi, 오후 9:12
Update.bat copy files and run exe again