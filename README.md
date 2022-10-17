<h1>
Simple http async server, build on boost asio and boost beast
</h1>

<h2>
General Description
</h2>

1) To pull all dependencies do - cd conan && conan install conanfile.txt --build=missing
2) Log file create near binary file
3) Correct user turn off by sending SIGTERM
<h2>
Description common_config.json
</h2>

Option  | Description
--------------------------------|---------------------------------
time_to_flush_logs_s            | Period for flush logs
time_to_backup_s                | Period for making dump/backup
directory_to_dump               | Specified directory for dumping
network.host                    | Ip address in which server works
network.port                    | Port in which server works
network.time_socket_expires_s   | Period in which socket expire
logging.file_logger             | Name of file logger
logging.console_logger          | ame of console logger

