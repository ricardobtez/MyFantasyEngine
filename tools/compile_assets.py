import os, subprocess
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent
BIN_DIR = 'bin/Debug'
BUILD_DIR = 'build'
ASSETS_DIR = 'assets'

def main():
	assets_path = ROOT_DIR / (ASSETS_DIR + '/')
	bin_path = ROOT_DIR / (BIN_DIR + '/')

	for root, subdirs, files in os.walk(assets_path):
		for file in files:
			# Make directories in build folder
			index = root.find(ASSETS_DIR)
			output_dir = root[:index] + BUILD_DIR + '\\' + root[index:]
			if not os.path.exists(output_dir):
				os.makedirs(output_dir)

			source_file = Path(root) / file

			if file.endswith('.mat'):
				output_file = Path(output_dir) / file.replace('.mat', '.filamat')
				subprocess.run([bin_path / 'matc.exe', '-o', output_file, source_file])
			
	
if __name__ == '__main__':
	main()