#/bin/sh
kill -s 9 $(ps -Aef|grep smartfactory|gawk '$0 !~/grep/ {print $2}' |tr -s '\n' ' ')
ipcrm -m $(ipcs -m|grep 30720|gawk '$0 !~/grep/ {print $2}' |tr -s '\n' ' ')
