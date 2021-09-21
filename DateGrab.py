from datetime import date
import pandas as pd
import csv
import glob
import pandas as pd
import pathlib
import os
import time
import datetime
import yfinance as yf
import clearing


##################################################################

df = pd.read_csv("LearningData\AAL_data.csv")
today = date.today()
bottom = df.tail(1)


selact_date = df.iloc[-1, 0]
split_date = selact_date.split('-')

print (split_date)
print (selact_date)

todays_year = today.strftime("%Y")
todays_month = today.strftime("%m")
todays_day = today.strftime("%d")

print (today)
print (todays_year)
print (todays_month)
print (todays_day)



if split_date[0] < todays_year :
    print ("o rok jinde")
    n = 1
else:
    if split_date[1] < todays_month :
        print ("o měsíc jinde")
        n = 1
    else:
        if split_date[2] < todays_day :
            print ("o den jinde")
            n = 1
        else :
            print ("same day ")

if n == 1 :
    path = 'LearningData'#'test'
    test = 'test'
    ListOfTickers = []
    ListOfFiles = []

    obj = os.scandir(path)
    test = os.scandir(test)

    print("files in direcotry '% s':" % path)
    for entry in obj :
        if entry.is_dir() or entry.is_file():
            ListOfTickers.append(entry.name.split('_')[0])
            ListOfFiles.append(entry.name)


    #print(ListOfTickers[0:]) 
    print(ListOfFiles[0:])
#Ticker loading and stoaring


    for x in ListOfTickers:


#get data on this ticker
        tickerData = yf.Ticker(x)


#get the historical prices for this ticker
        tickerDf = tickerData.history(period='1d', start='1999-01-01', end=today)

    #print (tickerDf)
        with open('test/{}.csv'.format(x), 'w')as f_output:
            tickerDf = tickerDf.iloc[: , :-2]
            tickerDf.to_csv(f_output, line_terminator='\n')



