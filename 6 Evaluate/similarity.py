import sys
import rasterio
from sklearn.metrics import mean_absolute_error as mae, root_mean_squared_error as rmse
from skimage.metrics import structural_similarity as ssim, peak_signal_noise_ratio as psnr

def main():
    args = sys.argv[1:]

    if len(args) != 2:
        print('''TypeError: Missing arguments
usage: similarity.py <input DEM> <output/generated DEM>''')
        exit()
    
    print("========= Opening files =========")
    try:
        dem1 = rasterio.open(args[0])
    except:
        print("IOError: " + args[0] + ": No such file or directory")
        exit()

    try:
        dem2 = rasterio.open(args[1])
    except:
        print("IOError: " + args[1] + ": No such file or directory")
        exit()

    terrain1 = dem1.read(1)
    terrain2 = dem2.read(1)

    print("======= Comparing terrain =======")

    data_range = max(terrain1.max(), terrain2.max()) - min(terrain1.min(), terrain2.min())

    mae_score = mae(terrain1, terrain2)
    rmse_score = rmse(terrain1, terrain2)
    ssim_score = ssim(terrain1, terrain2, data_range=data_range)
    psnr_score = psnr(terrain1, terrain2, data_range=data_range)

    # Results
    print(f"MAE score  : {round(mae_score, 3):<10} (inf-0)")
    print(f"RMSE score : {round(rmse_score, 3):<10} (inf-0)")
    print(f"SSIM score : {round(ssim_score, 3):<10} (0-1)")
    print(f"PSNR score : {round(psnr_score, 3):<10} (0-inf)")
    

if __name__ == '__main__':
    main()