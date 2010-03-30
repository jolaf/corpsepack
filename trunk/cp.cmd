@echo off
javac -g -deprecation -source 1.3 -target 1.3 CorpsePack.java | more 2>&1 
if %ERRORLEVEL% NEQ 0 goto end
java CorpsePack 2>&1
:end
