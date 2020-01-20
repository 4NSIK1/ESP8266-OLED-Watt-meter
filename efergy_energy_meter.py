# efergy_energy_meter.py
# Python 3
# Robert Fearn (c) 2020
# Access you energy meter reading via the REST API of EFERGY
# You first need to register at https://engage.efergy.com/user/register then register your Efergy CT Sensor and network hub
# You will be given a API Token to be used inserted in the code below
# You can then view your power usgae or history via https://engage.efergy.com/dashboard or use the code below to access details such as
# Power
# Time
# Efergy CT sensor battery level
# Hub MAC address
# This allows you to create your own dashboard or statistics and great at finding what applicance and standby power usage
#
# The second part of the project is running the same code on a stand alone ESP8266 Wemos micro with OLED display,
# so we can see the power usage (not from a computer) 
# This code is a simple demo of the functions and has a short loop to periodically access the data. 
# Depending on your CT update period (mine is set at 6 seconds) you can adjust the sampling time.
#
# inspired by https://forums.indigodomo.com/viewtopic.php?f=107&t=13827

import requests
import json
import datetime
import time
import sys

API_TOKEN='Your API from Effergy'

# Main function that gathers and parses the data
def get_power_readings(api_token):
    power=0
    last_reading_time=0
    try:
        url='https://engage.efergy.com/mobile_proxy/getStatus?token='
        url1Returned = requests.get(url + api_token)
    except requests.exceptions.RequestException as e:  # This is the correct syntax
        print ("Error accessing "+url+ " Error: "+ e)
        sys.exit(1)
    try:
        url='https://engage.efergy.com/mobile_proxy/getInstant?token='
        url2Returned = requests.get(url + api_token)
    except requests.exceptions.RequestException as e:  # This is the correct syntax
        print ("Error accessing "+url+ " Error: "+ e)
        sys.exit(1)
    try:
        url='http://www.energyhive.com/mobile_proxy/getCurrentValuesSummary?token='
        url3Returned = requests.get(url + api_token)
    except requests.exceptions.RequestException as e:  # This is the correct syntax
        print ("Error accessing "+url+ " Error: "+ e)
        sys.exit(1)        
    
    if url1Returned.status_code==200:
        data_getStatus = url1Returned.content
        dict1 = json.loads(data_getStatus.decode())
        battery=str(dict1['listOfMacs'][0]['listofchannels'][0]['type']['battery']) #battery level
        falseBattery=str(dict1['listOfMacs'][0]['listofchannels'][0]['type']['falseBattery'])
        mac=str(dict1['listOfMacs'][0]['mac']) #device mac address
        
    if url2Returned.status_code==200:
        data_getInstant  = url2Returned.content
        dict2 = json.loads(data_getInstant.decode())
        last_reading_ts=float(dict2['last_reading_time']) #value of checked date and time in ticks
        age=str(dict2['age']) #age of last reading in seconds
        power=str(round(int(dict2['reading']),1)) 
        last_reading_time = datetime.datetime.fromtimestamp(last_reading_ts/1000).strftime('%Y-%m-%d %H:%M:%S')
        
    if url3Returned.status_code==200:
        data_getSummary  = url3Returned.content
        dict3 = json.loads(data_getSummary.decode())
    return power,last_reading_time

#Sample loop to display power and timestamp
for i in range (10):
    pwr,time1=get_power_readings(API_TOKEN)
    print("At "+str(time1)+ " your power usage was "+str(pwr) +" W")
    sys.stdout.flush()
    time.sleep(10)
