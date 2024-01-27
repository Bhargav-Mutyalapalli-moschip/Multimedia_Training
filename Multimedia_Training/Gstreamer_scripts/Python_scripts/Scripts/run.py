import subprocess

resolutions = [
    (640, 480),
    (720, 480),
    (3840, 2160),
    (176, 144),
    # Add more resolutions as needed
]

for width, height in resolutions:
    pipeline = f"gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! autovideosink"
    
    try:
        subprocess.run(pipeline, shell=True, check=True)
        
    except subprocess.CalledProcessError as e:
        print(f"Error running pipeline for resolution {width}x{height}: {e}")

