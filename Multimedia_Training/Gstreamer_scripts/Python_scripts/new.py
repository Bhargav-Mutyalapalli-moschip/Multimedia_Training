import subprocess
import time
#This resolutions is for testing
Resolution = [(176,144),(320,240),(640,480),(1280,720),(1920,1080),(2560,1440),(3840,2160),(7680,4320)]

#It is the function to run testcases with desired resolutions
def run_gstreamer_pipeline(pipeline, duration=10):
    try:
        # Run the pipeline in a subprocess
        process = subprocess.Popen(pipeline, shell=True)
        process.wait(timeout=duration)  # Wait for the process to finish
        return process.returncode == 0  # Return True if the return code is 0 (success)

    except subprocess.CalledProcessError as e:
        print(f"Error running GStreamer pipeline: {e}")
        return False  # Return False if the pipeline fails


def write_result_to_file(result):
    with open("test_results.txt", "a") as file:
        file.write(result + "\n")


def test_cases():
    # Test Case 1: Default pipeline
    for width, height in Resolution:
        pipeline = f"gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! autovideosink"

        ret = run_gstreamer_pipeline(pipeline)
        if(ret == True):
            write_result_to_file(f"Test Case {width}x{height} :   Pass")
        else:
            write_result_to_file(f"Test Case {width}x{height} :   Fail")

    
    
if __name__ == "__main__":
    test_cases()

