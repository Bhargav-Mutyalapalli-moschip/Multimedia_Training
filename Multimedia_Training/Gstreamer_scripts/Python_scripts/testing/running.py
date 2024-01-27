import subprocess
import time

def run_pipeline(width, height):
    # Define the gst-launch-1.0 command
    pipeline_command = f'gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! autovideosink'

    # Run the command and capture output and error
    try:
        result = subprocess.run(
            pipeline_command,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=10,
            text=True
        )

        # Check if the process exited successfully within the time limit
        if result.returncode == 0 and result.stderr.strip() == '':
            return "Pass"
        else:
            return f"Fail - Error: {result.stderr.strip()}"

    except subprocess.TimeoutExpired:
        return "Fail - Timeout"

def main():
    resolutions = [(176, 144), (320, 240), (640, 480), (1280, 720), (1920, 1080), (2560, 1440), (3840, 2160), (7680, 4320)]

    with open("result.txt", "w") as result_file:
        for width, height in resolutions:
            result = run_pipeline(width, height)
            result_file.write(f"Resolution: {width}x{height}, Result: {result}\n")

if __name__ == "__main__":
    main()

