#! /bin/bash
g++ -g test.c wrap.cpp -lpthread -Wl,--wrap,malloc -Wl,--wrap,free -o test