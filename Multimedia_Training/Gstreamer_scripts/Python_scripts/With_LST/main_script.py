import subprocess
import time

def read_lst_file(file_path):
    # Read configurations from the lst file
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Parse configurations from each line
    configurations = []
    for line in lines:
        values = line.strip().split(',')
        print("values:",values)
        if len(values) == 3:
            try:
                width, height = map(int, values[:2])
                formate = values[2]
                configurations.append({'width': width, 'height': height,'format':formate})
            except ValueError:
                print(f"Skipping invalid line: {line}")

    return configurations


def run_pipeline(width, height):
    # Define the gst-launch-1.0 command
    pipeline_command = f'gst-launch-1.0 videotestsrc ! video/x-raw,width={width},height={height},format=NV12 ! autovideosink'

    # Run the command and capture output and error
    try:
        result = subprocess.run(
            f'timeout 10 {pipeline_command}',  # Use timeout to limit execution time
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # Check if the process exited successfully
        print("code:",result.returncode)
        if result.returncode != 1 and result.stderr.strip() == '':
            return "Pass"
        else:
            return f"Fail - Error: {result.stderr.strip()}"

    except subprocess.CalledProcessError as e:
        return f"Fail - Error: {e.stderr.strip()}"
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

