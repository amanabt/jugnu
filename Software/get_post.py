#! /usr/bin/env python

import requests
from datetime import datetime

api_url = "http://192.168.43.194/"


cmd = "name"

res = requests.get(api_url+cmd)
print(res.json())


cmd = "sw_stat"
#sw_stat_data = {"sw_stat" : 1}

res = requests.get(api_url+cmd)
print(res.json())


cmd = "time"
res = requests.get(api_url+cmd)
print(res.json())


cmd = "set_time"
now = datetime.now()

time_data = {"h" : now.hour,
             "min" : now.minute,
             "s"   : now.second,
             "dd"  : now.day,
             "mm"  : now.month,
             "yyyy" : now.year}

print("##############################")
print(time_data)
print("##############################")

res = requests.post(api_url+cmd, json=time_data)
print(res.json())


#switch_stat = 0
#cmd = "set_sw"

#switch_stat = int(input("Set switch status to? (0/1): "))
#sw_stat_data = {"sw_stat" : switch_stat}

#res = requests.post(api_url+cmd, json=sw_stat_data)
#print(res.json())

