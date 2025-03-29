import numpy as np
from skimage import filters, segmentation, measure, transform
import csv
import argparse
# import matplotlib as plt

def collect_segment_points(segmented_image, unique_labels):
    segment_points = {label: [] for label in unique_labels}
    
    for row in range(segmented_image.shape[0]):
        for col in range(segmented_image.shape[1]):
            label = segmented_image[row, col]
            segment_points[label].append((row, col))
            
    return segment_points

def collect_segment_values(segmented_image, img, unique_labels):
    segment_values = {label: [] for label in unique_labels}
    
    for row in range(segmented_image.shape[0]):
        for col in range(segmented_image.shape[1]):
            label = segmented_image[row, col]
            segment_values[label].append(img[row, col])
    
    return segment_values

def write_metadata(rows, cols, old_header):
    with open('metadata.csv', 'w') as file:
        file.write(f"nrows         {rows}\n")
        file.write(f"ncols         {cols}\n")
        file.write(f"xllcorner     {old_header['xllcorner']}\n")
        file.write(f"yllcorner     {old_header['yllcorner']}\n")
        file.write(f"cellsize      {old_header['cellsize']}\n")
        file.write(f"NODATA_value  {old_header['NODATA_value']}\n")

def write_segment_to_file(label, points, values):
    points_str = [f'{p[0]},{p[1]}' for p in points]
    filename = f'output_segments/segment_{label}.csv'
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file, delimiter=' ')
        writer.writerow(['NewPointsSize', len(points)])
        writer.writerow(['ValuesSize', len(values)])
        writer.writerow(['NewPoints'] + points_str)
        writer.writerow(['Values'] + [f"{value:.2f}" for value in values])

def normalize_dem(dem):
    min_val = np.nanmin(dem)
    max_val = np.nanmax(dem)
    normalized_dem = (dem - min_val) / (max_val - min_val)
    return normalized_dem 

def downsample_dem_by_percentage(dem, percentage):
    if percentage <= 0 or percentage >= 100:
        raise ValueError("Percentage must be between 0 and 100.")
    
    scale_factor = percentage / 100
    new_height = int(dem.shape[0] * scale_factor)
    new_width = int(dem.shape[1] * scale_factor)
    
    return transform.resize(dem, (new_height, new_width), mode='edge', anti_aliasing=True, preserve_range=True)

def read_asc(filename):
    header = {}
    with open(filename, 'r') as file:
        for _ in range(6):
            line = file.readline()
            key, value = line.split()
            header[key] = float(value)
        data = np.loadtxt(file)
    return header, data

parser = argparse.ArgumentParser(description="")
parser.add_argument("input", help="Path to the input DEM")
parser.add_argument("-ds", "--downsample_ratio", type=int, default=100, help="Downsample ratio before performing segmentation.")
parser.add_argument("-hi", "--out_height", type=int, default=600, help="Height of output DEM.")
parser.add_argument("-wi", "--out_width", type=int, default=600, help="Width of output DEM.")
args = parser.parse_args()

header, dem = read_asc(args.input)

if args.downsample_ratio < 100:
    downsampled_dem = downsample_dem_by_percentage(dem, args.downsample_ratio)
else:
    downsampled_dem = dem

normalized_dem = normalize_dem(downsampled_dem)
smoothed_dem = filters.gaussian(normalized_dem, sigma=1)

gradient = filters.sobel(smoothed_dem)

thresh = filters.threshold_otsu(smoothed_dem)
markers = np.zeros_like(smoothed_dem)
markers[smoothed_dem < thresh] = 1
markers[smoothed_dem >= thresh] = 2
markers = measure.label(markers)

labels = segmentation.watershed(gradient, markers)
resized_labels = transform.resize(labels, (args.out_height, args.out_width), order=0, anti_aliasing=False, preserve_range=True)
unique_labels = np.unique(resized_labels)

segment_points = collect_segment_points(resized_labels, unique_labels)
segment_values = collect_segment_values(labels, downsampled_dem, np.unique(labels))
    
write_metadata(args.out_height, args.out_width, header)

total_points = 0
total_values = 0
for label in segment_points:
    total_points += len(segment_points[label])
    total_values += len(segment_values[label])
    write_segment_to_file(label, segment_points[label], segment_values[label])

print("New points size:", total_points)
print("Old points size:", total_values)
print("Number of segments:", len(unique_labels))

# norm_dem = cv2.normalize(downsampled_dem, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)
# plt.imshow(norm_dem, cmap='gray')
# plt.show()
