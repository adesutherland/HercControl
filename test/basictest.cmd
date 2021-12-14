.\herccontrol "ipl 141" -w "USER DSC LOGOFF AS AUTOLOG1"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/enable all" -w "COMMAND COMPLETE"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/cp disc" -w "^VM/370 Online"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/logon cmsuser cmsuser" -w "^CMS"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/" -w "^Ready"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/listf * * a" -w "^Ready"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/logoff" -w "^VM/370 Online"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/logon operator operator" -w "RECONNECTED AT"
if %errorlevel% NEQ 0 goto:eof
.\herccontrol "/shutdown" -w "^HHCCP011I"
