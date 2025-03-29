param([float]$simplex=0.5)

# segmentation parameters
$input_dem = "../1 Data/kings_canyon_input.asc"
$out_width = 593
$out_height = 375
$input_downscale_ratio = 100

# noise generator parameters
$simplex_weight = $simplex
$max_altitude = -1 # set to -1 to use automated max altitude
$octaves = 8
$lacunarity = 0.5
$amplitude = 2.0

$conda_env = "designer-world"

echo "Start | Simplex Weight: $simplex_weight"

Set-Location "2 Segment"
New-Item -ItemType Directory -Path "./output_segments" -Force | Out-Null
Get-ChildItem "./output_segments" -File | Remove-Item -Force

conda activate $conda_env # comment if unnecessary
python segment.py $input_dem -ds $input_downscale_ratio -wi $out_width -hi $out_height | Out-Null
conda deactivate # comment if unnecessary

Set-Location "..\3 Analyze"
g++ main.cpp -o main.exe | Out-Null

Set-Location "..\4 Generate"
g++ simplexnoise.cpp valuenoise.cpp main.cpp -o main.exe -l:libwinmm.a | Out-Null

New-Item -ItemType Directory -Path "./output_segments" -Force | Out-Null
Get-ChildItem "./output_segments" -File | Remove-Item -Force | Out-Null

$segmentPath = Join-Path (Get-Location) "../2 Segment/output_segments"
New-Item -ItemType Directory -Path "./output_segments" -Force  | Out-Null
Get-ChildItem $segmentPath -File | ForEach-Object {
    $filepath = $_.FullName
    $filename = (Get-Item $filepath).Name
    
    Set-Location "../3 Analyze"
    & .\main.exe "$filepath" | Out-Null

    Set-Location "../4 Generate"
    & .\main.exe "./output_segments/$filename" $simplex_weight $max_altitude $octaves $lacunarity $amplitude | Out-Null
}

Copy-Item -Path "../2 Segment/metadata.csv" -Destination "../5 Finalize/output.asc"
Set-Location "..\5 Finalize"
conda activate $conda_env # comment if unnecessary
python merge.py
python postprocess.py output.asc smooth-output.asc -m gaussian
conda deactivate # comment if unnecessary

Set-Location "..\6 Evaluate"
Copy-Item -Path $input_dem -Destination "input.asc"

conda activate $conda_env
python similarity.py input.asc "../5 Finalize/smooth-output.asc"
conda deactivate

Set-Location $PSScriptRoot
