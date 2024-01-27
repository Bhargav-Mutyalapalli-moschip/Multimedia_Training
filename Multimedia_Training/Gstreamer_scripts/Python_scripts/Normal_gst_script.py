import subprocess
import time
#This resolutions is for testing
Resolution = [(176,144),(320,240),(640,480),(1280,720),(1920,1080),(2560,1440),(3840,2160),(7680,4320)]

#It is the function to run testcases with desired resolutions
def run_gstreamer_pipeline(pipeline, duration=10):
    try:
        # Run the pipeline in a subprocess
        start_time = time.time()
        process = subprocess.Popen(pipeline, shell=True)
        process.communicate(timeout=10)
        end_time = time.time()

        # Calculate the execution time
        execution_time = end_time - start_time

        # Check if the process exited successfully within the time limit
        if process.returncode == 0 and execution_time <= 10:
           return "Pass"
        else:
           #return "Fail"
    except:



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
        print("ret:",ret)
        if(ret == "Pass"):
            write_result_to_file(f"Test Case {width}x{height} :   Pass")
        else:
            write_result_to_file(f"Test Case {width}x{height} :   Fail")



if __name__ == "__main__":
    test_cases()

