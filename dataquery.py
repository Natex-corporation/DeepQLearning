import csv
import glob
import pandas as pd
import pathlib
import os
import time
import datetime

path = 'LearningData'
ListOfTickers = []

obj = os.scandir(path)

print("files in direcotry '% s':" % path)
for entry in obj :
    if entry.is_dir() or entry.is_file():
        ListOfTickers.append(entry.name.split('_')[0])

print(ListOfTickers[0:]) 
<<<<<<< Updated upstream
=======
print(ListOfFiles[0:])
#Ticker loading and stoaring

for x in ListOfTickers:
#get data on this ticker
    tickerData = yf.Ticker(x)
>>>>>>> Stashed changes

class YahooAPI(object):
    def __init__(self) -> None:
        super().__init__()  

<<<<<<< Updated upstream
f = open ('LearningFiles/A_Data.csv', 'w')
=======
    #print (tickerDf)
    with open('{}.csv'.format(x), 'w')as f_output:
        tickerDf = tickerDf.iloc[: , :-2]
        tickerDf.to_csv(f_output)
       
#Gathering important date for each ticker
#Data writing in to coresponding files
>>>>>>> Stashed changes

writer = csv.writer(f)


writer.writerow(row)
f.close()

obj.close()
        
