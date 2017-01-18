import time
import serial

mySer = serial.Serial('COM14', 9600)
receivedBuf = ""
count = 0
while 1:
    mydata = raw_input('Prompt :')
    if mydata == "0" or ".":
        while receivedBuf != "chOk":
            MyRealTime = time.asctime()
            mySer.write('C' + '\r')
            bytesToRead = mySer.inWaiting()
            receivedBuf = mySer.read(bytesToRead)
            print "PC Received:" + receivedBuf
            if  receivedBuf == "X":
                print "X Confirmed"
                mySer.write("ch" + MyRealTime + '\r')
                print "PC: Hey ST, Get the Time:" + MyRealTime
                receivedBuf = mySer.readline()
                print receivedBuf
                receivedBuf = ""
                break
            else:
                print "X Not Confirmed"
                receivedBuf = ""

            time.sleep(1)
    else:
        print "Unknown Command"
    time.sleep(1)


