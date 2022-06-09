import os, subprocess, sys
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent
BIN_DIR = 'bin/Debug'
BUILD_DIR = 'build'
ASSETS_DIR = 'assets'

def main():
	assets_path = sys.argv[1]
	bin_path = ROOT_DIR / (BIN_DIR + '/')

	if not os.path.exists(assets_path):
		print('Asset directory does not exist')
		return

	if not os.path.exists(bin_path):
		print('Please compile application first to generate tools')
		return

	for root, subdirs, files in os.walk(assets_path):
		for file in files:
			# Make directories in build folder
			output_dir = ROOT_DIR / ASSETS_DIR / os.path.relpath(root, assets_path)
			if not os.path.exists(output_dir):
				os.makedirs(output_dir)

			source_file = Path(root) / file

			if file.endswith('.mat'):
				output_file = Path(output_dir) / file.replace('.mat', '.filamat')
				print('Compiling {}'.format(file))
				subprocess.run([bin_path / 'matc.exe', '-o', output_file, source_file])
			
	
if __name__ == '__main__':
	main()