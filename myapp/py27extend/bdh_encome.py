#-*- coding: utf-8 -*-

import datetime;
import bdh;
import wxm;

everyhour_encome = (
    80,
    90,
    95
)

def hunt_everyhour_encome():
    return everyhour_encome

# python call c++
# user is user_t type
# pyadd_money_cb is bdh module function
def pyadd_money_cb(user, count):
    return bdh.add_money(user, count)

def pyattr_list_cb():
    res = wxm.attr_list()
    filename = 'pyattr_list_cb.conf'
    fnpath = filename
    print ('fnpath: ', fnpath)
    with open(fnpath,  'w+') as f:
        for i in res:
            f.write(i + '\n')
    f.close()