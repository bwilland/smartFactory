CREATE TABLE NODES(MAC TEXT  NOT NULL,MEMO TEXT  NOT NULL,AREA INTEGER,TYPE INTEGER, MAXSPEED FLOAT, MAXDISTANCE FLOAT,PULSES FLOAT,ISVALID INTEGER);
CREATE TABLE CALIBERATE(NODEID INTEGER,CHANNEL INTEGER, LEDS INTEGER, PWM FLOAT,CURRENTMA FLOAT);

CREATE TABLE AREA(NAME TEXT  NOT NULL,MEMO TEXT  NOT NULL,KR INTEGER,KB INTEGER,ORDERID INTEGER,NODECOUNT INTEGER);
CREATE INDEX IF NOT EXISTS IDX_ORDERID ON AREA(ORDERID);

CREATE TABLE MODEL(NAME TEXT  NOT NULL,TYPE INTEGER,PPF1 FLOAT,PPF2 FLOAT,PPF3 FLOAT,PPF4 FLOAT,MOTORMODE INTEGER,DIRECTION INTEGER,SPEED FLOAT,DISTANCE FLOAT);

CREATE TABLE STATICMODEL(NAME TEXT  NOT NULL,TYPE INTEGER,PPF1 FLOAT,PPF2 FLOAT,PPF3 FLOAT,PPF4 FLOAT,MOTORMODE INTEGER,DIRECTION INTEGER,SPEED FLOAT,DISTANCE FLOAT);

CREATE TABLE SCHEDULE(STARTTIME INTEGER,ENDTIME INTEGER,DESCRIPTION TEXT,STATUS INTEGER,AREALIST TEXT,MODIFYTIME INTEGER,MAXSQUENCE INTEGER,CURRENTSQU INTEGER,CURRSQULEFTTIME INTEGER);
CREATE TABLE TASK(SCHEDULEID INTEGER,SEQUENECE INTEGER, MODELID INTEGER,DURING INTEGER);

CREATE TABLE TASKMODEL(TASKID INTEGER,SEQUENECE INTEGER  NOT NULL,MODELID INTEGER,MODELNAME TEXT,DURING INTEGER);

CREATE TABLE COMMLOG(DATETIME INTEGER  NOT NULL,ENDTIME INTEGER  NOT NULL,MODELID INTEGER,MODELNAME TEXT, AREAID INTEGER,AREANAME TEXT,PPF1 FLOAT,PPF2 FLOAT,PPF3 FLOAT,PPF4 FLOAT,OPTIONTYPE INTEGER,TIMESTOPED INTEGER);

CREATE TABLE TIMESPAN(Span INTEGER);
CREATE TABLE SENDSETTING(SENDTIMES INTEGER,BROADCAST INTEGER,SENDCONFIRM INTEGER);


CREATE TABLE USERS(USERNAME TEXT  NOT NULL,PASSWD TEXT  NOT NULL,ROLE INTEGER);
CREATE TABLE SERVERNAME(SERVERNAME text NOT NULL);
INSERT INTO SERVERNAME values("Test Server");

INSERT INTO USERS values('admin','admin',2);

CREATE TABLE BACKDB(DATETIME TEXT  NOT NULL,DBNAME TEXT);


INSERT INTO TIMESPAN VALUES(1);
INSERT INTO SENDSETTING VALUES(1,0,0);

INSERT INTO "MODEL" VALUES('CloseAll',0,0,0,0,0,0,0,0,0);
INSERT INTO "MODEL" VALUES('WatchMode',0,10,10,10,10,0,0,0,0);



