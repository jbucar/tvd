import yaml
import os 

CURRENT_DIR=os.path.dirname(os.path.abspath(__file__))
CONFIG_FILENAME=os.path.join(CURRENT_DIR, 'config.json')

config = []
with open(CONFIG_FILENAME) as data_file:
	config = yaml.load(data_file)

