#You need to enable libraries - "dplyr", "xts"
dataSource<-read.table("resultMonitoring.txt", header=TRUE)
dataTemp<-group_by(dataSource,Sec)
dataDrain<-summarize(dataTemp,volFrame=sum(Len),countFrame=n(),timeFrame=1/n(),meanFrame=mean(Len), sdFrame=sd(Len))
#Creating time series
time<-as.POSIXlt(dataDrain$Sec,origin="1970-01-01")
ts_volFrame<-xts(dataDrain$volFrame, time)
ts_countFrame<-xts(dataDrain$countFrame, time)
ts_timeFrame<-xts(dataDrain$timeFrame, time)
ts_meanFrame<-xts(dataDrain$meanFrame, time)
ts_sdFrame<-xts(dataDrain$sdFrame, time)
#Building time series graphs
plot(ts_volFrame,main="Amount of the received data, byte/s",type="o",col="blue")
plot(ts_countFrame,main="Volume of frames received, 1/s",type="o",col="blue")
plot(ts_timeFrame,main="Average time between frames, s",type="o",col="blue")
plot(ts_meanFrame,main="The average frame size, byte",type="o",col="blue")
plot(ts_sdFrame,main="Standart deviation of the frame size, byte",type="o",col="blue")
#plot(ts_volFrame,main="Объем полученных данных, байт/с",type="o")
#plot(ts_countFrame,main="Число принятых пакетов, 1/с",type="o")
#plot(ts_timeFrame,main="Среднее время между кадрами, с",type="o")
#plot(ts_meanFrame,main="Средний размер кадра, байт",type="o")
#plot(ts_meanFrame,main="Среднеквадратичное отклонение размера кадра, байт",type="o")



