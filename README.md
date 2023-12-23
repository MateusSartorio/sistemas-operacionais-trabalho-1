# acsh-shell
A shell named acsh (anti-crowd shell), implemented in C, that putsi in practice the principles of process manipulation. Upon initiation, acsh displays its prompt (symbols at the beginning of each line indicating that the shell is awaiting commands). When it receives a user command line, its processing begins. First, the line must be interpreted in terms of the command language defined below, and each identified command must be executed. This operation may lead to the launching of new processes.
A key feature of acsh is that, in the command line, the user can request the creation of one or more processes:

```bash
acsh> command1 <3 command2 <3 command3
```

However, unlike traditional UNIX shells, when processing a command or a set of commands, it creates the process(es) to execute the respective command(s) in a separate session from the acsh session. This is precisely to avoid process crowds... only small groups within each session! Thus, in the example given above, acsh creates 3 processes – P1, P2, and P3 – to execute the commands command1, command2, and command3 respectively. And these three processes will be be "siblings" and belong to the same session, which is different from the acsh session. The number of commands passed in a single command line can range from 1 to 5... remember... no crowds! Thus, in the same session, there can only be processes that were created during the same command line.
