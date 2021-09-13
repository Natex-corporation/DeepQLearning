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

class YahooAPI(object):
    def __init__(self) -> None:
        super().__init__()  

f = open ('LearningFiles/A_Data.csv', 'w')

writer = csv.writer(f)


writer.writerow(row)
f.close()

obj.close()
        
