#!../../bin/linux-arm/GPD4303Stest

## You may have to change GPD4303Stest to something else
## everywhere it appears in this file
< envPaths

epicsEnvSet "STREAM_PROTOCOL_PATH" "$(TOP)/db"

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/GPD4303Stest.dbd"
GPD4303Stest_registerRecordDeviceDriver pdbbase

# drvAsynSerialPortConfigure port ipInfo priority noAutoconnect noProcessEos 
drvAsynSerialPortConfigure("L0","/dev/ttyUSB0",0,0,0) 
asynSetOption("L0", 0, "baud", "9600") 
asynSetOption("L0", 0, "bits", "8") 
asynSetOption("L0", 0, "parity", "none") 
asynSetOption("L0", 0, "stop", "1") 
asynSetOption("L0", 0, "clocal", "N") 
asynSetOption("L0", 0, "crtscts", "N") 
asynOctetSetInputEos("L0", 0, "\r\n") 
asynOctetSetOutputEos("L0", 0, "\r\n") 
#asynSetTraceIOMask("L0",0,0x18) 
#asynSetTraceMask("L0",0,0x18) 
#asynSetTraceIOMask("L0",0,0x2) 
#asynSetTraceMask("L0",0,0x9) 

## defines HOSTNAME variable
epicsEnvSet "HOSTNAME" "USOPFWD0"

epicsEnvSet("VSET_LABEL","VSET")
epicsEnvSet("VOUT_LABEL","VOLTAGE")
epicsEnvSet("ISET_LABEL","ISET")
epicsEnvSet("IOUT_LABEL","CURRENT")

#dbLoadRecords("db/devGPD4303S.db","PORT=L0,P=$(HOSTNAME):GPD4303S")
dbLoadRecords("db/devGPD4303S.db","PORT=L0,P=BEAST:$(HOSTNAME):PS, C=1, VSET_LABEL=$(VSET_LABEL), VOUT_LABEL=$(VOUT_LABEL), ISET_LABEL=$(ISET_LABEL), IOUT_LABEL=$(IOUT_LABEL)")
dbLoadRecords("db/devGPD4303S.db","PORT=L0,P=BEAST:$(HOSTNAME):PS, C=2, VSET_LABEL=$(VSET_LABEL), VOUT_LABEL=$(VOUT_LABEL), ISET_LABEL=$(ISET_LABEL), IOUT_LABEL=$(IOUT_LABEL)")
dbLoadRecords("db/devGPD4303S.db","PORT=L0,P=BEAST:$(HOSTNAME):PS, C=3, VSET_LABEL=$(VSET_LABEL), VOUT_LABEL=$(VOUT_LABEL), ISET_LABEL=$(ISET_LABEL), IOUT_LABEL=$(IOUT_LABEL)")
dbLoadRecords("db/devGPD4303S.db","PORT=L0,P=BEAST:$(HOSTNAME):PS, C=4, VSET_LABEL=$(VSET_LABEL), VOUT_LABEL=$(VOUT_LABEL), ISET_LABEL=$(ISET_LABEL), IOUT_LABEL=$(IOUT_LABEL)")

cd "${TOP}/iocBoot/${IOC}"

# enable very verbose asyn debug information
#var streamDebug 1

iocInit
