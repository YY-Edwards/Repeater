#!/bin/sh
ps -fe|grep repeater |grep -v grep
if [ $? -ne 0 ];then
	echo "repeaeter does not exist">>/opt/shell_info.txt
else
	PROCESS=`ps -ef|grep repeater |grep -v grep|grep -v PPID|awk '{ print $1}'`
	echo $PROCESS
	for i in $PROCESS
	do
  		echo "Kill the repeater process [ $i ]"
		kill  $i
	done
	
	echo "repeaeter is killed">>/opt/shell_info.txt
	
fi


ps -fe|grep monitor |grep -v grep
if [ $? -ne 0 ];then
	echo "monitor does not exist">>/opt/shell_info.txt
else
	PROCESS=`ps -ef|grep monitor |grep -v grep|grep -v PPID|awk '{ print $1}'`
	echo $PROCESS
	for i in $PROCESS
	do
  		echo "Kill the monitor process [ $i ]"
		kill -s 9 $i
	done
	
	echo "monitor is killed">>/opt/shell_info.txt
	
fi

