import subprocess

resolutions = [
    (640, 480),
    (720, 480),
    (3840, 2160),
    (176, 144),
    # Add more resolutions as needed
]

for width, height in resolutions:
    output_file = f"output_{width}x{height}.mp4"
    pipeline = f"gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! filesink location={output_file}"
    
    try:
        subprocess.run(pipeline, shell=True, check=True)
        print(f"Pipeline for resolution {width}x{height} saved to {output_file}")
    except subprocess.CalledProcessError as e:
        print(f"Error running pipeline for resolution {width}x{height}: {e}")

