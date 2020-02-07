# HercControl
Tool to allow Batch Control of Hercules mainframe emulator

The current Hercules toolset does not *seem* to have a tool to execute a command on the console and wait for a specific reply.

This tool fills this gap. It works by connecting connecting to the HTTP console facility pushing commands and polling the console. It used comment "markers" in the Hercules console so that it does not need to get the full console history as it polls.

# Instructions

    Usage: herccontrol [OPTIONS] [command]

    Positionals:
      command TEXT                The command to send to Hercules

    Options:
      -h,--help                   Print this help message and exit
      -w,--waitfor TEXT           The regex string we are waiting for
      -m,--mark                   Set and output mark point (to be used in herccontrol -f)
      -f,--frommark TEXT          Search log from mark point (returned from herccontrol -m)
      -d,--debug (Env:HC_DEBUG)   Debug mode
      -u,--url TEXT (Env:HC_HOSTURL)
                                  Host URL - default is 127.0.0.1:8038
      -v,--version                Show version and exit
      -t,--timeout INT (Env:HC_TIMEOUT)
                                  Timeout/sec - default is 30
      --starthistorysize INT      Start History Size/lines (advanced) - default is 10
      --pollingtime INT           Polling Time/ms (advanced) - default is 250
      --maxconsolesize INT        Max Console Size/lines (advanced) - default is 20


# TODO
Use Regex waitfor string to extract the host command return code
