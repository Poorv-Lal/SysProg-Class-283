1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  the 'fork' lets us create a child process that is a copy of the parent process and will not mess with the parent. Since the work we are doing is of this new copied child version, if something theoretically goes wrong, it can terminate without affecting the original. It is also better for not messing with the parent's memory. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  When the fork fails, a child is not made. In my code, this sends back an error message and exits with an error return code. anytime something goes wrong with the fork, there is a safeguard in place to let the user know. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in the path environment variable. it executes the first match it finds, and if it does not find a match at all it  returns the propper error precautions to let the user know something went amiss.  

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  if we don't make the parent process wait until the child process is done the computer will get confused about what resources are going where. we pause the parent so the computer can focus on doing the child process and getting that exit status, and then the parent can handle whatever comes next from there. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  it extracts the exit status chode of a child process. this will let us know if the child was a success or a failure, if what steps we need to take based on that information. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  my build_cmd_buff() takes in the line inputted by the user and goes through it one character at a time to determine what behavior it needs to do. if it reaches a quote, it goes into a state where it treats white space as any other acceptable character. once the quote ends, it exits this state. once it hits a space it segments off that part of the word using a '\0' and adds the last segment into the array that stores arguments and continues to the next segment until the entire line is processed.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Since the data structure itself was different, the relevant information needed to be stored differently. also, my assignment last week assumed that there was no whitespace between arguments, it only handled white space at the beginning and end of the line, not in between, so I had to adjust for that. Also since some spaces were relevant and needed to be kept I could not just tokenize everything based on where the spaces fell like I did last week, hence why I ended up going through manually by character. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The signals are there to inform us of the state of the system. For example, if we run into a problem the signal allows us to react to that output in a way that is logical, like terminating the program. this is different from other IPCs because it does not require us to handle it explicitly. it is not exchanging data, but notifying us of what has happened.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:
    > SIGINT - Signal Interrupt. This lets us terminate the process gracefully and it will stop in the foreground and clean itself up and stop. You go, "Stop when you can" and the computer goes "ok give me a sec"
    > SIGTERM - Signal Terminate. this tells the program to kill whatever process it is currently doing and gracefully terminate from there. You go, "Stop now please" and the computer goes "ok ok, I'm closing everything down"
    > SIGKILL - Signal Kill. this is a forceful termination. the computer does not have the chance to clean anything up, everything just immediately ends. You go, "OMG STOP RIGHT NOW" and the computer goes "AHHHHHH" and stops everything. it's very dramatic, and probably not ideal. 

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP - Signal Stop. this will pause the program until it's told to continue. it cannot be ignored or caught because it's there so we can consistently and reliably pause something while it's happening to maintain control over the system. You go, "Freeze!" and the computer goes "o---" and pauses. It's like freeze tag :D
