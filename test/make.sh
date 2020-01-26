# Start
# hercules -f make.conf -d 2>/dev/null 2>/dev/null & 

home="."

${home}/HercControl "ipl 141" -w ""

${home}/HercControl "" -w "USER DSC LOGOFF AS AUTOLOG1"

${home}/HercControl "/enable all" -w "COMMAND COMPLETE"

${home}/HercControl "/cp disc" -w "^VM/370 Online"

${home}/HercControl "/logon cmsuser cmsuser" -w "^CMS VERSION"

${home}/HercControl "/" -w "^Ready"

${home}/HercControl "/listf * * a" -w "^Ready"

${home}/HercControl "/logoff" -w "^VM/370 Online"

${home}/HercControl "/logon operator operator" -w "RECONNECTED AT"

# Done
${home}/HercControl "/shutdown" -w "^HHCCP011I"
# ${home}/HercControl "exit" -w "" 