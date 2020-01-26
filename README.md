# HercControl
Tool to allow Batch Control of Hercules mainframe emulator

The current Hercules toolset does not *seem* to have a tool to execute a command on the console and wait for a specific reply.

This tool fills this gap. It works by connecting connecting to the HTTP console facility pushing commands and polling the console.

# Instructions

Usage: HercControl [OPTIONS] [command]

Positionals:
  command TEXT                The command to send to Hercules

Options:
  -h,--help                   Print this help message and exit
  -w,--waitfor TEXT           The regex string we are waiting for
  -d,--debug                  Debug mode
  -u,--url TEXT               host URL - default is 127.0.0.1:8038
  -v,--version                Show version and exit
