import numpy as np
import os

def parse_csv(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        _ = int(lines[0].split()[1])
        points = lines[1].split()[1:] 
        values = list(map(float, lines[2].split()[1:])) 
        
        points = [tuple(map(int, point.split(','))) for point in points]
        
    return points, values

def create_matrix(folder_path):
    max_row = 0
    max_col = 0
    data_points = []

    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            points, _ = parse_csv(os.path.join(folder_path, filename))
            for x, y in points:
                max_row = max(max_row, x)
                max_col = max(max_col, y)
                data_points.append((x, y))
    
    matrix = np.full((max_row + 1, max_col + 1), np.nan)

    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            points, values = parse_csv(os.path.join(folder_path, filename))
            for (x, y), value in zip(points, values):
                matrix[x, y] = value

    return matrix

def save_matrix_to_file(matrix, output_path):
    nrows, ncols = matrix.shape
    with open(output_path, 'w') as file:
        file.write(f"nrows {nrows}\n")
        file.write(f"ncols {ncols}\n")
        file.write(f"xllcorner 0.0\n")
        file.write(f"yllcorner 0.0\n")
        file.write(f"cellsize 0.01\n")
        file.write(f"NODATA_value -99999\n")
        
        for row in matrix:
            line = ' '.join('nan' if np.isnan(x) else f'{x:.2f}' for x in row)
            file.write(line + '\n')

folder_path = '../4 Generate/output_segments'
matrix = create_matrix(folder_path)
save_matrix_to_file(matrix, "output.asc")