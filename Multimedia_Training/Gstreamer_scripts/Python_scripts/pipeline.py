import subprocess

def run_gstreamer_pipeline(pipeline):
    try:
        # Construct the command to run GStreamer pipeline
        command = f"gst-launch-1.0 {pipeline}"

        # Run the command using subprocess
        subprocess.run(command, shell=True, check=True)

    except subprocess.CalledProcessError as e:
        print(f"Error running GStreamer pipeline: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    # Replace the following with your GStreamer pipeline string
    my_pipeline = "videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12 ! autovideosink"

    # Run the GStreamer pipeline
    run_gstreamer_pipeline(my_pipeline)

