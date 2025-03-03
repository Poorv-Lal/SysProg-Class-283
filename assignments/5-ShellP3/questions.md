1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes are complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

We need to loop through in order to make sure we waitpid() all the child processes we opened. I do this in a for loop that goes from 0 to clist->num. If we did not waitpid() all the children would become zombies and keep taking up resources and space. This is bad. We don't want this at all. 

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

if the pipes remain open the children might not receive the correct signals they need to stop and we would be stuck waiting for input forever. 

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is a built-in command because it affects where you are physically (in a computer directory sense). Once you cd somewhere you want to stay there, that's your new location. all the execvp() commends go off, do their jobs, and then stop when they are finished. Cd's behavior specifies long after you've called it. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Instead of setting aside a set number of memory and then being mad at the user for needing more than the memory provided, we would need to systematically reallocate memory based on specifically how much memory the user's tasks require. The problem with this is that as the user needs more and more memory the computer will have a harder time hopping around and finding suitable locations to store it. 
