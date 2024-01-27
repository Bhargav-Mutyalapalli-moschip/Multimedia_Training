import subprocess

def run_gstreamer_pipeline(pipeline):
    try:
        subprocess.run(pipeline, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running GStreamer pipeline: {e}")

def test_cases():
    # Test Case 1: Default pipeline
    pipeline_1 = "gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12 ! autovideosink"
    
    # Test Case 2: Change resolution
    pipeline_2 = "gst-launch-1.0 videotestsrc ! video/x-raw,width=1280,height=720,format=NV12 ! autovideosink"
    
    # Test Case 3: Add a filter or element
    pipeline_3 = "gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12 ! autovideosink textoverlay text=\"Test Overlay\""

    # Add more test cases as needed...

    # Run test cases
    run_gstreamer_pipeline(pipeline_1)
    run_gstreamer_pipeline(pipeline_2)
    run_gstreamer_pipeline(pipeline_3)

if __name__ == "__main__":
    test_cases()

