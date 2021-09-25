import os
import pandas as pd


path = 'test'
ListOfTickers = []

obj = os.scandir(path)
print("files in direcotry '% s':" % path)

for entry in obj :
        if entry.is_dir() or entry.is_file():
            ListOfTickers.append(entry.name.split('_')[0])

print(ListOfTickers[0:])

for x in ListOfTickers:
    df = pd.read_csv('test/{}'.format(x))
    #print (df)
    if df.empty == True:
        os.remove('test/{}'.format(x))
        print ("empty")
 