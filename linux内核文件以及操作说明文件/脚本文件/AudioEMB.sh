#!/bin/sh
ps -fe|grep monitor |grep -v grep
if [ $? -ne 0 ];then
echo "start monitor process.....">>/opt/shell_info.txt
	nohup /opt/monitor >>/opt/myout.file 2>&1 &
else

echo "monitor is running.....">>/opt/shell_info.txt

fi

ps -fe|grep repeater |grep -v grep
if [ $? -ne 0 ];then
echo "start repeater process.....">>/opt/shell_info.txt
	nohup /opt/repeater >>/opt/myout.file 2>&1  &
else

echo "repeater is running.....">>/opt/shell_info.txt

fi


ps -fe|grep update |grep -v grep
if [ $? -ne 0 ];then
echo "start update process.....">>/opt/shell_info.txt
        nohup /opt/update >>/opt/myout.file 2>&1  &
else

echo "update is running.....">>/opt/shell_info.txt

fi



