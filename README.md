# CS570Project
This is a basic POSIX based shell which was written as a semester long project for my Operating Systems course. In order to test this code, please change the default linux shell to tcsh using the following commands:

chsh

/bin/tcsh

yourpassword



You can check that this worked by typing typing the following command:

finger $USER

This should display your info, within the info you should see 'Shell: /bin/tcsh'.

After this is done, please make a directory name p2 and then pull from this repo. Type make and then the executable will be named p2. I recommend making various output files named your.output* (replacing * with however many number of outputs you would like to try). Once this is done, you cant test outputs using this command:

p2 < /path/your.output* 


If you wish to use the shell normally, type p2 and begin working, to exit simply press CTRL+C . 
