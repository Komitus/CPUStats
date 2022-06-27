# CPUStats
Multi threaded console application for printing cpu usage

## Compilation
``` make ```

## Tests
``` make test ```

# My thoughts:
### I have garbage in commits bcs I have been changing approach every time I have read smth in details (more on that later). 
<ol>
<li> I have tested program in terms of memory leaks and I have none, even if i exit with SIGINT or SIGTERM.
I had also tested this in this way that I have been changing time of sleeps  (should I have globals for this for real tests?) and watchdog has been doing it's job.
<li> I know logger should be implemented first, I have made it at the end just for checking if buffer really works (yes, it is, I've seen many pushes from the reader in a row).
<li> If global variables are ok? I know they are in data segment of program therefore there are available for the entire time of program execution.
This variables are in fact necessery for the entire life of program (i mean ringbuffers).
I have read thats bad idea if main thread have variables on stack and I give other threads pointers to them (thats why I switched to global).
But global variables stopped me from doing it in SOLID principle (every relays on this globals and init_data func).
Is singleton approach implemented this way ok: https://github.com/Kukos/OOP-in-C/blob/master/framework/inc/oop_singleton.h  ?
<li> If atomic_bools are ok for stopping threads. I had previos version using pthread_cancel. I have heard that's bad but in my case it could be ok, bcs I have many cancellation points in functions given to threads.
According to cancelling threads. I don't have in the final approach push_clean_up_handler bcs I free memory after exiting loop, and if I had to do this handler, I had to have many struct of pointers to variables local to stack or make this variables global to bee seen from this handlers.
<li> I know I have bad Makefile approach. Should I have separate folders for every module and separete Makefile in it?
Smth like here: https://github.com/Kukos/MyLibs But there is no separate obj folder and every folder have only one source file, and each folder makes up a library, not executable.
<li> Previous implementations of watchdogs (but very accurate) was taking too much CPU resources so I switched to poor version of doing sleep in watchdog thread.
</ol>
