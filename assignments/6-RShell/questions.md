1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

We need to set aside a specific marker to indicate when the message has been fully transmitted. this way the computer will continue to wait as the user types until it sees the marker, and then it can take all of what the user typed as an input. 

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

using the marker to identify the beginning and end of a command. issues that arise are that it may be difficult to identify the boundaries between the makers and all the components that belong to one command. the computer could also get stuck in a state where it is forever waiting for the end of the command marker. 

3. Describe the general differences between stateful and stateless protocols.

a stateful protocol is when the server maintains information about the current state the client is in a Stateless protocol is when each request is taken independently and previous request information is not stored. the pros of a stateful protocol are that it lets the commands be more complicated, as it can "remember" what was asked of it previously; the cons are that maintaining the data efficiently is difficult. The pros of a stateless protocol are that it is similar to execute, self-contained, and requires less data storage to operate; the cons are that if you want to do more complicated things, you need to input more commands at once as a user since the computer won't "remember" what you did last.  

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDPs may be useful in situations when you need to utilize speed over accuracy, for example, something that is happening in real-time, if you lose the data for one second, that won't affect the overall experience too much, becuase it will be updated data in the next second. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

the abstraction that operating systems enable is the process of creating sockets, binding the addresses, listening for connections, connecting to the remote hosts, sending and receiving data, and then finally closing the connection after it's completed. 
