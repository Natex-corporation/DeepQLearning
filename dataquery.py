import csv
import glob
import pandas as pd
import pathlib
import os
import time
import datetime
import yfinance as yf

path = 'LearningData'
ListOfTickers = []
ListOfFiles = []

obj = os.scandir(path)

print("files in direcotry '% s':" % path)
for entry in obj :
    if entry.is_dir() or entry.is_file():
        ListOfTickers.append(entry.name.split('_')[0])
        #print(entry)
        ListOfFiles.append(entry.name)


print(ListOfTickers[0:]) 
print(ListOfFiles[0:])
#Ticker loading and stoaring

for x in ListOfTickers:


#get data on this ticker
    tickerData = yf.Ticker(x)

#get the historical prices for this ticker
    tickerDf = tickerData.history(period='1d', start='1999-11-18', end='2020-1-25')

    #print (tickerDf)
    with open('{}.csv'.format(x), 'w')as f_output:
        tickerDf = tickerDf.iloc[: , :-1]
        tickerDf.to_csv(f_output)
       
#Gathering important date for each ticker
#Data writing in to coresponding files



#Timing script to execute every day at the end

obj.close()
        
