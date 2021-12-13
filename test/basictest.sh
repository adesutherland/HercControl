#!/bin/bash
set -e
./herccontrol "ipl 141" -w ""
./herccontrol "" -w "USER DSC LOGOFF AS AUTOLOG1"
./herccontrol "/enable all" -w "COMMAND COMPLETE"
./herccontrol "/cp disc" -w "^VM/370 Online"
./herccontrol "/logon cmsuser cmsuser" -w "^CMS"
./herccontrol "/" -w "^Ready"
./herccontrol "/listf * * a" -w "^Ready"
./herccontrol "/logoff" -w "^VM/370 Online"
./herccontrol "/logon operator operator" -w "RECONNECTED AT"
./herccontrol "/shutdown" -w "^HHCCP011I"
