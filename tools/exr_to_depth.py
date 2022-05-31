import sys
from pathlib import Path
import numpy as np
from PIL import Image, ImageCms
import matplotlib.pyplot as plt
import minexr

z_near = 5.0
z_far = 20.0

def main():
	exr_path = Path(__file__).parent.parent / sys.argv[1]
	with open(exr_path, 'rb') as fp:
		reader = minexr.load(fp)
		rgb = reader.select(['Color.R','Color.G','Color.B']).astype(np.float32)
		depth = reader.select(['Depth.V']).astype(np.float32)

		#plot_maps(rgb, depth)

		depth_norm = (1.0/depth.clip(min = z_near, max = z_far) - 1.0/z_near) / (1.0/z_far - 1.0/z_near)

		save_depth(depth_norm.squeeze())

def plot_maps(rgb, depth):
	fig, axs = plt.subplots(1,2, figsize=(9,3))
	axs[0].imshow(rgb)
	axs[0].set_title('Linear Color')
	axs[0].set_axis_off()
	axs[1].imshow(depth[...,0], cmap='gray_r', vmin=1.0, vmax=300.0)
	axs[1].set_title('Linear Depth')
	axs[1].set_axis_off()
	plt.tight_layout()
	plt.show()

def save_depth(depth):

	# Creating sRGB profile
	#profile = ImageCms.createProfile("LAB")
	#profile_cms = ImageCms.ImageCmsProfile(profile)

	buffer_array_unint16 = (depth * np.iinfo(np.uint16).max).astype(np.uint16)

	depth_image = Image.new("I", depth.T.shape)
	depth_image.frombytes(buffer_array_unint16, 'raw', "I;16")
	depth_image.save("depth.png")

	#depth_image = Image.fromarray(depth_unint16.squeeze(), mode="L", bits=16)
	#depth_image.save("depth.png", icc_profile=profile_cms.tobytes())


if __name__ == '__main__':
	main()