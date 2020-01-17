# Start
# hercules -f make.conf -d 2>/dev/null 2>/dev/null & 

home="../build"

${home}/HercControl "ipl 141" ""

${home}/HercControl "" "USER DSC LOGOFF AS AUTOLOG1"

${home}/HercControl "/enable all" "COMMAND COMPLETE"

${home}/HercControl "/cp disc" "^VM/370 Online"

${home}/HercControl "/logon cmsuser cmsuser" "^CMS VERSION"

${home}/HercControl "/" "^Ready"

${home}/HercControl "/listf * * a" "^Ready"

${home}/HercControl "/logoff" "^VM/370 Online"

${home}/HercControl "/logon operator operator" "RECONNECTED AT"

# Done
${home}/HercControl "/shutdown" "^HHCCP011I"
${home}/HercControl "exit" "" 