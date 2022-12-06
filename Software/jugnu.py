#! /usr/bin/env python

import argparse
import requests
from datetime import datetime


class Jugnu:
    """
    """
    def __init__(self, url):
        """
        """
        self._apiurl = None
        self.apiURL(url)

    def identify(self):
        """
        :return:
        """
        cmd = "name"
        res = requests.get(self._apiurl + cmd)
        return res.json()

    def apiURL(self, url):
        """
        New API URL
        :param url:
        :type url: str
        :return:
        """
        if url[-1] != '/':
            url = url + "/"
        self._apiurl = url
        return

    def getAPIURL(self):
        """
        :return: current API URL
        :rtype: str
        """
        return self._apiurl

    def getTime(self):
        """
        :return:
        """
        cmd = "time"
        print(self._apiurl + cmd)
        res = requests.get(self._apiurl + cmd)
        return res.json()

    def syncTime(self):
        """
        :return:
        """
        cmd = "set_time"
        now = datetime.now()

        time_data = {"h": now.hour,
                     "min": now.minute,
                     "s": now.second,
                     "dd": now.day,
                     "mm": now.month,
                     "yyyy": now.year}
        print(self._apiurl + cmd)
        res = requests.post(self._apiurl + cmd, json=time_data)
        return res.json()

    def getSwitchStat(self):
        """
        :return:
        """
        cmd = "sw_stat"
        print(self._apiurl + cmd)

        res = requests.get(self._apiurl + cmd)
        return res.json()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='Jugnu - wifi light controller',
        description='This command is used to initialize jugnu',
        epilog='Run "jugnu -h" for help')

    parser.add_argument('-t', '--gettime', action='store_true')
    parser.add_argument('-T', '--synctime', action='store_true')
    parser.add_argument('-g', '--getswitch', action='store_true')
    parser.add_argument('-u', '--apiurl', required=True)

    args = parser.parse_args()
    jugnu = Jugnu(args.apiurl)

    response = jugnu.identify()
    print("##############################################")
    for key, val in zip(response, response.values()):
        print(key, ":", val)
    print("##############################################")

    if args.gettime is True:
        print("##############################################")
        response = jugnu.getTime()
        for key, val in zip(response, response.values()):
            print(key, ":", val)
        print("##############################################")

    if args.synctime is True:
        print("##############################################")
        response = jugnu.syncTime()
        for key, val in zip(response, response.values()):
            print(key, ":", val)
        print("##############################################")

    if args.getswitch is True:
        print("##############################################")
        response = jugnu.getSwitchStat()
        for key, val in zip(response, response.values()):
            print(key, ":", val)
        print("##############################################")
