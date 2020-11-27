#! /bin/bash
gcc -g test.c cmd_tool_server.c -lpthread -o test
gcc -g cmd_tool.c -o cmd_tool
#ln -s /home/runoob/work/my_project/cmd_tool/cmd_tool /bin/getIp
#ln -s /home/runoob/work/my_project/cmd_tool/cmd_tool /bin/getSoftInfo
#ln -s /home/runoob/work/my_project/cmd_tool/cmd_tool /bin/setIp
