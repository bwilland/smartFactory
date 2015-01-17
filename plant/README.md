# Smart Plant Lightning System


## File structure
* web: including web pages/database/main executable/configuration for boa/upgrading scripts/logs
* web/www: web pages
* web/boa.conf: configuration for boa server.
* web/log/ErrorLog: log file
* server: server files
* server/mlinux.sh: script to compile source code in Linux, will generate makefile, and you can use make commands afterwards.
* server/marm.sh: script to compile source code in arm system.
* server/boa: boa webserver
* server/actions: controller actions for handling web page requests
* server/shared: data structure / Data Access Object / zigbee commands/programe for serial ports/ shared memory (for multi-threading)
* server/thread: schecule scripts
* server/plant.sql: database schema


## Environments
* development: eclipse C/C++ IDE  sqlite3 libsqlite3-dev
* compiling:  gcc(linux)  arm-linux-gnueabihf-gcc(arm)   byacc,flex
* running: web brower --pcduino -- usb --serialport-- zigbee 


## installation
```
sudo apt-get install sqlite3 libsqlite3-dev byacc flex
sudo ln -s /plant web       # /plant folder is used as the working directory, which is linked to web folder
cd web
cp plant.db.bak plant.db    # create default db
cd ../server
sh ./mlinux.sh              # compile and generate executable file: smartplant
sudo ./smartplant           # start server
xdg-open http://localhost   # add port if you have different port set in web/boa.conf
```