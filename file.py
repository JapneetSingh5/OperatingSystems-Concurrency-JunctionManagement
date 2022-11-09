import argparse
from random import randint as r
import os
import time
import subprocess
parser = argparse.ArgumentParser()
parser.add_argument('filename')
parser.add_argument('-l', '--length')
args = parser.parse_args()
list = ['N', 'S', 'W', 'E']
dirs = "".join([list[r(0,3)] for _ in range((int)(args.length))])
st = time.time()
st_p = time.process_time()
os.system(f"./{args.filename} {dirs} > temp.out")
et = time.time()
et_p = time.process_time()
status, message = subprocess.getstatusoutput("./trial temp.out")
if(status == 0):
  print("Program works")
else:
  print(f"Something wrong: {message}")
print(f"Program took {et - st} seconds, CPU time: {et_p - st_p} seconds")