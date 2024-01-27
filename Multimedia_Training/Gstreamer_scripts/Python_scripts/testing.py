import subprocess

resolutions = [
    (640, 480)
    # Add more resolutions as needed
]

for width, height in resolutions:
    pipeline = f"gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! autovideosink"
    
    try:
        subprocess.run(pipeline, shell=True, check=True)
        print(f"Pipeline for resolution {width}x{height} saved to {output_file}")
    except subprocess.CalledProcessError as e:
        print("Except is invoked\n")
        print(f"Error running pipeline for resolution {width}x{height}: {e}")

