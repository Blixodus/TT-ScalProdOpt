import ast
import multiprocessing
import os
import sys
import time
import subprocess
import configparser
from dataclasses import dataclass

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns

from alive_progress import alive_bar
    
cores = min(40, multiprocessing.cpu_count())
pool = multiprocessing.Pool(processes=cores)