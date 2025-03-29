import numpy as np
from skimage import filters
import argparse

def read_asc(filename):
    header = {}
    with open(filename, 'r') as file:
        for _ in range(6):
            line = file.readline()
            key, value = line.split()
            header[key] = float(value)
        data = np.loadtxt(file)
    return header, data

def write_asc(filename, header, data):
    with open(filename, 'w') as file:
        file.write(f"nrows         {int(header['nrows'])}\n")
        file.write(f"ncols         {int(header['ncols'])}\n")
        file.write(f"xllcorner     {header['xllcorner']}\n")
        file.write(f"yllcorner     {header['yllcorner']}\n")
        file.write(f"cellsize      {header['cellsize']}\n")
        file.write(f"NODATA_value  {header['NODATA_value']}\n")
        np.savetxt(file, data, fmt="%1.2f")

def gauss_smooth_dem(data, sigma=1):
    return filters.gaussian(data, sigma=sigma, mode='nearest', preserve_range=True)

def percentile_balancing(data):
    p25 = np.percentile(data, 25)
    p75 = np.percentile(data, 75)
    median = np.median(data)
    
    modified_elevation = data.copy()
    
    modified_elevation[data > p75] *= 0.7
    
    mask = (data > median) & (data <= p75)
    modified_elevation[mask] *= 0.85
    
    modified_elevation[data < p25] *= 1.3
    
    mask = (data >= p25) & (data < median)
    modified_elevation[mask] *= 1.15
    
    return modified_elevation, p25, median, p75

def process_dem(input_filename, output_filename, sigma=1, method='gaussian'):
    header, data = read_asc(input_filename)
    if method == 'gaussian':
        processed_data = gauss_smooth_dem(data, sigma=sigma)
    elif method == 'balancing':
        processed_data, p25, median, p75 = percentile_balancing(data)
        print(f'p25: {p25}, median: {median}, p75: {p75}')
    else:
        raise 'Unsupported method. Choose `gaussian` or `balancing`'
    write_asc(output_filename, header, processed_data)

parser = argparse.ArgumentParser(description="")
parser.add_argument("input", help="Path to the input DEM")
parser.add_argument("output", help="Path to the output DEM")
parser.add_argument("-m", "--method", default='balancing', help="Post processing method: `balancing` or `gaussian`. Default: `balancing`")
parser.add_argument("-gs", "--gauss_sigma", type=int, default=2, help="Sigma for Gaussian blur method.")
args = parser.parse_args()

process_dem(args.input, args.output, sigma=args.gauss_sigma, method=args.method)
