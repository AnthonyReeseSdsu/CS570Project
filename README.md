# CS570Project
This is a basic POSIX based shell which was written as a semester long project for my Operating Systems course. In order to test this code, I recommend changing your default Linux shell to tcsh. The default shell will work as well, so if you are having trouble changing your shell, you can skip the next three commands.


-------------------
chsh
--------------------
/bin/tcsh
---------------------
yourpassword(to confirm)
-------------------------------------

You can check that this worked by typing the following command: 'finger $USER'.

This should display your info, within the info you should see 'Shell: /bin/tcsh'. Once this is done and you have pulled from the repo, type make and the executable will be name 'p2'. Type p2, then press enter and the shell is ready to run. In order to exit 
the shell, press ctrl+c. If you want to see in detail what a certain system call is doing, visit the linux manual page for 
the call, eg. google 'fork manpage'.





